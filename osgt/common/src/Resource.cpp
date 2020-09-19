#include <Resource.h>

#ifdef WIN32

#else
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

#include <cassert>

#include <osgDB/FileUtils>
#include <OsgFactory.h>

namespace toy
{

class StatError : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

const char* statErrorToString(int errorNumber)
{
    switch (errorNumber)
    {
        case EACCES:
            return "EACCES";
        case EBADF:
            return "EBADF";
        case EFAULT:
            return "EFAULT";
        case ELOOP:
            return "ELOOP";
        case ENAMETOOLONG:
            return "ENAMETOOLONG";
        case ENOENT:
            return "ENOENT";
        case ENOMEM:
            return "ENOMEM";
        case ENOTDIR:
            return "ENOTDIR";
        case EOVERFLOW:
            return "EOVERFLOW";
        default:
            return "";
    }
}

auto getLastModifyTime(const std::string& file)
{
    struct stat statbuf;
    if (stat(file.c_str(), &statbuf) != 0)
    {
        throw StatError(std::string("stat failed with ") + statErrorToString(errno));
    }
#ifdef WIN32
    return statbuf.st_mtime;
#else
    return statbuf.st_mtim;
#endif

}

Resource::Resource(const std::string& file, ModifiedCallback ModifiedCallback)
    : _callback(ModifiedCallback)
{
    _file = osgDB::findDataFile(file);
    if (_file.empty())
    {
        throw ResourceNotFoundError(file + " not found in OSG_FILE_PATH ");
    }
}

bool Resource::check()
{
    try
    {
        // reload supershape if file changed
#ifdef WIN32
        auto mtime = getLastModifyTime(_file);
        if (mtime != _mtime)
#else
        auto mtime = getLastModifyTime(_file);
        if (mtime.tv_nsec != _mtime.tv_nsec)
#endif
        {
            _mtime = mtime;
            invokeCallback();
            return true;
        }
    }
    catch (const StatError& e)
    {
        OSG_WARN << e.what() << std::endl;
    }

    return false;
}

void Resource::invokeCallback()
{
    if (_callback)
    {
        _callback(_file);
    }
}

Resource createShaderResource(osg::Shader* shader)
{
    auto& file = shader->getFileName();
    assert(!file.empty());

    auto callback = [shaderRef = osg::ref_ptr<osg::Shader>(shader)](
                        const std::string& f) { osgf::reloadShader(*shaderRef, f); };

    return Resource(file, callback);
}

bool ResourceObserver::run(osg::Object* object, osg::Object* data)
{
    std::map<std::string, bool> checkedFiles;

    auto visitor = data->asNodeVisitor();
    if (visitor)
    {
        for (auto& resource: _resources)
        {
            auto iter = checkedFiles.find(resource.getFile());
            if (iter == checkedFiles.end())
            {
                auto b = resource.check();
                iter = checkedFiles.insert(std::make_pair(resource.getFile(), b)).first;
            }

            if (iter->second)
            {
                resource.invokeCallback();
            }
        }
    }

    return traverse(object, data);
}

void ResourceObserver::addResource(const Resource& resource)
{
    _resources.push_back(resource);
    OSG_NOTICE << "Observing " << resource.getFile() << std::endl;
}

void ResourceObserver::addResource(osg::Shader* shader)
{
    if (!shader)
    {
        return;
    }

    if (shader->getFileName().empty())
    {
        OSG_NOTICE << "Shader " << shader << " has no file" << std::endl;
        return;
    }

    addResource(createShaderResource(shader));
}

void ResourceObserver::addResource(osg::Program& prg)
{
    if (prg.getNumShaders() == 0)
    {
        OSG_WARN << "Nothing to observe, " << prg.getName() << " has 0 shaders"
                 << std::endl;
        return;
    }

    for (auto i = 0; i < prg.getNumShaders(); ++i)
    {
        addResource(prg.getShader(i));
    }
}

void ResourceObserver::clear()
{
    _resources.clear();
}

}  // namespace toy
