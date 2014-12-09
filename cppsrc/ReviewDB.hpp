#pragma once
#include <SQLAPI.h>
#include <future>

template<typename Input>
class ReviewParser;


template<typename Input>
class ReviewDB{

public:

	typedef typename ReviewParser<Input>::sets sets;

	std::string dbname;
	SAConnection con;

	ReviewDB(std::string dname):dbname(dname){
		try {
			con.Connect(SAString(dbname.c_str()),"research","researchVM",SA_MySQL_Client);
		}
		catch (SAException e){
			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}

	}

private:
	auto execute(SACommand &cmd){
		try {
			cmd.Execute();
		}
		catch (SAException e){
			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}
		
	}

	void extractReview(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb, SACommand& cmd, sets& sets){

		assert(cmd.isResultSet());

		SAField &summary = cmd.Field(1);
		SAField &score = cmd.Field(2);
		SAField &time = cmd.Field(3);
		SAField &reviewer = cmd.Field(4);
		SAField &helpyes = cmd.Field(5);
		SAField &helptot = cmd.Field(6);
		SAField &product = cmd.Field(7);
		SAField &text = cmd.Field(8);
		SAField &id = cmd.Field(9);

		std::list<Review::Memo> rml;
		while (cmd.FetchNext()){

			auto txt = std::string(text.asString().GetMultiByteChars());
			rml.emplace_back(
				id.asLong(),
				summary.asString().GetMultiByteChars(),
				rb.tok.tokenize<std::list<std::string> > (txt),
				score.asDouble(),
				time.asLong(),
				reviewer.asLong(),
				Helpfulness(helpyes.asLong(), helptot.asLong()),
				product.asLong(),
				txt);
		}
		
		getAllProducts(pb, rml, sets, [](auto &rm) {return rm.product.asInt();} );
		getAllReviewers(rrb, rml, sets, [](auto &rm) {return rm.reviewer.asInt();} );

		for (auto &rm : rml) sets.rs.insert(rm.unpack());
	}

	void extractProduct(Product::builder &, SACommand& cmd, sets &sets){
		assert(cmd.isResultSet());
		
		
		SAField &productID = cmd.Field(1);
		SAField &title = cmd.Field(2);
		SAField &price = cmd.Field(3);
		SAField &productType = cmd.Field(4);
		SAField &real_id = cmd.Field(5);

		std::list<Product::Memo> pml;
		while (cmd.FetchNext()){
			pml.emplace_back(
				real_id.asLong(),
				productID.asString().GetMultiByteChars(),
				title.asString().GetMultiByteChars(),
				price.asDouble(),
				productType.asString().GetMultiByteChars());
		}

		for (auto &pm : pml) sets.ps.insert(pm.unpack());
	}

	void extractReviewer (Reviewer::builder &, SACommand& cmd, sets &sets){
		assert(cmd.isResultSet());

		SAField &profileName = cmd.Field(1);
		SAField &userID = cmd.Field(2);
		SAField &real_id = cmd.Field(3);

		std::list<Reviewer::Memo> rrml;
		while (cmd.FetchNext()){
			rrml.emplace_back(
				real_id.asLong(),
				profileName.asString().GetMultiByteChars(),
				userID.asString().GetMultiByteChars());
		}

		for (auto &rrm : rrml) sets.rrs.insert(rrm.unpack());
	}


	void getField(std::string initial_cmd, std::string agg_command, SACommand &cmd, auto &rml, 
				  auto f = [](auto &rm){return rm;}, std::function<void (SACommand&) > setup = [](auto &){}){
		assert(rml.size() > 0);
		std::stringstream cstr(initial_cmd);
		for (uint i = 2; i < rml.size() + 1; ++i)
			cstr << agg_command << i;
		cmd.setCommandText(cstr.str().c_str());
		setup(cmd);
		for (auto &s : rml) cmd << f(s);
		execute(cmd);
	}

	void getAllProducts(Product::builder &pb, auto &rml, sets &sets, auto f = [](auto &rm){return rm;} ){
		SACommand cmd(&con);
		getField("select * from Product where productref = :1 ", " or productref = :", cmd, rml, f);
		extractProduct(pb, cmd, sets);
	}

	void getAllReviewers(Reviewer::builder &rb, auto &rml, sets &sets, auto f = [](auto &rm){return rm;} ){
		SACommand cmd(&con);
		getField("select * from Reviewer where id = :1 ", " or id = :", cmd, rml, f);
		extractReviewer(rb, cmd, sets);
	}
	

public:

	void getAllReviews(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb, 
					   const auto &textList, sets &sets, auto f = [](auto &s){return s;} ){
		SACommand cmd(&con);
		getField("select * from Review where text = :1 ", " or text = :", cmd, textList, [&f](auto &s){return f(s).c_str();});
		extractReview(rb, pb, rrb, cmd, sets);
	}


	void getAllReviews(Review::builder &rb, const Reviewer &r, sets &sets, const auto& caviats = {}){
		SACommand cmd(&con);
		getField("select * from Review where reviewer = :1", " and not id = :", cmd, r.reviews, 
				 [](auto &r){return r.id.asInt(); }, [&r](auto &cmd){cmd << r.id.asInt(); });
		extractReview(rb, cmd, sets);
	}

