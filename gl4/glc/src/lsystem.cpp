#include <lsystem.h>
#include <sstream>

namespace zxd
{

//--------------------------------------------------------------------
lsystem::lsystem(const rule_map& rules):
  m_rules(rules)
{
}

//--------------------------------------------------------------------
const std::map<char, std::string>& lsystem::rules() const
{
  return m_rules;
}

//--------------------------------------------------------------------
void lsystem::rules(const std::map<char, std::string>& v)
{
  m_rules = v;
}

//--------------------------------------------------------------------
void lsystem::add_rule(char c, const std::string& s)
{
  m_rules[c] = s;
}

//--------------------------------------------------------------------
std::string lsystem::generate(const std::string& axiom, int iterations)
{
  if(iterations == 0)
    return axiom;

  std::stringstream ss;
  for(char c : axiom)
  {
    auto iter = m_rules.find(c);
    if(iter == m_rules.end())
      ss << c;
    else
      ss << iter->second;
  }
  return generate(ss.str(), --iterations);
}

//--------------------------------------------------------------------
std::string lsystem::step(const std::string& axiom, int count)
{
  if(count == 0 || axiom.empty())
    return axiom;

  std::stringstream ss;
  for (int i = 0; i < axiom.size(); ++i)
  {
    char c = axiom[i];
    auto iter = m_rules.find(c);
    if(iter == m_rules.end() || i >= count)
      ss << c;
    else
      ss << iter->second;
  }
  return ss.str();
}
}
