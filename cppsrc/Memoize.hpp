#pragma once
#include <memory>

template<typename T>
class Memo {
public:
  
  typedef std::unique_ptr<Memo<T> > t;
  
  virtual T unpack() const = 0;
};

template<typename T>
class Memoizeable {
public:
    
    typedef std::unique_ptr<Memo<T> > Memo_p;
    virtual Memo_p pack() const = 0;
};