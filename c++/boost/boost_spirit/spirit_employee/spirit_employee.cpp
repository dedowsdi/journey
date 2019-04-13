#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace qi = boost::spirit::qi;

struct employee
{
  int age;
  std::string surname;
  std::string forename;
  double salary;
};

BOOST_FUSION_ADAPT_STRUCT(
  employee,
  (int, age)
  (std::string, surname)
  (std::string, forename)
  (double, salary)
)

template <typename It>
struct employee_parser : qi::grammar<It, employee(), qi::ascii::space_type>
{
  employee_parser() : employee_parser::base_type(start)
  {
    //lexeme inhibits space skip
    // %= will set _val of rule to attribute of parser
    quoted_string %= qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];
    start %= 
      qi::lit("employee") >> '{'
      >> qi::int_ >> ','
      >> quoted_string >> ','
      >> quoted_string >> ','
      >> qi::double_
      >> '}'
      ;
  }

  qi::rule<It, std::string(), qi::ascii::space_type> quoted_string;
  qi::rule<It, employee(), qi::ascii::space_type> start;
};


int main(int argc, char *argv[])
{
  std::string s;
  employee_parser<std::string::iterator> parser;
  employee e;
  std::cout << "pls input employee{age, surname, forname, salary}" << std::endl;
  while(std::getline(std::cin, s))
  {
    auto beg = s.begin();
    auto end = s.end();
    // you must use phrase_parse if you want to skip space?
    auto r = qi::phrase_parse(beg, end, parser, qi::ascii::space, e);
    if(r && beg == end)
    {
      std::cout << e.age << std::endl;
      std::cout << e.surname << std::endl;
      std::cout << e.forename << std::endl;
      std::cout << e.salary << std::endl;
    }
    else
      std::cout << "parse failed" << std::endl;
  }
  
  return 0;
}
