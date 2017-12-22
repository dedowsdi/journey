#include "program.h"
#include "common.h"
#include <algorithm>
#include <functional>

namespace zxd {

//--------------------------------------------------------------------
void Program::setUniformLocation(GLint* location, const std::string& name) {
  ::setUniformLocation(location, object, (char*)name.c_str());
}

//--------------------------------------------------------------------
void Program::attachShaderFile(GLenum type, const std::string& file) {
  ::attachShaderFile(object, type, (char*)file.c_str());
}

//--------------------------------------------------------------------
void Program::attachShaderSource(GLenum type, StringVector& sources) {
  CStringVector csv;
  std::transform(sources.begin(), sources.end(), std::back_inserter(csv),
    std::mem_fn(&std::string::c_str));
  ::attachShaderSource(object, type, csv.size(), (char**)&csv[0]);
}

//--------------------------------------------------------------------
void Program::attachShaderSourceAndFile(
  GLenum type, StringVector& sources, const std::string& file) {
  CStringVector csv;
  std::transform(sources.begin(), sources.end(), std::back_inserter(csv),
    std::mem_fn(&std::string::c_str));
  ::attachShaderSourceAndFile(
    object, type, csv.size(), (char**)&csv[0], (char*)file.c_str());
}
}
