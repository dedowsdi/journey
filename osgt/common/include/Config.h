#ifndef UFO_CONFIG_H
#define UFO_CONFIG_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <osg/Matrixd>
#include <osg/Matrixf>
#include <osg/Vec2>
#include <osg/Vec2s>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/io_utils>

namespace toy
{

#define sgc ::toy::Config::instance()

std::istream& operator>>(std::istream& input, osg::Matrixf& m);

std::istream& operator>>(std::istream& input, osg::Matrixd& m);

class Config
{
public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& instance()
    {
        static Config instance;
        return instance;
    }

    void load(const std::string& file);

    void reload();

    void clear();

    template<typename T>
    T get(const std::string& key)
    {
        auto iter = _dict.find(key);
        if (iter == _dict.end())
        {
            OSG_NOTICE << key << " not found." << std::endl;
            return T();
        }

        T t;
        std::istringstream iss(iter->second);
        iss >> t;

        if (!iss)
        {
            OSG_WARN << "Failed to convert " << key << "\n";
            return T();
        }

        return t;
    }

    template<typename T>
    std::vector<T> getVector(const std::string& key)
    {
        std::vector<T> v;

        auto iter = _dict.find(key);
        if (iter == _dict.end())
        {
            OSG_NOTICE << key << " not found." << std::endl;
            return v;
        }

        std::istringstream iss(iter->second); 
        T t;
        while (iss >> t)
        {
            v.push_back(t);
        }

        if (v.empty())
        {
            OSG_NOTICE << "Failed to read " << key << std::endl;
        }

        return v;
    }

    bool getBool(const std::string& key) { return get<bool>(key); }
    char getChar(const std::string& key) { return get<char>(key); }
    short getShort(const std::string& key) { return get<short>(key); }
    int getInt(const std::string& key) { return get<int>(key); }
    unsigned char getUnsignedChar(const std::string& key)
    {
        return get<unsigned char>(key);
    }
    unsigned short getUnsignedShort(const std::string& key)
    {
        return get<unsigned short>(key);
    }
    unsigned getUnsigned(const std::string& key) { return get<unsigned>(key); }
    float getFloat(const std::string& key) { return get<float>(key); }
    double getDouble(const std::string& key) { return get<double>(key); }
    osg::Vec2s getVec2s(const std::string& key) { return get<osg::Vec2s>(key); }
    osg::Vec2 getVec2(const std::string& key) { return get<osg::Vec2>(key); }
    osg::Vec3 getVec3(const std::string& key) { return get<osg::Vec3>(key); }
    osg::Vec4 getVec4(const std::string& key) { return get<osg::Vec4>(key); }
    osg::Matrixf getMatrixf(const std::string& key) { return get<osg::Matrixf>(key); }
    osg::Matrixd getMatrixd(const std::string& key) { return get<osg::Matrixd>(key); }
    std::string getString(const std::string& key) { return get<std::string>(key); }
    osg::Vec4 getColor(const std::string& key);
    std::vector<float> getFloats(const std::string& key) { return getVector<float>(key); }

    // return "" if not found, no complain
    std::string search(const std::string& key);

private:
    Config();
    ~Config() = default;

    std::string _file;
    using ConfigMap = std::map<std::string, std::string>;
    ConfigMap _dict;
};

}  // namespace toy

#endif  // UFO_CONFIG_H
