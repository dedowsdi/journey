// from Ogre
#ifndef SINGLETON_SINGLETON_H
#define SINGLETON_SINGLETON_H
#include <cassert>

template<typename T>
class singleton
{
public:
  T& instance(){
    assert(m_instance);
  }

  singleton(const singleton& t) = delete;
  singleton& operator=(const singleton& t) = delete;

protected:

  singleton()
  {
    assert(!m_instance);
    m_instance = static_cast<T*>(this);
  }

  // static class member, be careful of the initialization and destruction order
  static T* m_instance; // child class must define this
};

#endif /* SINGLETON_SINGLETON_H */
