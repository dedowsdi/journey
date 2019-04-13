#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix.hpp>
#include <fstream>
#include <algorithm>
#include <iterator>

//#pragma GCC diagnostic ignored "-Woverloaded-shift-op-parentheses" // gcc has
// no this kind of option
#ifdef _COMPILER_CLANG
// ycm failed to detech this push?
//#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#endif

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;
bool use_auto = true;

struct mini_xml;

// a node can be mini_xml or text
using mini_xml_node = boost::variant<boost::recursive_wrapper<mini_xml>, std::string>;
struct mini_xml
{
  std::string name; // tag name
  std::vector<mini_xml_node> children;
};

// We need to tell fusion about our mini_xml struct
// to make it a first-class fusion citizen
//[tutorial_xml1_adapt_structures
BOOST_FUSION_ADAPT_STRUCT(
    mini_xml,
    (std::string, name)
    (std::vector<mini_xml_node>, children)
)
//]

///////////////////////////////////////////////////////////////////////////
//  Print out the mini xml tree
///////////////////////////////////////////////////////////////////////////
int const tabsize = 4;

void tab(int indent)
{
  for (int i = 0; i < indent; ++i)
    std::cout << ' ';
}

struct mini_xml_printer
{
  mini_xml_printer(int indent = 0)
  : indent(indent)
  {
  }

  void operator()(mini_xml const& xml) const;

  int indent;
};

struct mini_xml_node_printer : boost::static_visitor<>
{
  mini_xml_node_printer(int indent = 0)
  : indent(indent)
  {
  }

  void operator()(mini_xml const& xml) const
  {
    mini_xml_printer(indent+tabsize)(xml);
  }

  void operator()(std::string const& text) const
  {
    tab(indent+tabsize);
    std::cout << "text: \"" << text << '"' << std::endl;
  }

  int indent;
};

void mini_xml_printer::operator()(mini_xml const& xml) const
{
  tab(indent);
  std::cout << "tag: " << xml.name << std::endl;
  tab(indent);
  std::cout << '{' << std::endl;

  BOOST_FOREACH(mini_xml_node const& node, xml.children)
  {
    boost::apply_visitor(mini_xml_node_printer(indent), node);
  }

  tab(indent);
  std::cout << '}' << std::endl;
}

template <typename It>
struct mini_xml_grammar : qi::grammar<It, mini_xml(), qi::ascii::space_type>
{

  mini_xml_grammar() : mini_xml_grammar::base_type(xml)
  {
    text = qi::lexeme[+(qi::char_ - '<') [qi::_val += qi::_1] ];
    node = (xml | text ) [qi::_val = qi::_1];
    start_tag = '<'
              >> !qi::lit('/')
              >> qi::lexeme[+(qi::char_ - '>') [qi::_val += qi::_1] ]
              >> '>';
    end_tag = "</"
            >> qi::string(qi::_r1) // qi::_r1 is a phoenix place holder for the first inherited attribute
            >> '>';
    xml = start_tag  [ph::at_c<0>(qi::_val) = qi::_1]
        >> *node     [ph::push_back(ph::at_c<1>(qi::_val), qi::_1)]
        >> end_tag(ph::at_c<0>(qi::_val)); // send attribute as argument to void(std::string)

  }

  qi::rule<It, mini_xml(), qi::ascii::space_type> xml;
  qi::rule<It, mini_xml_node(), qi::ascii::space_type> node;
  qi::rule<It, std::string(), qi::ascii::space_type> text;
  qi::rule<It, std::string(), qi::ascii::space_type> start_tag;
  // accept an inherited string attribute(parameter), return void for attribute
  qi::rule<It, void(std::string), qi::ascii::space_type> end_tag;
};

// auto rule, with local variable, error info
template <typename It>
struct mini_xml_grammar_auto_rule : qi::grammar<It, mini_xml(), qi::locals<std::string>, qi::ascii::space_type>
{

  mini_xml_grammar_auto_rule() : mini_xml_grammar_auto_rule::base_type(xml, "auto_rule_xml")
  {
    text %= qi::lexeme[+(qi::char_ - '<') ];
    node %= (xml | text );

    start_tag %= '<'
              >> !qi::lit('/')
              > qi::lexeme[+(qi::char_ - '>') ] // > is an exception operator, no backtracking can occur
              > '>';

    end_tag = "</"
            > qi::string(qi::_r1) // qi::_r1 is a phoenix place holder for the first inherited attribute
            > '>';

    // _a is the first local variable
    xml %= start_tag  [qi::_a = qi::_1]
        > *node
        > end_tag(qi::_a); // send attribute as argument to void(std::string)

    xml.name("xml");
    node.name("node");
    text.name("text");
    start_tag.name("start_tag");
    end_tag.name("end_tag");

    // print error message
    qi::on_error<qi::fail>
    (
      xml,
      std::cout << ph::val("Error! Excepting ")
                << qi::_4 // what failed?
                << ph::val("here : \"")
                << ph::construct<std::string>(qi::_3, qi::_2) // iterators from error-pos to end
                << ph::val("\"")
                << std::endl
    );

    // most of time, you don't need this
    qi::on_success
    (
      xml,
      std::cout << ph::val("Match successed") << std::endl
    );
  }

  qi::rule<It, mini_xml(), qi::locals<std::string>, qi::ascii::space_type> xml;
  qi::rule<It, mini_xml_node(), qi::ascii::space_type> node;
  qi::rule<It, std::string(), qi::ascii::space_type> text;
  qi::rule<It, std::string(), qi::ascii::space_type> start_tag;
  // accept an inherited string attribute(parameter), return void for attribute
  qi::rule<It, void(std::string), qi::ascii::space_type> end_tag;
};


int main(int argc, char *argv[])
{
  std::string filepath;
  std::cout << "input file path pls" << std::endl;
  mini_xml_grammar<std::string::iterator> grammar;
  mini_xml_grammar_auto_rule<std::string::iterator> auto_grammar;
  // TODO why does auto fail here ????
  //auto g = mini_xml_grammar<std::string::iterator>() ;

  while(std::getline(std::cin, filepath))
  {
    std::ifstream ifs(filepath);
    if(!ifs)
    {
      std::cout << filepath << " not found " << std::endl;
      continue;
    }

    std::string content;
    std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(),
        std::back_inserter(content));

    auto beg = content.begin();
    auto end = content.end();
    mini_xml ast;
    bool r;
    if(!use_auto)
      r = qi::phrase_parse(beg, end, grammar, qi::ascii::space, ast);
    else
      r = qi::phrase_parse(beg, end, auto_grammar, qi::ascii::space, ast);
    if(r && beg == end)
    {
      mini_xml_printer()(ast);
    }
    else
    {
      std::cout << "parse failed" << std::endl;
    }

  }

  return 0;
}

//#ifdef _COMPILER_CLANG
//#pragma clang diagnostic pop
//#endif
