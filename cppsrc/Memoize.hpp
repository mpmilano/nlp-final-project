#pragma once
#include <memory>
#include <iostream>
#include <set>

template<typename T>
class Memo {
public:
	
	virtual T unpack() const = 0;
	
	virtual bool from_archive() const = 0;
	virtual bool from_pack() const = 0;	

private:
	template<class Archive>
	void serialize(Archive &, const unsigned int ){}
	

};

template<typename T>
class Memoizeable {
public:
	
	typedef std::unique_ptr<Memo<T> > Memo_p;
	virtual Memo_p pack() const = 0;

	virtual ~Memoizeable(){}

};
