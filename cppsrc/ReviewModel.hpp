#pragma once

#include "FirstVector.hpp"
#include <cmath>
#include "linear.h"
#include "tron.h"

enum class humor : int{
	funny = 1, normal = -1
		};

struct classification{
	humor h;
	double confidence;
};


template<int num_features>
class ReviewModel{

private:

	std::function<double (const FirstVector &v, int pos)> val_as_double;

	model *trained;
	SafeMalloc sm;
	struct problem *prob;
	struct parameter *params;

	void convert(const FirstVector &v, feature_node x[num_features + 1]) const {
		for (int j = 0; j < num_features; ++j){
			feature_node n;
			n.index = j + 1;
			n.value = val_as_double(v, j);
			x[j] = n;
		}
		feature_node n;
		n.index = -1;
		n.value = -1;
		x[num_features] = n;
	}

	void build_problem(SafeMalloc &sm, problem &prob, const VecMap1 &vm1_funny, const VecMap1 &vm1_normal) const {
		prob.n = num_features;
		
		int total_size = (1 + vm1_funny.size() + vm1_normal.size());
		prob.l = total_size - 1;
		
		double *y = sm.malloc<double>(total_size);
		
		struct feature_node **x = sm.malloc<feature_node *>(total_size);
		struct feature_node *x_elems = sm.malloc<feature_node>((prob.n * prob.l) + prob.l);

		{ int i = 0;
			for (auto &fp : vm1_funny){
				x[i] = &x_elems[i * (prob.n +1)];
				y[i] = (int) humor::funny;
				convert(fp.second, x[i]);
				++i;
			}
			for (auto np : vm1_normal){
				x[i] = &x_elems[i * (prob.n +1)];
				y[i] = (int) humor::normal;
				convert(np.second, x[i]);
				++i;
			}
		}
		
		x[total_size - 1] = nullptr;
		y[total_size - 1] = 0;
	
		prob.y = y;
		prob.x = x;
		prob.bias = 0;
	}

public:

	std::string print_test(std::pair<double, double> test_resuls) {
		std::stringstream ss;
		ss << "funnyrate: " << test_resuls.first * 100 << "%; normalrate: " << test_resuls.second * 100 << "%" << std::endl;
		return ss.str();
	}

	std::pair<double, double> test(const VecMap1 &vm1_funny, const VecMap1 &vm1_normal) const{
		int funny_rate = 0;
		for (auto &vp : vm1_funny) if (predict(vp.second).h == humor::funny) ++funny_rate;
		int normal_rate = 0;
		for (auto &vp : vm1_normal) if (predict(vp.second).h == humor::normal) ++normal_rate;
		return std::make_pair( ((double) funny_rate / vm1_funny.size()), ((double) normal_rate / vm1_normal.size()) );
		
	}


	const problem& getProblem() const { return *prob;}

	classification predict(const FirstVector &v) const {
		feature_node x[num_features + 1];
		convert(v, x);
		classification c;
		c.confidence = ::predict(trained, x);
		c.h = (c.confidence > 0 ? humor::funny : humor::normal);
		//c.confidence = (c.confidence > 0 ? c.confidence : c.confidence * -1.0);
		return c;
	}


	//construct with training set
	ReviewModel(decltype(val_as_double) vad, const VecMap1 &vm1_funny, const VecMap1 &vm1_normal)
		:val_as_double(vad), trained(nullptr),sm(),prob(sm.malloc<struct problem>(1)),params(sm.malloc<struct parameter>(1)) {

		build_problem(sm, *prob, vm1_funny, vm1_normal);

		params->solver_type = L2R_L2LOSS_SVC_DUAL;
		
		params->eps = 0.001;
		params->C = 1;
		params->nr_weight =0;
		params->weight_label = nullptr;
		params->weight = nullptr;
		params->p = 0.1;

		const char* pc = check_parameter(prob, params);
		if (pc){
			std::cerr << pc << std::endl;
			assert(!check_parameter(prob, params));
		}
	
		trained = train(prob, params);
	}


};

std::ostream& operator<<(std::ostream& os, const classification& s){
	std::string h;
	switch(s.h){
	case humor::funny: 
		h = "funny";
		break;
	case humor::normal:
		h = "normal";
		break;
	}
	return (os << h << " (with " << (s.confidence * 100) << "% confidence)");
}

std::ostream& operator<<(std::ostream& os, const problem &p){
	for (int i = 0; i < p.l; ++i){
		os << p.y[i];
		os << " ";
		for (int j = 0; p.x[i][j].index != -1 && j < p.n; ++j)
			os << p.x[i][j].index << ":" << p.x[i][j].value << " ";
		os << std::endl;
	}
	return os;
}
