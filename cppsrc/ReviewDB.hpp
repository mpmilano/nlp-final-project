#pragma once
#include <SQLAPI.h>
#include <future>
#include <execinfo.h>

template<typename Input>
class ReviewParser;


class ReviewDB{

public:

	typedef typename ReviewParser<std::istream>::sets sets;

	std::string dbname;
	SAConnection con;

	ReviewDB(std::string dname):dbname(dname) {
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


	void try_wrap(auto &f){
		try{
			f();
		}
		catch (SAException e){
			void *array[128];
			size_t size;
			size = backtrace(array, 128);
			backtrace_symbols_fd(array, size, STDERR_FILENO);

			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}
	}

	void try_wrap(auto f){
		try{
			f();
		}
		catch (SAException e){
			void *array[128];
			size_t size;
			size = backtrace(array, 128);
			backtrace_symbols_fd(array, size, STDERR_FILENO);

			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}
	}

private:

	auto execute(SACommand &cmd){
		try {
			int nBulkSize = 1000;
			SAString sBulkSize;
			sBulkSize.Format("%d", nBulkSize);
			cmd.setOption("PreFetchRows") = sBulkSize;
			cmd.Execute();
		}
		catch (SAException e){
			void *array[128];
			size_t size;
			size = backtrace(array, 128);
			backtrace_symbols_fd(array, size, STDERR_FILENO);
			std::string errtxt = e.ErrText().GetMultiByteChars();
			std::cerr << errtxt << std::endl;
			sleep(5);
			throw e;
		}
		
	}

	void extractReview(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb, SACommand& cmd, sets& sets){

		assert(cmd.isResultSet());
		
		int i = 0;

		std::cout << "result set has this many things in it: " << cmd.RowsAffected() << std::endl;

		std::list<Review::Memo> rml;
		while (cmd.FetchNext()){
			++i;
			SAField &summary = cmd.Field(1);
			SAField &score = cmd.Field(2);
			SAField &time = cmd.Field(3);
			SAField &reviewer = cmd.Field(4);
			SAField &helpyes = cmd.Field(5);
			SAField &helptot = cmd.Field(6);
			SAField &product = cmd.Field(7);
			SAField &text = cmd.Field(8);
			SAField &id = cmd.Field(9);

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

		std::cout << "we extracted this many reviews: " << rml.size() << std::endl;
		
		getAllProducts(pb, rml, sets, [](auto &rm) {return rm.product.asInt();} );
		getAllReviewers(rrb, rml, sets, [](auto &rm) {return rm.reviewer.asInt();} );

		for (auto &rm : rml) sets.rs.insert(rm.unpack());
	}

	void extractProduct(Product::builder &, SACommand& cmd, sets &sets){
		assert(cmd.isResultSet());
		
		

		std::list<Product::Memo> pml;
		while (cmd.FetchNext()){
			SAField &productID = cmd.Field(1);
			SAField &title = cmd.Field(2);
			SAField &price = cmd.Field(3);
			SAField &productType = cmd.Field(4);
			SAField &real_id = cmd.Field(5);

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

		std::list<Reviewer::Memo> rrml;
		while (cmd.FetchNext()){
			SAField &profileName = cmd.Field(1);
			SAField &userID = cmd.Field(2);
			SAField &real_id = cmd.Field(3);
			
			rrml.emplace_back(
				real_id.asLong(),
				profileName.asString().GetMultiByteChars(),
				userID.asString().GetMultiByteChars());
		}

		for (auto &rrm : rrml) sets.rrs.insert(rrm.unpack());
	}


	template<typename List>
	void getField(std::string initial_cmd, std::function<std::string (int) > agg_command, SACommand &cmd, List &rml, 
				  std::function<void (SACommand&, const typename List::value_type&) > f = 
				  [](SACommand &cmd, const typename List::value_type &rm){cmd << rm;}, 
				  std::function<void (SACommand&) > setup = noop_f<SACommand>, 
				  int end = 1, int start = 2){
		assert(setup != nullptr);
		assert(rml.size() > 0);
		std::stringstream cstr;
		cstr << initial_cmd;
		uint i = start;
		for (; i < rml.size() + end; ++i)
			cstr << agg_command(i);
		assert(i >= rml.size());
		cmd.setCommandText(cstr.str().c_str());
		setup(cmd);
		try {
			for (auto &s : rml) f(cmd, s);
		}
		catch (SAException e){
			std::cerr << "This was our intended query string: " << cstr.str() << std::endl;
			std::cerr << "This was our collection size: " << rml.size() << std::endl;
			try_wrap([&](){throw e;});
		}
		execute(cmd);
	}

	void getAllProducts(Product::builder &pb, auto &rml_i, sets &sets, auto f = id_f ){
		assert(rml_i.size() > 0);

		std::list<long> rml;

		for (auto &tmp : rml_i){
			int prod_int = f(tmp);
			if (pb.interned(prod_int)) sets.ps.insert(pb.build(prod_int));
			else rml.push_back(prod_int);
		}
		
		if (rml.size() == 0) return;

		SACommand cmd(&con);
		getField("select * from Product where productref = :1 ", [](int i){ return str_add(" or productref = :",i); }, cmd, rml);
		extractProduct(pb, cmd, sets);
	}

	void getAllReviewers(Reviewer::builder &rb, auto &rml_i, sets &sets, auto f = id_f ){
		assert(rml_i.size() > 0);

		std::list<long> rml;
		for (auto &tmp : rml_i){
			int rr_int = f(tmp);
			if (rb.interned(rr_int)) sets.rrs.insert(rb.build(rr_int));
			else rml.push_back(rr_int);
		}
		
		if (rml.size() == 0) return;
		

		SACommand cmd(&con);
		getField("select * from Reviewer where id = :1 ", [](int i) {return str_add(" or id = :", i);}, cmd, rml);
		extractReviewer(rb, cmd, sets);
	}
	

public:

	void getAllReviews(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb, 
					   const auto &reviewList, sets &sets, auto f = id_f ){
		assert(reviewList.size() > 0);
		SACommand cmd(&con);
		getField("select * from Review where (score = :1 and helpyes = :2 and helptotal = :3 and time = :4 and text = :5)", 
				 [](int i) {
					 auto start = i * 5;
					 std::stringstream ss;
					 ss <<" or (score = :"<< (1 + start)
						<<" and helpyes = :"<< (2 + start)
						<<" and helptotal = :"<< (3 + start)
						<<" and time = :"<< (4 + start)
						<<" and text = :"<< (5 + start)
						<< ") ";
					 return ss.str();
				 }, cmd, reviewList, 
				 [&f](auto &cmd, auto &r) {
					 cmd << f(r).score
						 << (long) f(r).help.first
						 << (long) f(r).help.second
						 << (long) f(r).time
						 << f(r).text.get().c_str();
				 }, noop_f<SACommand>, 0, 1);
		extractReview(rb, pb, rrb, cmd, sets);
	}


	void getAllReviews(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb,
					   const Reviewer &r, sets &sets){
		assert(r.reviews.size() > 0);
		SACommand cmd(&con);
		getField("select * from Review where reviewer = :1", [](int i){return str_add(" and not id = :", i);}, cmd, r.reviews, 
				 [](auto &cmd, auto &r){cmd << r.lock()->id.asInt(); }, [&r](auto &cmd){cmd << r.id.asInt(); }, 2);
		extractReview(rb, pb, rrb, cmd, sets);
	}

	void getAllReviews(Review::builder &rb, Product::builder &pb, Reviewer::builder &rrb, 
					   const Product &p, sets &sets) {
		assert(p.reviews.size() > 0);
		SACommand cmd(&con); 
		getField("select * from Review where product = :1", [](int i){return str_add(" and not id = :", i);}, cmd, p.reviews, 
				 [](auto &cmd, auto &r){cmd << r.lock()->id.asInt(); }, [&p](auto &cmd){cmd << p.id.asInt(); }, 2);
		extractReview(rb, pb, rrb, cmd, sets);
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
