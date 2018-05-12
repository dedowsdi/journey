/*
 * program_options:
 *   define a bunch of options_description to describe all the option:
 *      generic   // cmd line only
 *      config   // both on cmd line and config file
 *      hidden   // both on cmd line and config file, but invisible to user
 *   define a positional option to describe opsitional option
 *   combine options_description to
 *     cmdline_options
 *     config_file_options
 *   use command_line_parser to parse cmdline_options to get parsed options from cmd line
 *   use po::parse_config_file to get parsed options from config file
 *   define a variables_map to store values of options
 *   store parsed options to variables_map
 *   notiify variables_map every time you store some options in it
 *
 * if you call store multiple times to the same variable_map, the first non
 * default option will be the final option, what ever comes next will be
 * neglected.
 */
#include <iostream>
#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include <istream>

using namespace std;
namespace po = boost::program_options;

template <typename T>
std::ostream& operator<<(std::ostream& os , const std::vector<T> values){
    std::for_each(values.begin(), values.end(),[&](decltype(*values.begin()) v) {
        std::cout << v << " ";
    });
  return os;
}

class A{
public:
  std::string s;
};

// you must at least provide >> operator for custom type if you don't providate
// validate
istream& operator>>(std::istream& is, A& a){
  is >> a.s;
  return is;
}

class B{
public:
  int i;
};

// By default, the conversion of option's value from string into C++ type is
// done using iostreams, which sometimes is not convenient. The library allows
// the user to customize the conversion for specific classes. In order to do so,
// the user should provide suitable overload of the validate function.
void validate(boost::any& v, const std::vector<std::string>& values, B*, int){

  // make sure no previous assignment exists
  po::validators::check_first_occurrence(v);

  if (v.empty()) {
    v = boost::any(B());
  }
  B* b = boost::any_cast<B>(&v);

  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const string& s = po::validators::get_single_string(values);
  try {
    b->i = stoi(s);
  }catch(exception& e) {
    throw po::validation_error(po::validation_error::invalid_option_value);
  }
}


struct options{
  int int_option;
  float float_option;
  string string_option;
  bool bool_option;
  vector<int> int_vec_option;
  vector<string> string_vec_option;
  string config_file;

  void print(){
    cout << "config_file : "       << config_file       << std::endl;
    cout << "int_option : "        << int_option        << std::endl;
    cout << "float_option : "      << float_option      << std::endl;
    cout << "string_option : "     << string_option     << std::endl;
    cout << "bool_option : "       << bool_option       << std::endl;
    cout << "int_vec_option : "    << int_vec_option    << std::endl;
    cout << "string_vec_option : " << string_vec_option << std::endl;
  }
};

int main(int argc, char *argv[])
{
  options _options;

  // variables_map store values of options
  po::variables_map vm;
  //options_description contans all allowed option_description
  po::options_description generic("generic options");

  // cmd line only
  generic.add_options()
    ("help", "produce help message")
    ("version", "print version string")
    ("config,c", po::value<string>(&_options.config_file), "config file")
    ;

  // both cmd and config file
  po::options_description config("config options");
  config.add_options()
    ("int,i", po::value<int>(&_options.int_option)->default_value(5), "int option")
    ("string,s", po::value<string>(&_options.string_option)->implicit_value("implicit")->default_value("default"), "string option")
    ("bool", po::bool_switch(&_options.bool_option), "bool option")
    // multitoken : -v 1 2 3 4 5
    ("int_vec,v", po::value<vector<int>>(&_options.int_vec_option)->multitoken()->composing(), "int vec option, support multitoken")
    ("string_vec,b", po::value<vector<string>>(&_options.string_vec_option), "string vec option")
    // you don't have to store option to local variable, although you wont's see
    // them in help if you don't specify one.
    ("data", po::value<string>()->default_value("data"), "data")
    // texual value of default_value is only used to display help info ?
    ("A", po::value<A>()->default_value(A(), "custom A with custom << operator"), "A")
    ("B", po::value<B>()->default_value(B(), "custom B with custom validator function"), "B")
    ;

  // hidden
  po::options_description hidden("hidden optoins");
  hidden.add_options()
    ("float,f", po::value<float>(&_options.float_option), "float option")
    ;

  // add positional options
  po::positional_options_description pod;
  pod.add("int", 1);

  po::options_description cmdline_options("command line options");
  cmdline_options.add(generic).add(config).add(hidden);

  po::options_description file_options("config file options");
  file_options.add(config).add(hidden);

  po::options_description visible_options("visible options");
  visible_options.add(generic).add(config);

  // parse_command_line is just a convenient wrapper for simple case
  //po::store(po::parse_command_line(argc, argv, desc), vm);
  
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run();
  po::store(parsed, vm);
  notify(vm);

  _options.print();

  if (vm.count("help")) {
    std::cout << visible_options << std::endl;
    return 0;
  }

  if (vm.count("version")) {
    std::cout << "v1.0.0" << std::endl;
    return 0;
  }

  if (vm.count("data")) {
    std::cout << "data : " << vm["data"].as<string>() << std::endl;
  }

  if (vm.count("A")) {
    std::cout << "A : " << vm["A"].as<A>().s << std::endl;
  }

  if (vm.count("B")) {
    std::cout << "B : " << vm["B"].as<B>().i << std::endl;
  }

  if (vm.count("config")) {
    ifstream ifs(_options.config_file);
    std::cout << file_options ;
    std::cout << "-------------" << std::endl; ;
    if (!ifs) {
      std::cout << "failed to open config file " << _options.config_file << std::endl;
      return 0;
    }
    else{
    // If 'vm' already has a non-defaulted value of an option, that value is not
    // changed, even if 'file_options' specify some value.        
      po::store(po::parse_config_file(ifs, file_options, true), vm);
      po::notify(vm);
    }
  }

  _options.print();

  return 0;
}
