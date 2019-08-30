#include "dict_script.h"

#include <fstream>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>
#include "string_util.h"
#include "stream_util.h"

namespace bfs = boost::filesystem;

namespace zxd
{

//--------------------------------------------------------------------
file_observer::file_observer(const std::string& file_path, float time_interval, std::atomic_bool* atomic_changed):
  m_file_path(file_path),
  m_interval(time_interval),
  m_atomic_changed(atomic_changed)
{
  m_last_w_time = bfs::last_write_time(m_file_path);
}

//--------------------------------------------------------------------
file_observer::~file_observer()
{
}

//--------------------------------------------------------------------
void file_observer::operator()()
{
  std::chrono::milliseconds interval(static_cast<int>(m_interval * 1000));
  while(true)
  {
    std::this_thread::sleep_for(interval);
    try
    {
      // throws if file is being written?
      auto t = bfs::last_write_time(m_file_path);
      if(t != m_last_w_time)
      {
        std::cout << "found change " << std::endl;
        m_last_w_time = t;
        //std::cout << "read " << m_file_path << " at " <<  std::ctime(&m_last_w_time) << std::endl;
        m_atomic_changed->store(true);
      }
    }catch(std::exception& e)
    {
      std::cout << e.what() << std::endl;
    }
  }
}

//--------------------------------------------------------------------
dict_script::dict_script(const std::string& file_path):
  m_file_path(file_path),
  m_thread(0)
{
  read(file_path);
}

dict_script::dict_script()
{
}

//--------------------------------------------------------------------
dict_script::~dict_script()
{
}

//--------------------------------------------------------------------
std::string dict_script::get(const std::string& key, const std::string& fallback/* = ""*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : iter->second;
}

//--------------------------------------------------------------------
float dict_script::get(const std::string& key, float fallback/* = 0*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : std::stof(iter->second);
}

//--------------------------------------------------------------------
glm::vec2 dict_script::get(const std::string& key, const glm::vec2& fallback/* = vec2(0)*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : string_util::to_vec2(iter->second);
}

//--------------------------------------------------------------------
glm::vec3 dict_script::get(const std::string& key, const glm::vec3& fallback/* = vec3(0)*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : string_util::to_vec3(iter->second);
}

//--------------------------------------------------------------------
glm::vec4 dict_script::get(const std::string& key, const glm::vec4& fallback/* = vec4(0)*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : string_util::to_vec4(iter->second);
}

//--------------------------------------------------------------------
glm::mat4 dict_script::get(const std::string& key, const glm::mat4& fallback/* = mat4(1)*/)
{
  auto iter = m_dict.find(key);
  return iter == m_dict.end() ? fallback : string_util::to_mat4(iter->second);
}

//--------------------------------------------------------------------
void dict_script::track(float interval)
{
  if(m_thread)
  {
    delete m_thread;
    m_thread = 0;
  }
  m_thread = new std::thread(file_observer(m_file_path, interval, &m_atomic_changed));
}

//--------------------------------------------------------------------
void dict_script::read(const std::string& file_path)
{
  m_file_path = file_path;
  m_atomic_changed.store(true);
  update();
}

//--------------------------------------------------------------------
bool dict_script::changed()
{
  return m_atomic_changed;
}

//--------------------------------------------------------------------
void dict_script::update()
{
  if(m_atomic_changed)
  {
    read();
    m_atomic_changed.store(false);
  }
}

//--------------------------------------------------------------------
void dict_script::read()
{
  std::cout << "reading..." << std::endl;
  std::ifstream ifs(m_file_path);
  if(!ifs)
  {
    throw std::runtime_error("failed to open " + m_file_path);
  }

  std::string line;

  std::string key;
  std::string value;

  while(std::getline(ifs,line))
  {
    if(is_comment(line))
      continue;

    auto pos = line.find_first_of(":");
    if(pos == std::string::npos)
    {
      std::cout << "missing \":\" in this line : " << std::endl;
      std::cout << "    " <<  line << std::endl;
      continue;
    }
    key = string_util::trim(line.substr(0, pos));
    value = string_util::trim(line.substr(pos + 1));
    m_dict[key] = value;
    std::cout << "   \"" << key << "\" : \"" << value << "\"" <<  std::endl;
  }
}

//--------------------------------------------------------------------
bool dict_script::is_comment(const std::string& line)
{
  std::string s =  string_util::trim(line, true, false);
  return !s.empty() && s.front() == '#';
}

}
