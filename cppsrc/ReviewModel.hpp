#pragma once

#include "FirstVector.hpp"
#include <cmath>
#include "linear.h"
#include "tron.h"
class ReviewModel{

private:

	double val_as_double(const FirstVector &v, int pos) const{
		
		switch(pos){
		case 0: 
			return std::sqrt(v.numchars * 1.0);
		case 1:
			return v.allpunct;
		case 2://books
			return v.category == "Books";
		case 3://clothing
			return v.category == "Clothing";
		case 4://grocery
			return v.category == "Grocery";
		case 5://industrial
			return v.category == "Industrial";
		case 6://movies
			return v.category == "Movies";
		case 7://music
			return v.category == "Music";
		case 8://sports
			return v.category == "Sports";
		case 9://toys
			return v.category == "Toys";

		}
		
		
		return v.quotes_word * pos;
	}

	model *trained;
	SafeMalloc sm;
	struct problem *prob;
	struct parameter *params;


public:

	enum class humor : int{
		funny = 1, normal = 2
	};

	//construct with training set
	ReviewModel(const VecMap1 &vm1_funny, const VecMap1 &vm1_normal)
		:trained(nullptr),sm(),prob(sm.malloc<struct problem>(1)),params(sm.malloc<struct parameter>(1)) {

		prob->n = 10;
		
		int total_size = (1 + vm1_funny.size() + vm1_normal.size());
		prob->l = total_size - 1;
		
		double *y = sm.malloc<double>(total_size);
		
		struct feature_node **x = sm.malloc<feature_node *>(total_size);
		
		{ int i = 0;
			for (auto &fp : vm1_funny){
				x[i] = sm.malloc<feature_node>(prob->n +1);
				y[i] = (int) humor::funny;
				for (int j = 0; j < prob->n; ++j){
					feature_node n;
					n.index = j;
					n.value = val_as_double(fp.second, j);
					x[i][j] = n;
				}
				feature_node n;
				n.index = -1;
				n.value = -1;
				x[i][prob->n] = n;
				++i;
			}
			for (auto np : vm1_normal){
				x[i] = sm.malloc<feature_node>(prob->n +1);
				y[i] = (int) humor::normal;
				for (int j = 0; j < prob->n; ++j){
					feature_node n;
					n.index = j;
					n.value = val_as_double(np.second, j);
					x[i][j] = n;
				}
				feature_node n;
				n.index = -1;
				n.value = -1;
				x[i][prob->n] = n;
				++i;
			}

			if (i != prob->l){
				std::cout << i << std::endl;
				std::cout << prob->l << std::endl;
				assert(i == prob->l);
			}
		}
		
		x[total_size - 1] = nullptr;
		y[total_size - 1] = 0;
	
		prob->y = y;
		prob->x = x;
		prob->bias = 0;

		params->solver_type = L1R_LR;
		
		params->eps =  -0.001;
		params->C = 0;
		params->nr_weight =0;
		params->weight_label = nullptr;
		params->weight = nullptr;
		params->p = 0;

		//testing memory time!

		for (int i = 0; i < prob->l; ++i){
			assert(y[i] + 1);
			assert(x[i]);
			for (int j = 0; j < prob->n; ++j){
				assert(x[i][j].value + 2);
			}
		}
		
		//trained = train(prob, params);
		
		//model* train (const struct prob->lem *prob->, const struct parameter *param); 

		/*
		  struct prob->lem
		  {
		    int l, n; // should be the same, number of features 
			int *y;  // classification (integers which represent categories) 
			struct feature_node **x; //pair of index,value
			double bias; // this is 0
		 */

	}


};
