/*
 * boost filesystem path support both posix(unix variants, linux, Max OS X) generic and native format.
 */
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = boost::filesystem;
using namespace std;

const char * say_what(bool b) { return b ? "true" : "false"; }

void print_dir_path(const fs::path& dir_path){
  cout << dir_path << " is a directory containing:" << endl;

  vector<string> filenames;

  for(fs::directory_entry& e : fs::directory_iterator(dir_path)){
    // extract tail string
    filenames.push_back(e.path().filename().string());
  }

  sort(filenames.begin(), filenames.end());

  for(const string& s : filenames)
  {
    std::cout << "    " << s << std::endl;
  }
}

void compose_path(){
  fs::path path0("/a");
  fs::path path1("/b");
  fs::path p = path0 / path1; // or p - path0; p /= path1
  std::cout << p << std::endl;
  // make_prefered will convert slash to back slash on windows
  std::cout << p.make_preferred() << std::endl;
}

void decompose_path(const fs::path& p){
  cout  <<  "\ndecomposition:\n";
  cout  <<  "  root_name()----------: " << p.root_name() << '\n';
  cout  <<  "  root_directory()-----: " << p.root_directory() << '\n';
  cout  <<  "  root_path()----------: " << p.root_path() << '\n';
  cout  <<  "  relative_path()------: " << p.relative_path() << '\n';
  cout  <<  "  parent_path()--------: " << p.parent_path() << '\n';
  cout  <<  "  filename()-----------: " << p.filename() << '\n';
  cout  <<  "  stem()---------------: " << p.stem() << '\n';
  cout  <<  "  extension()----------: " << p.extension() << '\n';
}

void query_path(const fs::path& p){
  cout  <<  "\nquery:\n";
  cout  <<  "  empty()--------------: " << say_what(p.empty()) << '\n';
  cout  <<  "  is_absolute()--------: " << say_what(p.is_absolute()) << '\n';
  cout  <<  "  has_root_name()------: " << say_what(p.has_root_name()) << '\n';
  cout  <<  "  has_root_directory()-: " << say_what(p.has_root_directory()) << '\n';
  cout  <<  "  has_root_path()------: " << say_what(p.has_root_path()) << '\n';
  cout  <<  "  has_relative_path()--: " << say_what(p.has_relative_path()) << '\n';
  cout  <<  "  has_parent_path()----: " << say_what(p.has_parent_path()) << '\n';
  cout  <<  "  has_filename()-------: " << say_what(p.has_filename()) << '\n';
  cout  <<  "  has_stem()-----------: " << say_what(p.has_stem()) << '\n';
  cout  <<  "  has_extension()------: " << say_what(p.has_extension()) << '\n';
}

void observer_path(const fs::path& p){
 cout  <<  "\nobservers, native format:" << endl;
# ifdef BOOST_POSIX_API
  cout  <<  "  native()-------------: " << p.native() << endl;
  cout  <<  "  c_str()--------------: " << p.c_str() << endl;
# else  // BOOST_WINDOWS_API
  wcout << L"  native()-------------: " << p.native() << endl;
  wcout << L"  c_str()--------------: " << p.c_str() << endl;
# endif
  cout  <<  "  string()-------------: " << p.string() << endl;
  wcout << L"  wstring()------------: " << p.wstring() << endl;
}

void print_path_type(const fs::path& path0){
  try{

    if (fs::exists(path0)) {
      if (fs::is_regular_file(path0)) {
        // file_size works only on regular file
        cout << path0 << " is a regular file of size " << fs::file_size(path0) << endl;
      }else if(fs::is_directory(path0)){
        print_dir_path(path0);
      }else{
        cout << path0 << "exists, but it's neither regular file nor directory" << endl;
      }
    }else{
      cout << path0 << " doesn't exist" << endl;
    }
  }

  // some file or dir is not readable
  catch(const fs::filesystem_error& e){
    cout << e.what() << endl;
  }
}

void print_smile(){
  // Class path takes care of whatever character type or encoding conversions
  // are required by the particular operating system. 
  { fs::ofstream f("smile"); }
  // out put file stream with wide character name
  { fs::ofstream f(L"smile\u263A"); }
}

int main(int argc, char *argv[])
{

  fs::path data_path("data");
  print_path_type(data_path);
  print_path_type("data/config_options");
  print_path_type("data/not_exist");
  // forbidden_dir/forbidden must be created manually, cmake won't be able to
  // copy them as they are nor readable
  print_path_type("data/forbidden_dir/forbidden");

  print_smile();

  compose_path();

  decompose_path("/a/b/c/d.txt");
  query_path("/a/b/c/d.txt");
  observer_path("/a/b/c/d.txt");


  return 0;
}
