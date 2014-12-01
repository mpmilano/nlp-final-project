#pragma once
#include "python2.7/Python.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"

class NLTKInstance {

public:
	typedef decltype(PyImport_ImportModule("")) py_type;

private:
	static plain_ptr<NLTKInstance>& current() {static plain_ptr<NLTKInstance> b(nullptr); return b;}

	py_type nltk_module, nltk_dict;

public:

	static NLTKInstance& active(){ assert (current() != nullptr); return *(current()); }

	NLTKInstance(){ 
		assert (current() == nullptr);
		current() = this;
		assert (current() != nullptr);
		Py_Initialize();
		nltk_module = PyImport_ImportModule("nltk");
		nltk_dict = PyModule_GetDict(nltk_module);
		assert (nltk_module != nullptr);
		assert (nltk_dict != nullptr); 
	}

	NLTKInstance(const NLTKInstance&) = delete;

	class Stemmer{
	private:
		//new 
		py_type module, stemmer;
		//borrowed 
		py_type stemclass;
	public:

		/*
		  stemmer = nltk.stem.porter.PorterStemmer();
		 */
		Stemmer(const NLTKInstance&):module(PyImport_ImportModule("nltk.stem.porter")), 
					     stemclass(PyDict_GetItemString(
							       PyModule_GetDict(module),"PorterStemmer")) 
			{
				assert(module != nullptr);
				assert(PyModule_Check(module));
				assert(stemclass != nullptr);
				assert(PyCallable_Check(stemclass));
				stemmer = PyObject_CallObject(stemclass,nullptr);
				assert(stemmer != nullptr);
			}

		Stemmer(const Stemmer&) = delete;
		
		std::string stem(const std::string &orig){
			/* return stemmer.stem(orig) */
			auto pstr = PyString_FromString(orig.c_str());
			assert (pstr != nullptr);
			auto ret = PyObject_CallMethod(stemmer,"stem","(s)",pstr);
			if (ret == nullptr) {
				std::cerr << "In C++ function NLTKInstance::Stemmer::stem with argument " 
					  << orig.c_str()
					  << " (python " << PyString_AsString(pstr) << "):" << std::endl;
				PyErr_Print();
			}
			assert (ret != nullptr);
			return PyString_AsString(ret);
		}

		virtual ~Stemmer(){
			Py_DECREF(stemmer);
			Py_DECREF(module);
		}

	};


	virtual ~NLTKInstance(){
		Py_DECREF(nltk_module);
		Py_Finalize();
	}


	

};
