#ifndef GL_GLC_LSYSTEM_H
#define GL_GLC_LSYSTEM_H

#include <map>

namespace zxd
{

class lsystem
{
public:

  using rule_map = std::map<char, std::string>;

  lsystem() = default;
  lsystem(int i){}
  lsystem(const rule_map& rules);

  const std::map<char, std::string>& rules() const;
  void rules(const std::map<char, std::string>& v);

  void add_rule(char c, const std::string& s);

  std::string generate(const std::string& axiom, int iterations);
  std::string step(const std::string& axiom, int count);

  const rule_map& get_rules() const{ return m_rules;}

private:
  rule_map m_rules;
};

}


#endif /* GL_GLC_LSYSTEM_H */
