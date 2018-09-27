#ifndef DICT_SCRIPT_H
#define DICT_SCRIPT_H

#include <map>
#include <string>
#include <thread>
#include <functional>
#include "glm.h"
#include <atomic>

namespace zxd
{

typedef std::map<std::string, std::string> script_dict;


class file_observer
{
protected:
  time_t m_last_w_time;
  float m_interval;
  std::string m_file_path;
  std::atomic_bool* m_atomic_changed;

public:

  file_observer(const std::string& file_path, float time_interval, std::atomic_bool* atomic_changed);
  ~file_observer();

  void operator()();

};


class dict_script
{
protected:
  bool m_tracking;

  std::string m_file_path;
  script_dict m_dict;

  std::thread* m_thread;
  std::atomic_bool m_atomic_changed;

public:
  dict_script(const std::string& file_path);
  ~dict_script();

  std::string get(const std::string& key, const std::string& fallback = "");
  float get(const std::string& key, float fallback = 0);
  glm::vec2 get(const std::string& key, const glm::vec2& fallback = vec2(0));
  glm::vec3 get(const std::string& key, const glm::vec3& fallback = vec3(0));
  glm::vec4 get(const std::string& key, const glm::vec4& fallback = vec4(0));
  glm::mat4 get(const std::string& key, const glm::mat4& fallback = mat4(1));

  void track(float interval);

  bool changed();
  void update();

protected:
  void read();
  bool is_comment(const std::string& line);

};

}

#endif /* DICT_SCRIPT_H */
