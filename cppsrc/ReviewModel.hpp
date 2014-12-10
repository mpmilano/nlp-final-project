#pragma once

#include "FirstVector.hpp"
#include "SecondVector.hpp"

class ReviewModel{

private:

	double val_as_double(const FirstVector &v, int pos) const{
		return v.quotes_word * pos;
	}

public:

	enum class humor : int{
		funny = 1, normal = 2
	};

	//construct with training set
	ReviewModel(const VecMap1 &vm1_funny, const VecMap1 &vm1_normal){

		struct problem prob;
		prob.n = 25;
		
		int total_size = (1 + vm1_funny.size() + vm1_normal.size());
		prob.l = total_size - 1;
		
		double *y = (double*) malloc(sizeof(double) * total_size);

		struct feature_node **x = (feature_node **) malloc(sizeof(feature_node*) * total_size);
		
		{ uint i = 0;
			for (auto &fp : vm1_funny){
				x[i] = (feature_node *) malloc(sizeof(feature_node) * (prob.n +1) );
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
				x[i] = (feature_node *) malloc(sizeof(feature_node) * (prob.n +1) );
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
		
		
		//model* train (const struct problem *prob, const struct parameter *param); 

		/*
		  struct problem
		  {
		    int l, n; // should be the same, number of features 
			int *y;  // classification (integers which represent categories) 
			struct feature_node **x; //pair of index,value
			double bias; // this is 0
		 */

		free(y);
	}


};
