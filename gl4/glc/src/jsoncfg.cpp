#include <jsoncfg.h>

#include <iostream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <boost/filesystem.hpp>
#include <stream_util.h>

namespace bfs = boost::filesystem;

namespace zxd
{

//--------------------------------------------------------------------
json_cfg::json_cfg(const std::string& jsonfile)
{
  m_doc = std::make_shared<rapidjson::Document>();
  if (!jsonfile.empty())
  {
    load(jsonfile);
  }
}

//--------------------------------------------------------------------
void json_cfg::load(const std::string& jsonfile)
{
  static std::string types[] = {
    "kNullType",      //!< null
    "kFalseType",     //!< false
    "kTrueType",      //!< true
    "kObjectType",    //!< object
    "kArrayType",     //!< array 
    "kStringType",    //!< string
    "kNumberType"     //!< number
  };

  m_file = stream_util::get_resource(jsonfile);
  auto content = stream_util::read_file(m_file);

  rapidjson::ParseResult res = m_doc->Parse(content.c_str());
  if (!res)
  {
    fprintf(stderr, "JSON parse error: %s (%lu)",
            rapidjson::GetParseError_En(res.Code()), res.Offset());
    exit(EXIT_FAILURE);
  }

  std::cout << "load json : " << m_file
            << ". base type : " << types[m_doc->GetType()] << std::endl;

  m_last_write_time = bfs::last_write_time(m_file);
}

//--------------------------------------------------------------------
void json_cfg::reload()
{
  load(m_file);
}

//--------------------------------------------------------------------
bool json_cfg::modified_outside()
{
  // sometimes boost failed to get last_write_time, a
  // boost::filesystem::filesystem_error is thrown, it usually happens when you
  // constantly write to this file in other process. I have no idea why would
  // boost throw this kind of err.
  try
  {
    return bfs::last_write_time(m_file) > m_last_write_time;
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

//--------------------------------------------------------------------
rapidjson::Value& json_cfg::get_value(const std::string& name) const
{
  return (*m_doc)[name.c_str()];
}

//--------------------------------------------------------------------
bool json_cfg::get_bool(const std::string& name) const
{
  return get_value(name).GetBool();
}

//--------------------------------------------------------------------
int json_cfg::get_int(const std::string& name) const
{
  return get_value(name).GetInt();
}

//--------------------------------------------------------------------
float json_cfg::get_float(const std::string& name) const
{
  return get_value(name).GetFloat();
}

//--------------------------------------------------------------------
double json_cfg::get_double(const std::string& name) const
{
  return get_value(name).GetDouble();
}

//--------------------------------------------------------------------
std::string json_cfg::get_string(const std::string& name) const
{
  return get_value(name).GetString();
}

template <typename T>
auto read_json_vec(const rapidjson::Value& v)
{
   assert(v.IsArray());
   T res;
   for (unsigned i = 0; i < T::length(); ++i)
   {
     res[i] = v[i].GetFloat();
   }
   return res;
}

//--------------------------------------------------------------------
vec2 read_json_vec2(const rapidjson::Value& v)
{
  return read_json_vec<vec2>(v);
}

//--------------------------------------------------------------------
vec3 read_json_vec3(const rapidjson::Value& v)
{
  return read_json_vec<vec3>(v);
}

//--------------------------------------------------------------------
vec4 read_json_vec4(const rapidjson::Value& v)
{
  return read_json_vec<vec4>(v);
}

}
