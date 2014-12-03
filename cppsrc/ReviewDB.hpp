#pragma once
#include "ReviewParser.hpp"
#include <SQLAPI.h>
#include <future>

class ReviewDB{

public:

	typedef ReviewParser<std::ifstream>::sets sets;

	auto writeToDB(const sets &s){
		auto task1 = [&s](){ //reviewers 
			SAConnection con;
			con.Connect("AmazonReviews","research","researchVM",SA_MySQL_Client);
			
			SACommand cmd(&con);
			cmd.setCommandText("insert into Reviewer (profileName, userID, id) values (:1, :2, :3)");
			for (const auto &rr : s.rrs){
				cmd << lock(rr)->profileName.c_str();
				cmd << lock(rr)->userID.c_str();
				cmd << lock(rr)->id.asInt();
				try{
					cmd.Execute();
				}
				catch (SAException e){
					std::string errtxt = e.ErrText().GetMultiByteChars();
					if (errtxt.substr(0,15) != "Duplicate entry"){
						sleep(5);
						std::cerr << "Previously-unseen error!" << std::endl;
						std::cerr << errtxt << std::endl;
						sleep(5);
						throw e;
					}
				}
			}
		};

		auto task2 = [&s](){ //Products
			SAConnection con;
			con.Connect("AmazonReviews","research","researchVM",SA_MySQL_Client);
			
			SACommand cmd(&con);
			cmd.setCommandText("insert into Product (ProductID, Title, Price, ProductType, ProductRef) values (:1, :2, :3, :4, :5)");
			for (const auto &p : s.ps){
				cmd << lock(p)->productID.c_str() << lock(p)->title.c_str() 
					<< lock(p)->price << lock(p)->productType.c_str() << lock(p)->id.asInt();
				try{
					cmd.Execute();
				}
				catch (SAException e){
					std::string errtxt = e.ErrText().GetMultiByteChars();
					if (errtxt.substr(0,15) != "Duplicate entry"){
						sleep(5);
						std::cerr << "Previously-unseen error!" << std::endl;
						std::cerr << errtxt << std::endl;
						sleep(5);
						throw e;
					}
				}
			}
		};

		auto task3 = [&s](){ //reviews
			SAConnection con;
			con.Connect("AmazonReviews","research","researchVM",SA_MySQL_Client);
			SACommand cmd(&con);
			cmd.setCommandText("insert into Review (Summary, Score, Time, Reviewer, HelpYes, HelpTotal, Product, Text, Id) values (:1, :2, :3, :4, :5, :6, :7, :8, :9)");
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
					if (errtxt.substr(0,15) != "Duplicate entry"){
						sleep(5);
						std::cerr << "Previously-unseen error!" << std::endl;
						std::cerr << errtxt << std::endl;
						sleep(5);
						throw e;
					}
				}
			}
		};

		auto fut1 = std::async(std::launch::async, task1).share();
		auto fut2 = std::async(std::launch::async, task2).share();
		auto fut3 = std::async(std::launch::async, task3).share();
		
		return [fut1, fut2, fut3](){
			fut1.wait();
			fut2.wait();
			fut3.wait();
		};
		
	}

	void readFromDB(Reviewer::builder &, Product::builder &, Review::builder &, sets &){
		
	}


};
