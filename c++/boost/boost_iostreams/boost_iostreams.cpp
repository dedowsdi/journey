/*
 * A Device which only supports writing is called a Sink.
 * A Device which only supports reading is called a Source.
 * A narrow-character Device for read-write access to a single character
 * sequence with a single position indicator is called a SeekableDevice
 */
#include "container_source.h"
#include "container_sink.h"
#include "container_device.h"
#include "shell_comments_filter.hpp"
#include "shell_comments_input_filter.hpp"
#include "shell_comments_output_filter.hpp"
#include <iostream>
#include <string>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>

using namespace std;
namespace io = boost::iostreams;

void test_container_source(){
  typedef std::vector<char> char_vec;
  typedef container_source<string> string_source;
  typedef container_source<std::vector<char>> char_vec_source;

  // read from string
  string input_string = "hello world!";
  string output_string;

  io::stream<string_source> ss_in(input_string);
  getline(ss_in, output_string);

  assert(input_string == output_string);

  //read from char vec
  char_vec input_char_vec(input_string.begin(), input_string.end());
  io::stream<char_vec_source> cv_in(input_char_vec);
  getline(ss_in, output_string);

  assert(input_string == output_string);
}

void test_container_sink(){
  typedef std::vector<char> char_vec;
  typedef container_sink<string> string_sink;
  typedef container_sink<std::vector<char>> char_vec_sink;

  string input_string = "hello world!";
  string output_string;

  // write to string
  io::stream<string_sink> ss_out(output_string);
  ss_out << input_string;
  ss_out.flush();
  assert(input_string == output_string);

  // write to char vec
  char_vec output_char_vec;
  io::stream<char_vec_sink> cv_out(output_char_vec);
  cv_out << input_string;
  cv_out.flush();

  output_string.assign(output_char_vec.begin(), output_char_vec.end());
  assert(input_string == output_string);
}

void test_container_device(){
  typedef container_device<string> string_device;

  string one, two;
  io::stream<string_device> io(one);
  io << "Hello World!";
  io.flush();
  io.seekg(0, BOOST_IOS::beg); // seek to the beginning before read
  getline(io, two);
  assert(one == "Hello World!");
  assert(two == "Hello World!");
}


void test_shell_comments_filter(){

  io::filtering_ostream out;
  out.push(shell_comments_output_filter());
  out.push(std::cout);
  out << "#this is commment, you should not see this" << std::endl;
  out << "this is not comment" << std::endl;

  std::stringstream ss;
  ss << "#this is commment, you should not see this" << std::endl;
  ss << "this is not comment" << std::endl;
  ss.seekg(0);
  io::filtering_istream in;
  in.push(shell_comments_input_filter());
  in.push(ss);
  std::string s;
  while (getline(in, s)){
    std::cout << s << std::endl;
  }

  io::filtering_ostream out2;
  out2.push(shell_comments_stdio_filter());
  out2.push(std::cout);
  out2 << "#this is commment, you should not see this" << std::endl;
  out2 << "this is not comment" << std::endl;

  ss.seekg(0);
  io::filtering_istream in2;
  in2.push(shell_comments_stdio_filter());
  in2.push(ss);
  while (getline(in2, s)){
    std::cout << s << std::endl;
  }

}

int main(int argc, char *argv[])
{
  test_container_source();
  test_container_sink();
  test_container_device();
  test_shell_comments_filter();
  
  return 0;
}
