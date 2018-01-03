#include <fstream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

std::string readFile(const std::string& file) {
  std::ifstream ifs(file);

  if (!ifs) {
    std::stringstream ss;
    ss << "failed to open file " << file << std::endl;
    throw std::runtime_error(ss.str());
  }

  // get size
  ifs.seekg(std::ios_base::end);
  int size = ifs.tellg();
  ifs.seekg(std::ios_base::beg);

  std::string s;
  s.reserve(size);

  //don't use istream_iterator if you just want to read the file character by
  //character
  std::copy(std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(), std::back_inserter(s));

  return s;
}

int main(int argc, char* argv[]) {
  std::ofstream ofs("fstream.txt");
  ofs << "abcdefg" << std::endl
      << "hijklmn" << std::endl
      << "opqrst" << std::endl
      << "uvwxyz" << std::endl;
  ofs.close();

  std::string s = readFile("fstream.txt");
  std::cout << s;
}

