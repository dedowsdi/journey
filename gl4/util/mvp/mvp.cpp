/*
 * a small tool used to make sure model view projection matrix is right
 * read setting from ${MVP_MATRIX_FILE} or -f
 *
 * file format:
 *
 * # this is comment
 * #
 * #
 *
 * # name of  space, following lines wil be
 * model   
 * matrix line 
 * ...
 *
 * # print matrix or vector
 * print 
 *
 * # print matrix, index starts from 0,  count can be -1
 * m start count
 * ...
 * # print vector, start count has the same meaning as m
 * v start count vec4
 * ...
 *
 * vector space_ount vec4  // get result of selected matrix * this vector
 *
 * matrix line format(intrinsic):
 *    matrix identity
 *    matrix scale vec3
 *    matrix rotate angle vec3
 *    matrix translate vec3
 *    matrix ortho left right bottom top near far
 *    matrix perspective flovy aspect_ratio near far
 *    matrix lookat vec3 vec3 vec3
 *    matrix plain 16 float
 */

#include "stream_util.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "string_util.h"
#include "glm.h"
#include <algorithm>
#include <iomanip>

using namespace glm;

std::string file_path;
std::string file_content;
zxd::mat4_vector mats;
std::vector<std::string> space_names;

enum ELEMENT
{
  INITIAL,
  SPACE,   // search for next space
  MATRIX,  // accumulate matrix
  PRINT,
  PRINT_MATRIX,
  PRINT_VECTOR
};

mat4 get_transform(GLuint start, GLuint count)
{
  if(mats.empty())
    return mat4(1);

  if(start+count > mats.size())
      throw std::runtime_error("start+count overflow");

  mat4 m;
  for(auto iter = mats.begin() + start; iter != mats.end() && count > 0; ++iter, --count){
    m = m * *iter ;
  }

  return m;
}

std::string get_mat_name(GLuint start, GLuint count)
{
  std::stringstream ss;
  while(count --)
  {
    ss << space_names[start++]<<"*";
  }
  std::string s = ss.str();
  return s.substr(0, s.size() - 1);
}

struct print_matrix
{
  GLuint start;
  GLuint count;

  void print()
  {
    std::cout << "--------------------------------------------------" << std::endl;;
    mat4 m = get_transform(start, count);
    std::cout << get_mat_name(start, count) << std::endl << std::endl;

    std::string::size_type w = 1;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; j++) {
        w = glm::max(w, string_util::to(m[i][j]).length());
      }
    }
    
    w += 1;

    mat4 identity(1);
    // print column major row by row
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; j++) {
        if(m[j][i] != identity[j][i])
          std::cout << string_util::bash_inverse << std::setw(w) << m[j][i] << string_util::bash_reset;
        else
          std::cout << std::setw(w) << m[j][i];
      }
      std::cout << std::endl;
    }
  }
};

struct print_vector
{
  GLuint start;
  GLuint count;
  vec4 v;
  void print()
  {
    std::cout << "--------------------------------------------------" << std::endl;;
    mat4 m = get_transform(start, count);
    vec4 v1 = m * v;

    // add cull warning corlor
    bool warning = false;
    if(v[3] != 0)
    {
      for (int i = 0; i < 3; ++i) {
        if(glm::abs(v[i] / v[3]) > 1)
        {
          warning = true;
          break;
        }
      }
    }

    std::cout << get_mat_name(start, count) << ""
      << "*(" << v[0] << "  "<< v[1] << "  "<< v[2] << "  "<< v[3] << ") = (";
    for (int i = 0; i < 4; ++i) {
      bool warning = v[3] != 0 && i != 3 && glm::abs(v1[i] / v1[3]) > 1;
      if(warning)
        std::cout << string_util::bash_warning;
      std::cout << v1[i] << "  ";
      if(warning)
        std::cout << string_util::bash_reset;
    }
    std::cout << ")" << std::endl;

  }
};

typedef std::vector<print_matrix> print_matrix_vector;
typedef std::vector<print_vector> print_vector_vector;

print_matrix_vector pms;
print_vector_vector pvs;

std::string read_word(std::istream& is)
{
  std::string word;
  is >> word;
  return word;
}

void assert_true(bool b, const std::string& msg)
{
  if(!b)
    throw std::runtime_error(msg);
}
void assert_element(ELEMENT s0, ELEMENT s1, const std::string& msg)
{
  assert_true(s0 == s1, msg);
}

void assert_element_false(ELEMENT s0, ELEMENT s1, const std::string& msg)
{
  assert_true(s0 != s1, msg);
}

  // replace pi with actual pi, pi2 with half pi ......
void replace_contents()
{
  int dnms[] = {16, 8, 4, 2};

  for (int i = 0; i < 4; ++i) 
  {
    std::stringstream ss;
    ss << "pi" << dnms[i];
    std::string pattern = ss.str();

    ss.str("");
    ss.clear();
    ss << glm::pi<GLfloat>() * dnms[i];

    file_content = string_util::replace_string(file_content, "pi" + string_util::to(dnms[i]), string_util::to(glm::pi<GLfloat>() / dnms[i]));
  }
  file_content = string_util::replace_string(file_content, "pi", string_util::to(glm::pi<GLfloat>()));
}

bool is_comment(const std::string& trimed_string)
{
  return !trimed_string.empty() && trimed_string.front() == '#';
}


