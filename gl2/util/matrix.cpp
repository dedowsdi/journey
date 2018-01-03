#include "glm.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace glm;

vec4 readVec4(std::istream& is) {
  glm::vec4 v;
  GLfloat* p = &v[0];

  int i = 0;
  while (i++ != 4) {
    if (!is) std::runtime_error("not enough data to populate vec4");
    is >> *p++;
  }
  return v;
}

mat4 readMatrix(std::istream& is) {
  glm::mat4 m;
  GLfloat* p = &m[0][0];

  int i = 0;
  while (i++ != 16) {
    if (!is) std::runtime_error("not enough data to populate matrix");
    is >> *p++;
  }
  return m;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "wront cmd, it shoud be " << argv[0] << " file" << std::endl;
  }

  std::ifstream ifs(argv[1]);
  if (!ifs) {
    std::cout << "faield to open " << argv[1] << std::endl;
    return 0;
  }

  std::string s;
  std::copy(std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(), std::back_inserter(s));
  // replace "[],"
  std::replace_if(s.begin(), s.end(),
    [&](decltype(*s.begin()) v) -> bool {
      static std::string garbage = "[],";
      return garbage.find(v) != -1;
    },
    ' ');

  std::stringstream ss(s);

  mat4 m = readMatrix(ss);
  vec4 v = readVec4(ss);

  using namespace zxd;
  std::cout << m * v << std::endl;

  return 0;
}
