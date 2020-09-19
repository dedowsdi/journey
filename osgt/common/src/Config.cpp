#include <Config.h>

#include <iomanip>
#include <sstream>

#include <osg/os_utils>
#include <osgDB/Registry>
#include <ToyMath.h>

namespace toy
{

void Config::load(const std::string& file)
{
    _file = file;
    reload();
}

void Config::reload()
{
    _dict.clear();

    if (_file.empty())
    {
        OSG_FATAL << "Empty config file" << std::endl;
        return;
    }

    auto configPath =
        osgDB::Registry::instance()->findDataFile(_file, 0, osgDB::CASE_SENSITIVE);

    if (configPath.empty())
    {
        OSG_FATAL << "missing config file" << std::endl;
        return;
    }

    std::ifstream ifs(configPath);

    std::string line;
    while (std::getline(ifs, line))
    {
        std::string key;
        std::istringstream iss(line);
        iss >> key;

        if (key.empty() || key.front() == '#')
            continue;

        std::string value;

        if (line.size() > key.size())
        {
            value = line.substr(line.find_first_not_of(" \t", key.size()));
        }

        if (value.empty())
        {
            OSG_WARN << "missing value for key : " << key << "\n";
            continue;
        }

        _dict.insert(std::make_pair(key, value));
    }
}

void Config::clear() {}

std::istream& operator>>(std::istream& input, osg::Matrixf& m)
{
    for (auto row = 0; row < 4; ++row)
    {
        for (auto col = 0; col < 4; ++col)
        {
            input >> m(row, col);
            if (row != 3 || col != 3)
                input >> std::ws;
        }
    }
    return input;
}

std::istream& operator>>(std::istream& input, osg::Matrixd& m)
{
    for (auto row = 0; row < 4; ++row)
    {
        for (auto col = 0; col < 4; ++col)
        {
            input >> m(row, col);
            if (row != 3 || col != 3)
                input >> std::ws;
        }
    }
    return input;
}

osg::Vec4 Config::getColor(const std::string& key)
{
    return htmlColorToVec4(getString(key));
}

std::string Config::search(const std::string& key)
{
    auto iter = _dict.find(key);
    return iter == _dict.end() ? "" : iter->second;
}

Config::Config()
{
}

}  // namespace toy
