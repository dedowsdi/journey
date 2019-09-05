#ifndef GL4_GLC_EXCEPTION_H
#define GL4_GLC_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace zxd
{

class except_base : public std::exception
{
public:
  except_base(const std::string& msg = "");

private:
  std::string _msg;
};

class gl_new_resource_failed : public except_base
{
public:
  using except_base::except_base;
};

class gl_empty_source : public except_base
{
public:
  using except_base::except_base;
};


}

#endif /* GL4_GLC_EXCEPTION_H */
