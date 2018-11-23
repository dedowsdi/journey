#include "lsystem.h"

namespace zxd
{

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

  std::string s;
  for(char c : axiom)
  {
    auto iter = m_rules.find(c);
    if(iter == m_rules.end())
      s.push_back(c);
    else
      s += iter->second;
  }

  return generate(s, --iterations);
}
}
