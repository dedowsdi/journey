#include "jsoncfg.h"

#include <iostream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <boost/filesystem.hpp>
#include "stream_util.h"

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
  return bfs::last_write_time(m_file) > m_last_write_time;
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
}