	void getAllReviews(Review::builder &rb, const Product &p, sets &sets) {
		SACommand cmd(&con); 
		getField("select * from Review where product = :1", " and not id = :", cmd, p.reviews, 
			[](auto &r){return r.id.asInt(); }, [&p](auto &cmd){cmd << p.id.asInt(); });
		extractReview(rb, cmd, sets);
	}


	auto getAllProducts(Product::builder &pb, const Reviewer &r, sets &sets, const auto &caviats = {}){
		std::string cstr("select productid, title, price, producttype, productref from (select id as rid from Reviewer where id=:1) as T1 left join Review on rid = reviewer left join Product on product = productref");
		SACommand cmd(&con);
		cmd.setCommandText(cstr.c_str());
		cmd << r.id.asInt();
		execute(cmd);
		return (extractProduct(pb,cmd,sets));
	}


	auto updateCategory(const std::string &product, const std::string &category){
		try {
			SACommand cmd (&con);
			cmd.setCommandText("update Product set ProductType=:1 where ProductID=:2;");
			cmd << category.c_str();
			cmd << product.c_str();
			execute(cmd);
		}
		catch (SAException e){
			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}
	}
	
	template <decltype(std::launch::deferred) launchpol = std::launch::deferred> 
	auto writeToDB(std::shared_ptr<ReviewDB> rp, std::shared_ptr<const sets> sp) {
		assert(rp.get() == this);
		auto &s = *sp;
		auto task1 = [&s, this](){ //reviewers
			SACommand cmd(&con);
			cmd.setCommandText("insert into Reviewer (profileName, userID, id) values (:1, :2, :3) ON DUPLICATE KEY UPDATE id=:3");
			for (const auto &rr : s.rrs){
				cmd << lock(rr)->profileName.c_str();
				cmd << lock(rr)->userID.c_str();
				cmd << lock(rr)->id.asInt();
				try{
					cmd.Execute();
				}
				catch (SAException e){
					std::string errtxt = e.ErrText().GetMultiByteChars();
					std::cerr << errtxt << std::endl;
					std::cerr << "Tried inserting this: " 
							  << lock(rr)->profileName.c_str() << ", " 
							  << lock(rr)->userID.c_str() << ", " 
							  << lock(rr)->id.asInt() << std::endl;
					sleep(5);
					throw e;
				}
			}
		};
		
		auto task2 = [&s,this](){ //Products
			
			SACommand cmd(&con);
			cmd.setCommandText("insert into Product (ProductID, Title, Price, ProductType, ProductRef) values (:1, :2, :3, :4, :5) ON DUPLICATE KEY UPDATE ProductRef=:5");
			for (const auto &p : s.ps){
				cmd << lock(p)->productID.c_str() << lock(p)->title.c_str() 
					<< lock(p)->price << lock(p)->productType.c_str() << lock(p)->id.asInt();
				try{
					cmd.Execute();
				}
				catch (SAException e){
					std::string errtxt = e.ErrText().GetMultiByteChars();
					std::cerr << errtxt << std::endl;
					std::cerr << "Tried inserting this: " << lock(p)->productID.c_str() << ", "
							  << lock(p)->title.c_str() << ", "
							  << lock(p)->price << ", "
							  << lock(p)->productType.c_str() << ", "
							  << lock(p)->id.asInt() << std::endl;
						
					sleep(5);
					throw e;
				}
			}
		};

		auto task3 = [&s, this](){ //reviews
			SACommand cmd(&con);
			cmd.setCommandText("insert into Review (Summary, Score, Time, Reviewer, HelpYes, HelpTotal, Product, Text, Id) values (:1, :2, :3, :4, :5, :6, :7, :8, :9) on duplicate key update HelpYes=:5");
			for (const auto &r : s.rs){
				long unsigned int ltime = r->time;
				long unsigned int lhf = r->help.first;
				long unsigned int lhs = r->help.second;
				cmd << r->summary.get().c_str() << r->score << ltime
					<< r->reviewer->id.asInt() << lhf << lhs
					<< r->product->id.asInt() << r->text.get().c_str() << r->id.asInt();
				try{
					cmd.Execute();
				}
				catch (SAException e){
					std::string errtxt = e.ErrText().GetMultiByteChars();
					std::cerr << errtxt << std::endl;
					std::cerr << "Tried inserting this: " 
							  << r->summary.get().c_str() << ", " << r->score << ", " << ltime << ", "
							  << r->reviewer->id.asInt() << ", " << lhf << ", " << lhs << ", "
							  << r->product->id.asInt() << r->text.get().c_str() << r->id.asInt() << std::endl;
					sleep(5);
					throw e;
				}
			}
		};

		auto fut1 = std::async(launchpol, task1).share();
		auto fut2 = std::async(launchpol, task2).share();
		auto fut3 = std::async(launchpol, task3).share();
		
		return std::async(launchpol,[rp,sp,fut1, fut2, fut3]() mutable {
				bool b = false;
				{
					auto rp2 = rp;
					rp.reset();
					fut1.get();
					fut2.get();
					fut3.get();
					sp.reset();
					b = rp2->isValidMem();
					rp.reset();
				}
				return b;
			});
	}
	
	static auto printException(SAException &e){
		std::string errtxt = e.ErrText().GetMultiByteChars();
		std::cerr << errtxt << std::endl;
		sleep(1);
	}

	auto isValidMem() const {
		return this;
	}

	void readFromDB(Reviewer::builder &, Product::builder &, Review::builder &, sets &){
		
	}


};
