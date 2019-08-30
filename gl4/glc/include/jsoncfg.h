#ifndef GL4_GLC_JSONCFG_H
#define GL4_GLC_JSONCFG_H

#include <string>
#include <ctime>
#include <memory>
#include <rapidjson/fwd.h>
#include "glm.h"

namespace zxd
{

class json_cfg
{
public:
  json_cfg(const std::string& jsonfile = "");

  void load(const std::string& jsonfile);
  void reload();
  bool modified_outside();

  rapidjson::Value& get_value(const std::string& name) const;

  bool get_bool(const std::string& name) const;
  int get_int(const std::string& name) const;
  float get_float(const std::string& name) const;
  double get_double(const std::string& name) const;
  std::string get_string(const std::string& name) const;

  const std::string& get_file() const { return m_file; }
  void set_file(const std::string& v) { m_file = v; }

private:
  std::time_t m_last_write_time;
  std::string m_file;
  std::shared_ptr<rapidjson::Document> m_doc;
};


vec2 read_json_vec2(const rapidjson::Value& v);
vec3 read_json_vec3(const rapidjson::Value& v);
vec4 read_json_vec4(const rapidjson::Value& v);

} /* zxd */

#endif /* GL4_GLC_JSONCFG_H */
