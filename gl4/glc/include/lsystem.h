#ifndef GL_GLC_LSYSTEM_H
#define GL_GLC_LSYSTEM_H

#include <map>

namespace zxd
{

class lsystem
{
protected:
  std::map<char, std::string> m_rules;

public:

  const std::map<char, std::string>& rules() const;
  void rules(const std::map<char, std::string>& v);

  void add_rule(char c, const std::string& s);

  std::string generate(const std::string& axiom, int iterations);
};

}


#endif /* GL_GLC_LSYSTEM_H */
