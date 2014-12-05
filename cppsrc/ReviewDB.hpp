#pragma once
#include "ReviewParser.hpp"
#include <SQLAPI.h>
#include <future>

template<typename Input>
class ReviewDB{

public:

	typedef typename ReviewParser<Input>::sets sets;

	std::string dbname;
	SAConnection con;

	ReviewDB(std::string dname):dbname(dname){
		con.Connect(SAString(dbname.c_str()),"research","researchVM",SA_MySQL_Client);
	}
	
	template <decltype(std::launch::deferred) launchpol = std::launch::deferred> 
	auto writeToDB(std::shared_ptr<ReviewDB> sp, std::shared_ptr<const sets> s) {
		assert(sp.get() == this);
		auto task1 = [s, this](){ //reviewers
			SACommand cmd(&con);
			cmd.setCommandText("insert into Reviewer (profileName, userID, id) values (:1, :2, :3) ON DUPLICATE KEY UPDATE id=:3");
			for (const auto &rr : s->rrs){
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
		
		auto task2 = [s,this](){ //Products
			
			SACommand cmd(&con);
			cmd.setCommandText("insert into Product (ProductID, Title, Price, ProductType, ProductRef) values (:1, :2, :3, :4, :5) ON DUPLICATE KEY UPDATE ProductRef=:5");
			for (const auto &p : s->ps){
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

		auto task3 = [s, this](){ //reviews
			SACommand cmd(&con);
			cmd.setCommandText("insert into Review (Summary, Score, Time, Reviewer, HelpYes, HelpTotal, Product, Text, Id) values (:1, :2, :3, :4, :5, :6, :7, :8, :9) on duplicate key update HelpYes=:5");
			for (const auto &r : s->rs){
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
		
		return std::async(launchpol,[sp,fut1, fut2, fut3]() -> bool {
				fut1.get();
				fut2.get();
				fut3.get();
				return (sp->isValidMem());
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
