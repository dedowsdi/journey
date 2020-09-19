#ifndef TOY_RESOURCE_H
#define TOY_RESOURCE_H

#include <functional>
#include <stdexcept>
#include <vector>
#ifdef WIN32

#else
#    include <sys/stat.h>
#endif

#include <osg/Node>

namespace toy
{

class Resource
{
public:
    using ModifiedCallback = std::function<void(const std::string&)>;

    Resource(const std::string& file, ModifiedCallback ModifiedCallback);

    bool check();

    void invokeCallback();

    struct ResourceNotFoundError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    const std::string& getFile() const { return _file; }
    void setFile(const std::string& v) { _file = v; }

private:

#ifdef WIN32
    time_t _mtime = 0;
#else
    timespec _mtime = {0, 0};
#endif

    std::string _file;
    ModifiedCallback _callback;
};

Resource createShaderResource(osg::Shader* shader);

using ResourceList = std::vector<Resource>;

class ResourceObserver : public osg::Callback
{
public:
    bool run(osg::Object* object, osg::Object* data) override;

    void addResource(const Resource& resource);

    void addResource(osg::Shader* shader);

    // observe all shaders in this prg, no further reference required, so it's &
    void addResource(osg::Program& prg);

    void clear();

private:
    ResourceList _resources;
};

}  // namespace toy

#endif  // TOY_RESOURCE_H
