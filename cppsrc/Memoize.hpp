#pragma once
#include <memory>
#include <boost/serialization/access.hpp>

template<typename T>
class Memo {
public:
  
  typedef std::unique_ptr<Memo<T> > t;
  
  virtual T unpack() const = 0;

	friend class boost::serialization::access;
private:
	template<class Archive>
	void serialize(Archive &, const unsigned int ){}

};

template<typename T>
class Memoizeable {
public:
    
    typedef std::unique_ptr<Memo<T> > Memo_p;
    virtual Memo_p pack() const = 0;
};
