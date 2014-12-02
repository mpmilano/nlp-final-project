#pragma once
#include "python2.7/Python.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"

class NLTKInstance {

public:
	typedef decltype(PyImport_ImportModule("")) py_type;

private:
	static plain_ptr<NLTKInstance>& current() {static plain_ptr<NLTKInstance> b(nullptr); return b;}

	py_type nltk_module, nltk_dict;

	bool built = false;

	void init(){
		assert(!built);
		Py_Initialize();
		nltk_module = PyImport_ImportModule("nltk");
		nltk_dict = PyModule_GetDict(nltk_module);
		assert (nltk_module != nullptr);
		assert (nltk_dict != nullptr);
		built = true;
	}

public:

	static NLTKInstance& active(){ assert (current() != nullptr); return *(current()); }

	NLTKInstance(){ 
		assert (current() == nullptr);
		current() = this;
		assert (current() != nullptr);
	}

	NLTKInstance(const NLTKInstance&) = delete;

	class Stemmer{
	private:

		bool built = false;

		NLTKInstance &inst;

		//new 
		py_type module, stemmer, stemname;
		//borrowed 
		py_type stemclass;

		void init(){
			assert(!built);
			if (!inst.built) inst.init();
			module = PyImport_ImportModule("nltk.stem.porter");
			stemclass = PyDict_GetItemString(PyModule_GetDict(module),"PorterStemmer");
			assert(module != nullptr);
			assert(PyModule_Check(module));
			assert(stemclass != nullptr);
			assert(PyCallable_Check(stemclass));
			stemmer = PyObject_CallObject(stemclass,nullptr);
			assert(stemmer != nullptr);
			stemname = PyString_FromString("stem");
			built = true;
		}

	public:

		/*
		  stemmer = nltk.stem.porter.PorterStemmer();
		 */
		Stemmer(NLTKInstance &i):inst(i){}

		Stemmer(const Stemmer&) = delete;
		
		std::string stem(const std::string &orig){
			if (!built) init();
			/* return stemmer.stem(orig) */
			auto pstr = PyString_FromString(orig.c_str());
			assert (pstr != nullptr);
			assert (PyString_Check(pstr));
			assert (PyString_Size(pstr) == (int) orig.length());
			auto ret = PyObject_CallMethodObjArgs(stemmer,stemname,pstr,NULL);
			if (ret == nullptr) {
				std::cerr << "In C++ function NLTKInstance::Stemmer::stem with argument " 
					  << orig.c_str()
					  << " (python " << PyString_AsString(pstr) << "):" << std::endl;
				PyErr_Print();
			}
			assert (ret != nullptr);
			assert(PyString_Check(ret));
			std::string rret(PyString_AsString(ret));
			Py_DECREF(ret);
			Py_DECREF(pstr);
			return rret;
		}

		virtual ~Stemmer(){
			if (built) {
				Py_DECREF(stemname);
				Py_DECREF(stemmer);
				Py_DECREF(module);
			}
		}

	};

	friend class Stemmer;

	class Sentence_Tokenizer{
	private:
		bool built = false;
		NLTKInstance& inst;
		//new 
		py_type module, ppath, mname, obj;

		void init(){
			assert(!built);
			if (!inst.built) inst.init();
			module = PyImport_ImportModule("nltk.data");
			ppath = PyString_FromString("tokenizers/punkt/english.pickle");
			mname = PyString_FromString("tokenize");
			obj = PyObject_CallFunctionObjArgs(
				PyDict_GetItemString(PyModule_GetDict(module), "load"), ppath, NULL);
			built = true;
		}
	public:

		/*
		  stemmer = nltk.stem.porter.PorterStemmer();
		 */
		Sentence_Tokenizer(NLTKInstance& i):inst(i){}
		Sentence_Tokenizer(const Sentence_Tokenizer&) = delete;
		
		
		template<typename container, typename string>
		auto tokenize(string s){
			if (!built) init();
			container rret;
			auto ps = PyString_FromString(s.get().c_str());
			auto tret = PyObject_CallMethodObjArgs(obj,mname,ps,NULL);
			auto ret = PySequence_Fast(tret,"don't care");
			assert(ret != nullptr);

			//all below borrowed (or ints)
			auto limit = PySequence_Size(ret);
			auto arr = PySequence_Fast_ITEMS(ret);
			for (auto i = 0 ; i < limit; ++i)
				rret.push_back(string(PyString_AsString(arr[i])));
			Py_DECREF(ret);
			Py_DECREF(tret);
			Py_DECREF(ps);
			return rret;
		}

		virtual ~Sentence_Tokenizer(){
			if (built){
				Py_DECREF(obj);
				Py_DECREF(module);
				Py_DECREF(ppath);
				Py_DECREF(mname);
			}
		}

	};

	friend class Sentence_Tokenizer;

	class Word_Tokenizer{
	private:

		bool built = false;

		NLTKInstance &inst;

		//new 
		py_type module, toker, tokname;
		//borrowed 
		py_type tokclass;

		void init(){
			assert(!built);
			if (!inst.built) inst.init();
			module = PyImport_ImportModule("nltk.tokenize.punkt");
			tokclass = PyDict_GetItemString(PyModule_GetDict(module),"PunktWordTokenizer");
			assert(module != nullptr);
			assert(PyModule_Check(module));
			assert(tokclass != nullptr);
			assert(PyCallable_Check(tokclass));
			toker = PyObject_CallObject(tokclass,nullptr);
			assert(toker != nullptr);
			tokname = PyString_FromString("tokenize");
			built = true;
		}

	public:

		Word_Tokenizer(NLTKInstance &i):inst(i){}

		Word_Tokenizer(const Word_Tokenizer&) = delete;
		
		template<typename container, typename string>
		container tokenize(const string &orig){
			container rret;
			if (!built) init();
			/* return toker.tok(orig) */
			auto pstr = PyString_FromString(orig.c_str());
			assert (pstr != nullptr);
			assert (PyString_Check(pstr));
			assert (PyString_Size(pstr) == (int) orig.length());
			auto tret = PyObject_CallMethodObjArgs(toker,tokname,pstr,NULL);
			if (tret == nullptr) {
				std::cerr << "In C++ function NLTKInstance::Toker::tok with argument " 
					  << orig.c_str()
					  << " (python " << PyString_AsString(pstr) << "):" << std::endl;
				PyErr_Print();
			}
			assert (tret != nullptr);
			auto ret = PySequence_Fast(tret,"don't care");
			assert(ret != nullptr);

			//all below borrowed (or ints)
			auto limit = PySequence_Size(ret);
			auto arr = PySequence_Fast_ITEMS(ret);
			for (auto i = 0 ; i < limit; ++i)
				rret.push_back(string(PyString_AsString(arr[i])));

			Py_DECREF(ret);
			Py_DECREF(tret);
			Py_DECREF(ret);
			Py_DECREF(pstr);
			return rret;
		}

		virtual ~Word_Tokenizer(){
			if (built) {
				Py_DECREF(tokname);
				Py_DECREF(toker);
				Py_DECREF(module);
			}
		}

	};

	friend class Word_Tokenizer;



	virtual ~NLTKInstance(){
		if (built){
			Py_DECREF(nltk_module);
			Py_Finalize();
		}
	}


	

};
