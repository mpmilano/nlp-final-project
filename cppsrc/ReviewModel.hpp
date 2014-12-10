#pragma once

#include "FirstVector.hpp"
#include "SecondVector.hpp"

class ReviewModel{

private:

	double val_as_double(const FirstVector &v, int pos) const{
		return v.quotes_word * pos;
	}

	model *trained;

public:

	enum class humor : int{
		funny = 1, normal = 2
	};

	//construct with training set
	ReviewModel(const VecMap1 &vm1_funny, const VecMap1 &vm1_normal){

		struct problem prob;
		struct parameter params;


		prob.n = 25;
		
		int total_size = (1 + vm1_funny.size() + vm1_normal.size());
		prob.l = total_size - 1;
		
		double *y = (double*) alloca(sizeof(double) * total_size);

		struct feature_node **x = (feature_node **) alloca(sizeof(feature_node*) * total_size);
		
		{ uint i = 0;
			for (auto &fp : vm1_funny){
				x[i] = (feature_node *) alloca(sizeof(feature_node) * (prob.n +1) );
				y[i] = (int) humor::funny;
				for (int j = 0; j < prob.n; ++j){
					feature_node n;
					n.index = j;
					n.value = val_as_double(fp.second, j);
					x[i][j] = n;
				}
				feature_node n;
				n.index = -1;
				n.value = -1;
				x[i][prob.n] = n;
				++i;
			}
			for (auto np : vm1_normal){
				x[i] = (feature_node *) alloca(sizeof(feature_node) * (prob.n +1) );
				y[i] = (int) humor::normal;
				for (int j = 0; j < prob.n; ++j){
					feature_node n;
					n.index = j;
					n.value = val_as_double(np.second, j);
					x[i][j] = n;
				}
				feature_node n;
				n.index = -1;
				n.value = -1;
				x[i][prob.n] = n;
				++i;
			}
		}
		
		x[total_size - 1] = nullptr;
		y[total_size - 1] = 0;
	
		prob.y = y;
		prob.x = x;
		prob.bias = 0;

		params.solver_type = L1R_LR;
		
		params.eps =  -0.001;
		params.C = 0;
		params.nr_weight =0;
		params.weight_label = nullptr;
		params.weight = nullptr;
		params.p = 0;
		
		trained = train(&prob, &params);
		
		//model* train (const struct problem *prob, const struct parameter *param); 

		/*
		  struct problem
		  {
		    int l, n; // should be the same, number of features 
			int *y;  // classification (integers which represent categories) 
			struct feature_node **x; //pair of index,value
			double bias; // this is 0
		 */

	}


};