mat4 read_mat4(std::istream& is)
{
  std::string word;
  is >> word;

  //    matrix identity
  if(word == "identity")
  {
    return glm::mat4(1);
  }

  //    matrix scale vec3
  if(word == "scale")
  {
    vec3 v = stream_util::read_vec3(is);
    std::cout << v[0] << " " << v[1] << " " << v[2] << " " << std::endl;
    return glm::scale(v);
  }

  //    matrix rotate angle vec3
  if(word == "rotate")
  {
    vec4 v = stream_util::read_vec4(is);
    return glm::rotate(v.x, v.yzw());
  }

  //    matrix translate vec3
  if(word == "translate")
  {
    vec3 v = stream_util::read_vec3(is);
    return glm::translate(v);
  }

  //    matrix ortho left right bottom top near far
  if(word == "ortho")
  {
    vec3 v0 = stream_util::read_vec3(is);
    vec3 v1 = stream_util::read_vec3(is);
    return glm::ortho(v0.x, v0.y, v0.z, v1.z, v1.y, v1.z);
  }

  //    matrix perspective flovy aspect_ratio near far
  if(word == "perspective")
  {
    vec4 v = stream_util::read_vec4(is);
    return glm::perspective(v.x, v.y, v.z, v.w);
  }

  //    matrix lookat vec3 vec3 vec3
  if(word == "lookat")
  {
    vec3 v0 = stream_util::read_vec3(is);
    vec3 v1 = stream_util::read_vec3(is);
    vec3 v2 = stream_util::read_vec3(is);
    return glm::lookAt(v0, v1, v2);
  }

  if(word == "plain")
  {
    return stream_util::read_mat(is);
  }

  throw std::runtime_error("unknow matrix type : " + word);
}

ELEMENT to(const std::string& s)
{
  if(s == "space")
    return SPACE;
  if(s == "matrix")
    return MATRIX;
  if(s == "print")
    return PRINT;
  if(s == "pm")
    return PRINT_MATRIX;
  if(s == "pv")
    return PRINT_VECTOR;

  std::stringstream ss;
  ss << "illegal elem \""<< s << "\"";
  throw std::runtime_error(ss.str());
}

void add_space(std::istream& is)
{
  space_names.push_back(read_word(is));
  std::cout << "add space "<< space_names.back() << std::endl;
}

void add_print_matrix(std::istream& is)
{
  print_matrix pd;
  is >> pd.start;
  is >> pd.count;
  if(is.fail())
    throw std::runtime_error("failed to add print matrix");
  pms.push_back(pd);
}

void add_print_vector(std::istream& is)
{
  print_vector pd;
  is >> pd.start;
  is >> pd.count;
  pd.v = stream_util::read_vec4(is);
  //if(is.fail())
    //throw std::runtime_error("failed to add print vector");
  pvs.push_back(pd);
}

void add_print_item(std::istream& is)
{
  ELEMENT elem = to(read_word(is));
  assert_true(elem == PRINT_MATRIX || elem == PRINT_VECTOR, 
      "error, only pm or pv can follow PRINT, PRINT_MATRIX or PRINT_VECTOR");
  if(elem == PRINT_MATRIX)
    add_print_matrix(is);
  else
    add_print_vector(is);
}

int main(int argc, char *argv[])
{
  if(argc == 1)
  {
    // set data file as MVP_MATRIX_FILE
    char* c = std::getenv("MVP_MATRIX_FILE");
    if(c == 0)
    {
      std::cout << "empty MVP_MATRIX_FILE, pls provide matrix file or specify MVP_MATRIX_FILE" << std::endl;
      return 0 ;
    }
    else
      file_path = c;
  }
  else
  {
    file_path = argv[1];
  }
  std::cout << "read content from " << file_path << std::endl;

  std::cout.precision(6);
  std::cout.setf(std::ios_base::left);
  //std::cout.flags(std::ios_base::fixed);
  //std::cout.unsetf(std::ios_base::showpoint);

  // std::ifstream ifs(file_path);
  // get space matrixes and vector

  ELEMENT elem = INITIAL;
  file_content = stream_util::read_resource(file_path);
  replace_contents();
  std::stringstream ifs(file_content);
  std::string line;
  mat4 m;

  while(std::getline(ifs, line))
  {
    line = string_util::trim(line);
    // skip blank and comment
    if(line.empty() || is_comment(line))
      continue;

    std::stringstream ss(line);

    if(elem == INITIAL)
    {
      elem = to(read_word(ss));
      assert_element(elem, SPACE, "error, 1st non comment or blank line should be space");
      add_space(ss);
    }
    else if(elem == SPACE)
    {
      // read next space
      elem = to(read_word(ss));
      assert_element(elem, MATRIX,"error, at least 1 matrix should follow space");
      if(elem == MATRIX)
      {
        m = m * read_mat4(ss);
      }
    }
    else if(elem == MATRIX)
    {
      // accumulate current matrix
      elem = to(read_word(ss));
      if(elem == MATRIX)
      {
        m = m * read_mat4(ss);
      }
      else if(elem == SPACE)
      {
        mats.push_back(m);
        m = mat4(1);
        add_space(ss);
      }
      else if(elem == PRINT)
      {
        mats.push_back(m);
        m = mat4(1);
      }
      else
        throw std::runtime_error("only matrix, space and print can follow matrix");
    }
    else if(elem == PRINT)
    {
      add_print_item(ss);
    }
    else
    {
      add_print_item(ss);
    }
  }

  std::cout << "finish reading, start printing" << std::endl;

  std::for_each(pms.begin(), pms.end(),[&](decltype(*pms.begin()) v) {
    v.print();
  });

  std::for_each(pvs.begin(), pvs.end(),[&](decltype(*pvs.begin()) v) {
    v.print();
  });

  return 0;
}
