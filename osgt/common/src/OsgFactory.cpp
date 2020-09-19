#include <OsgFactory.h>

#include <algorithm>
#include <cassert>

#include <osg/AnimationPath>
#include <osg/BlendFunc>
#include <osg/Drawable>
#include <osg/FrameBufferObject>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Hint>
#include <osg/LineWidth>
#include <osg/Program>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/io_utils>
#include <osgAnimation/Action>
#include <osgAnimation/Bone>
#include <osgAnimation/Channel>
#include <osgAnimation/Skeleton>
#include <osgAnimation/StackedMatrixElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osgAnimation/StackedScaleElement>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/UpdateBone>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osg/PolygonMode>
#include <osgGA/GUIEventHandler>

#include <OsgQuery.h>

namespace osgf
{

namespace detail
{

template<typename T>
T mix(const T& v0, const T& v1, float f)
{
    return v0 * (1 - f) + v1 * f;
}

template<>
osg::Quat mix<osg::Quat>(const osg::Quat& v0, const osg::Quat& v1, float f)
{
    osg::Quat q;
    q.slerp(f, v0, v1);
    return q;
}

}  // namespace detail

osg::Geometry* getNdcQuad()
{
    static osg::ref_ptr<osg::Geometry> quad;
    if (!quad)
    {
        quad = osg::createTexturedQuadGeometry(
            osg::Vec3(-1, -1, 0), osg::Vec3(2, 0, 0), osg::Vec3(0, 2, 0));
        quad->setCullingActive(false);
        quad->setName("NdcQuad");
    }
    return quad;
}

osg::Geode* createNdcQuadLeaf()
{
    auto leaf = new osg::Geode;
    leaf->addDrawable(getNdcQuad());
    return leaf;
}

osg::MatrixTransform* createSphereAt(
    const osg::Vec3& pos, float radius, const osg::Vec4& color)
{
    auto root = new osg::MatrixTransform;
    root->setMatrix(osg::Matrix::translate(pos));

    auto sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), radius));
    sphere->setColor(color);
    sphere->setName("DebugSphere");
    root->addChild(sphere);

    return root;
}

osg::MatrixTransform* createSphere(
    const osg::BoundingSphere& sphere, const osg::Vec4& color)
{
    return createSphereAt(sphere.center(), sphere.radius());
}

osg::MatrixTransform* createBoxAt(
    const osg::Vec3& pos, float nx, float ny, float nz, const osg::Vec4& color)
{
    auto root = new osg::MatrixTransform;
    root->setMatrix(osg::Matrix::translate(pos));

    auto cube = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), nx, ny, nz));
    cube->setColor(color);
    cube->setName("DebugCube");
    root->addChild(cube);

    return root;
}

osg::MatrixTransform* createBoxAt(const osg::Vec3& pos, float width, const osg::Vec4& color)
{
    return createBoxAt(pos, width, width, width, color);
}

osg::MatrixTransform* createBox(const osg::BoundingBox& box, const osg::Vec4& color)
{
    return createBoxAt(box.center(), box.xMax() - box.xMin(), box.yMax() - box.yMin(),
        box.zMax() - box.zMin(), color);
}

osg::MatrixTransform* createBoundingBoxWireFrame(
    const osg::BoundingBox& box, const osg::Vec4& color, int binNum)
{
    static auto ss = new osg::StateSet;
    ss->setAttributeAndModes(
        new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    ss->setRenderBinDetails(binNum, "RenderBin");
    ss->setTextureAttributeAndModes(0, 0);

    auto node = createBox(box, color);
    node->setStateSet(ss);

    return node;
}

namespace detail
{

class BakeVisitor : public osg::NodeVisitor
{
public:
    BakeVisitor() { setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN); }

    void apply(osg::Geometry& node) override
    {
        OSG_INFO << "baking " << node.getName() << std::endl;
        auto frame = osg::computeLocalToWorld(getNodePath());
        auto vertices = dynamic_cast<osg::Vec3Array*>(node.getVertexArray());
        if (vertices)
        {
            for (auto& vertex: *vertices)
            {
                vertex = vertex * frame;
            }
        }
        vertices->dirty();
        node.dirtyBound();
        node.dirtyGLObjects();
    }
};

}  // namespace detail

void bakeTransform(osg::Node& node)
{
    detail::BakeVisitor visitor;
    node.accept(visitor);
}

osg::Node* readNodeFile(
    const std::string& name, float desiredRadius, bool shiftToCenter)
{
    auto node = osgDB::readNodeFile(name);
    if (!node)
    {
        return node;
    }

    auto& bs = node->getBound();
    auto scale = desiredRadius / bs.radius();

    auto frame = new osg::MatrixTransform;

    auto m = osg::Matrix::scale(osg::Vec3(scale, scale, scale));
    if(shiftToCenter)
        m.preMultTranslate(-bs.center());

    frame->setMatrix(m);
    frame->addChild(node);

    return frame;
}

template<typename T>
void addGridElements(osg::Geometry& geom, int stacks, int slices, int offset, bool reverse)
{
    auto numElements = 2 * (slices + 1);

    for (auto i = 0; i < stacks; ++i)
    {
        auto elements = new T(GL_TRIANGLE_STRIP);
        elements->reserve(numElements);

        auto stack0 = (slices + 1) * i + offset;
        auto stack1 = stack0 + slices + 1;

        for (auto j = 0; j <= slices; j++)
        {
            if (reverse)
            {
                elements->push_back(stack1 + j);
                elements->push_back(stack0 + j);
            }
            else
            {
                elements->push_back(stack0 + j);
                elements->push_back(stack1 + j);
            }
        }
        geom.addPrimitiveSet(elements);
    }
}

#define INSTANTIATE_addGridElements(T)                                                     \
    template void addGridElements<T>(                                                      \
        osg::Geometry & geom, int stacks, int slices, int offset, bool reverse);
INSTANTIATE_addGridElements(osg::DrawElementsUInt);
INSTANTIATE_addGridElements(osg::DrawElementsUShort);
INSTANTIATE_addGridElements(osg::DrawElementsUByte);

template<typename T>
void addGridTexcoords(T& texcoords, int stacks, int slices, const osg::Vec2& texcoord0,
    const osg::Vec2& texcoord1)
{
    auto sStep = 1.0f / slices;
    auto tStep = 1.0f / stacks;
    for (auto i = 0; i <= stacks; ++i)
    {
        auto t = detail::mix(texcoord0.y(), texcoord1.y(), i * tStep);
        for (auto j = 0u; j <= slices; j++)
        {
            auto s = detail::mix(texcoord0.x(), texcoord1.x(), j * sStep);
            texcoords.push_back(osg::Vec2(s, t));
        }
    }
}

#define INSTANTIATE_addGridTexcoords(T)                                                    \
    template void addGridTexcoords<T>(T & texcoords, int stacks, int slices,               \
        const osg::Vec2& texcoord0, const osg::Vec2& texcoord1);
INSTANTIATE_addGridTexcoords(osg::Vec2Array);
INSTANTIATE_addGridTexcoords(std::vector<osg::Vec2>);

template<typename T>
void addGridTexcoordsSliceBySlice(T& texcoords, int slices, int stacks,
    const osg::Vec2& texcoord0, const osg::Vec2& texcoord1)
{
    auto sStep = 1.0f / slices;
    auto tStep = 1.0f / stacks;
    for (auto i = 0; i <= slices; ++i)
    {
        auto t = detail::mix(texcoord0.y(), texcoord1.y(), i * tStep);
        for (auto j = 0u; j <= stacks; j++)
        {
            auto s = detail::mix(texcoord0.x(), texcoord1.x(), j * sStep);
            texcoords.push_back(osg::Vec2(s, t));
        }
    }
}

#define INSTANTIATE_addGridTexcoordsSliceBySLice(T)                                        \
    template void addGridTexcoordsSliceBySlice<T>(T & texcoords, int stacks, int slices,   \
        const osg::Vec2& texcoord0, const osg::Vec2& texcoord1);
INSTANTIATE_addGridTexcoordsSliceBySLice(osg::Vec2Array);
INSTANTIATE_addGridTexcoordsSliceBySLice(std::vector<osg::Vec2>);

osg::Geometry* createAxes(float size, float lineWidth)
{
    static osg::ref_ptr<osg::StateSet> ss;
    if (!ss)
    {
        ss = new osg::StateSet;
        ss->setMode(GL_DEPTH_TEST, 0);
        ss->setMode(GL_LIGHTING, 0);
        ss->setAttributeAndModes(new osg::LineWidth(lineWidth));
    }

    auto geom = new osg::Geometry;
    geom->setName("Axes");

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->push_back(osg::Vec3(0, 0, 0));
    vertices->push_back(osg::Vec3(size, 0, 0));
    vertices->push_back(osg::Vec3(0, 0, 0));
    vertices->push_back(osg::Vec3(0, size, 0));
    vertices->push_back(osg::Vec3(0, 0, 0));
    vertices->push_back(osg::Vec3(0, 0, size));

    geom->setVertexArray(vertices);

    auto colors = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    colors->push_back(osg::Vec3(1, 0, 0));
    colors->push_back(osg::Vec3(1, 0, 0));
    colors->push_back(osg::Vec3(0, 1, 0));
    colors->push_back(osg::Vec3(0, 1, 0));
    colors->push_back(osg::Vec3(0, 0, 1));
    colors->push_back(osg::Vec3(0, 0, 1));

    geom->setColorArray(colors);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 6));
    geom->setStateSet(ss);

    return geom;
}

osg::Geometry* createPoints(const std::vector<osg::Vec3>& points)
{
    auto geom = new osg::Geometry;

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->assign(points.begin(), points.end());

    geom->setVertexArray(vertices);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vertices->size()));
    return geom;
}

osg::Geometry* createLines(const std::vector<osg::Vec3>& lines)
{
    auto geom = new osg::Geometry;

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->assign(lines.begin(), lines.end());

    geom->setVertexArray(vertices);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices->size()));
    return geom;
}

osg::Geometry* createTorus(float innerRadius, float outerRadius, int sides, int rings)
{
    auto geom = new osg::Geometry;
    auto numVertices = (rings + 1) * (sides + 1);

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->reserve(numVertices);

    geom->setVertexArray(vertices);

    auto normals = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    normals->reserve(numVertices);

    geom->setNormalArray(normals);

    auto texcoords = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
    texcoords->reserve(numVertices);
    addGridTexcoords(*texcoords, rings, sides);

    geom->setTexCoordArray(0, texcoords);

    // theta as azimuthal angle, phi as angle from +z axis
    float thetaStep = osg::PIf * 2.0f / rings;
    float phiStep = osg::PIf * 2.0f / sides;

    for (auto i = 0; i <= rings; ++i)
    {
        auto theta = i == rings ? 0 : thetaStep * i;
        auto cosTheta = std::cos(theta);
        auto sinTheta = std::sin(theta);
        auto ringCenter = osg::Vec3(cosTheta, sinTheta, 0) * outerRadius;

        for (auto j = 0; j <= sides; ++j)
        {
            float phi = phiStep * j;

            // projection radius of current point on xy plane
            float r = outerRadius + innerRadius * cos(phi);

            vertices->push_back(
                osg::Vec3(cosTheta * r, sinTheta * r, innerRadius * sin(phi)));

            auto normal = vertices->back() - ringCenter;
            normal.normalize();
            normals->push_back(normal);
        }
    }

    assert(vertices->size() == numVertices);
    assert(normals->size() == numVertices);
    assert(texcoords->size() == numVertices);

    addGridElements<osg::DrawElementsUInt>(*geom, rings, sides);

    return geom;
}

osg::Geometry* createCircle(const osg::Vec3& origin, const osg::Vec3& point0,
    const osg::Vec3& normal, int sides, bool createNormal, bool createTexcoord, float l,
    float b, float r, float t)
{
    auto geom = new osg::Geometry;

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->reserve(sides + 2);
    vertices->push_back(origin);
    geom->setVertexArray(vertices);

    osg::ref_ptr<osg::Vec2Array> texcoords;
    if (createTexcoord)
    {
        texcoords = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
        texcoords->reserve(vertices->capacity());
        texcoords->push_back(osg::Vec2((l + r) * 0.5f, (b + t) * 0.5f));
        geom->setTexCoordArray(0, texcoords);
    }

    if (createNormal)
    {
        auto normals = new osg::Vec3Array(osg::Array::BIND_OVERALL);
        normals->push_back(normal);
        geom->setNormalArray(normals);
    }

    auto axis0 = point0 - origin;
    float radius = axis0.normalize();

    auto axis1 = normal ^ axis0;
    axis1.normalize();

    for (auto i = 0; i < sides; ++i)
    {
        auto theta = osg::PIf * 2.0f * static_cast<float>(i) / sides;
        auto c = cos(theta);
        auto s = sin(theta);
        auto vertex = origin + (axis0 * c + axis1 * s) * radius;
        vertices->push_back(vertex);

        if (createTexcoord)
        {
            texcoords->push_back(osg::Vec2(
                detail::mix(l, r, c * 0.5 + 0.5), detail::mix(b, t, s * 0.5 + 0.5)));
        }
    }

    // close circle
    vertices->push_back((*vertices)[1]);
    if (createTexcoord)
    {
        texcoords->push_back((*texcoords)[1]);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, vertices->size()));

    return geom;
}

osg::Geometry* createCircleLines(
    const osg::Vec3& origin, const osg::Vec3& point0, const osg::Vec3& normal, int sides)
{

    auto geom = new osg::Geometry;

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->reserve(sides + 1);
    geom->setVertexArray(vertices);

    auto axis0 = point0 - origin;
    float radius = axis0.normalize();

    auto axis1 = normal ^ axis0;
    axis1.normalize();

    for (auto i = 0; i < sides; ++i)
    {
        auto theta = osg::PIf * 2.0f * static_cast<float>(i) / sides;
        auto c = cos(theta);
        auto s = sin(theta);
        auto vertex = origin + (axis0 * c + axis1 * s) * radius;
        vertices->push_back(vertex);
    }

    // close circle
    vertices->push_back((*vertices)[1]);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));

    return geom;
}

namespace detail
{
class NodeCallback : public osg::Callback
{
public:
    NodeCallback(CallbackFunction func) : _func(func) {}

    bool run(osg::Object* object, osg::Object* data) override
    {
        _func(object, data);
        return traverse(object, data);
    }

private:
    CallbackFunction _func;
};

class PruneCallback : public osg::Callback
{
public:
    PruneCallback() {}

    bool run(osg::Object* object, osg::Object* data) override
    {
        return false;
    }
};

class TimerFuncCallback : public osg::Callback
{
public:
    TimerFuncCallback(double time, CallbackFunction func) : _time(time), _func(func) {}

    bool run(osg::Object* object, osg::Object* data) override
    {
        auto visitor = data->asNodeVisitor();
        if (visitor->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
        {
            auto currentTime = visitor->getFrameStamp()->getSimulationTime();
            if (_lastTime == 0)
            {
                _lastTime = currentTime;
            }
            double deltaTime = currentTime - _lastTime;
            _lastTime = currentTime;

            _time -= deltaTime;
            if (_time < 0)
            {
                _func(object, data);
                auto node = object->asNode();
                if (node)
                {
                    osg::ref_ptr<Callback> callback = this;
                    node->removeUpdateCallback(this);
                    return callback->traverse(object, data);
                }
            }
        }
        return traverse(object, data);
    }

private:
    double _time = 0;
    double _lastTime = 0;
    CallbackFunction _func;
};

}  // namespace detail

osg::Callback* createCallback(CallbackFunction callback)
{
    return new detail::NodeCallback(callback);
}

namespace detail
{
class StateSetCallback : public osg::StateSet::Callback
{
public:
    StateSetCallback(StateSetCallbackFunction func) : _func(func) {}

    void operator()(osg::StateSet* object, osg::NodeVisitor* data) override
    {
        _func(object, data);
    }

private:
    StateSetCallbackFunction _func;
};
}

void* createStateSetCallback(StateSetCallbackFunction func)
{
    auto callback = new detail::StateSetCallback(func);
    return callback;
}

namespace detail
{
class QuadResizeHandler : public osgGA::GUIEventHandler
{
public:
    QuadResizeHandler(osg::Geometry* quad, const osg::Vec3& widthDir, const osg::Vec3& heightDir):
        _quad(quad),
        _widthDir(widthDir),
        _heightDir(heightDir)
    {
    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        switch (ea.getEventType())
        {
            case osgGA::GUIEventAdapter::RESIZE:
            {
                osg::Vec3 corner;
                osg::Vec3 heightVec = _heightDir * ea.getWindowHeight();
                osg::Vec3 widthVec = _widthDir * ea.getWindowWidth();

                // vertex starts from left top
                auto& vertices = dynamic_cast<osg::Vec3Array&>(*_quad->getVertexArray());
                vertices[0] = corner + heightVec;
                vertices[1] = corner;
                vertices[2] = corner + widthVec;
                vertices[3] = corner + widthVec + heightVec;
            }
            break;
            default:
                break;
        }
        return false;  // return true will stop event
    }
    osg::Geometry* _quad = 0;
    osg::Vec3 _widthDir;
    osg::Vec3 _heightDir;
};
}

void addQuadResizeCallback(
    osg::Geometry* quad, const osg::Vec3& widthDir, const osg::Vec3& heightDir)
{
    quad->addEventCallback(new detail::QuadResizeHandler(quad, widthDir, heightDir));
}

osg::Callback* createTimerUpdateCallback(double time, CallbackFunction callback)
{
    return new detail::TimerFuncCallback(time, callback);
}

osg::Callback* getPruneCallback()
{
    static auto callback = new detail::PruneCallback;
    return callback;
}

osg::Callback* createTimerRemoveNodeUpdateCallback(double time, osg::Node* node)
{
    auto callback = [node](osg::Object* object, osg::Object* data) {
        auto numParents = node->getNumParents();
        for (auto i = numParents; i > 0; --i)
        {
            node->getParent(i - 1)->removeChild(node);
        }
    };
    return createTimerUpdateCallback(time, callback);
}

namespace detail
{
class ComputeBoundingBoxFuncCallback : public osg::Drawable::ComputeBoundingBoxCallback
{
public:
    ComputeBoundingBoxFuncCallback(ComputeBoundingBoxCallbackFunction func) : _func(func) {}

    osg::BoundingBox computeBound(const osg::Drawable& v) const { return _func(v); }

private:
    ComputeBoundingBoxCallbackFunction _func;
};
}  // namespace detail

void* createComputeBoundingBoxCallback(ComputeBoundingBoxCallbackFunction func)
{
    return new detail::ComputeBoundingBoxFuncCallback(func);
}

void* getInvalidComputeBoundingBoxCallback()
{
    static osg::ref_ptr<osg::Drawable::ComputeBoundingBoxCallback> callback =
        static_cast<osg::Drawable::ComputeBoundingBoxCallback*>(
            createComputeBoundingBoxCallback(
                [=](const osg::Drawable& d) { return osg::BoundingBox(); }));
    return callback.get();
}

namespace detail
{
struct InvalidComputeBoundingSphereCallback
    : public osg::Node::ComputeBoundingSphereCallback
{
    virtual osg::BoundingSphere computeBound(const osg::Node&) const
    {
        return osg::BoundingSphere();
    }
};
}  // namespace detail

void* getInvalidComputeBoundingSphereCallback()
{
    static osg::ref_ptr<detail::InvalidComputeBoundingSphereCallback> callback =
        new detail::InvalidComputeBoundingSphereCallback;
    return callback.get();
}

void addConstantComputeBoundingBoxCallback(osg::Drawable& d, const osg::BoundingBox& bb)
{
    auto cbbCallback =
        osgf::createComputeBoundingBoxCallback([=](const osg::Drawable& d) { return bb; });
    d.setComputeBoundingBoxCallback(
        static_cast<osg::Drawable::ComputeBoundingBoxCallback*>(cbbCallback));
}

namespace detail
{
class ComputeBoundingSphereFuncCallback : public osg::Node::ComputeBoundingSphereCallback
{
public:
    ComputeBoundingSphereFuncCallback(ComputeBoundingSphereCallbackFunction func)
        : _func(func)
    {
    }

    osg::BoundingSphere computeBound(const osg::Drawable& v) const { return _func(v); }

private:
    ComputeBoundingSphereCallbackFunction _func;
};
}  // namespace detail

void* createComputeBoundingSphereCallback(ComputeBoundingSphereCallbackFunction func)
{
    return new detail::ComputeBoundingSphereFuncCallback(func);
}

void addConstantComputeBoundingSphereCallback(osg::Node& d, const osg::BoundingSphere& bs)
{
    auto cbsCallback =
        osgf::createComputeBoundingSphereCallback([=](const osg::Node& d) { return bs; });

    d.setComputeBoundingSphereCallback(
        static_cast<osg::Node::ComputeBoundingSphereCallback*>(cbsCallback));
}

namespace detail
{
class CameraDrawCallback : public osg::Camera::DrawCallback
{
public:
    CameraDrawCallback(CameraDrawCallbackFunction func) : _func(func) {}

    void operator()(osg::RenderInfo& renderInfo) const override { _func(renderInfo); }

private:
    CameraDrawCallbackFunction _func;
};
}

void* createCameraDrawCallback(CameraDrawCallbackFunction func)
{
    return new detail::CameraDrawCallback(func);
}

osg::Geometry* createTearDrop(float radius, float xyScale, int stacks, int slices)
{
    auto geom = new osg::Geometry;
    auto numVertices = (stacks + 1) * (slices + 1);

    auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
    vertices->reserve(numVertices);

    geom->setVertexArray(vertices);

    auto texcoords = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
    texcoords->reserve(numVertices);
    addGridTexcoords(*texcoords, stacks, slices);

    geom->setTexCoordArray(0, texcoords);

    // theta as azimuthal angle, phi as angle from +z axis (different from
    // paulbourke's site)
    float thetaStep = osg::PIf * 2.0f / slices;
    float phiStep = osg::PIf / stacks;

    for (auto i = 0; i <= stacks; ++i)
    {
        float phi = i * phiStep;

        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);
        float z = radius * cosPhi;

        float sf = xyScale * (1 - cosPhi) * sinPhi * radius;

        for (auto j = 0; j <= slices; j++)
        {
            float theta = thetaStep * j;
            if (j == slices)
                theta = 0;  // avoid precision problem

            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            vertices->push_back(osg::Vec3(sf * cosTheta, sf * sinTheta, z));
        }
    }

    addGridElements<osg::DrawElementsUInt>(*geom, stacks, slices);

    osgUtil::SmoothingVisitor sv;
    geom->accept(sv);

    return geom;
}

osg::Program* createProgram(const std::string& fragFile)
{
    auto prg = new osg::Program;

    auto fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT, fragFile);
    prg->addShader(fragShader);

    return prg;
}

osg::Program* createProgram(const std::string& vertFile, const std::string& fragFile)
{
    auto prg = new osg::Program;

    auto vertShader = osgDB::readShaderFile(osg::Shader::VERTEX, vertFile);
    prg->addShader(vertShader);

    auto fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT, fragFile);
    prg->addShader(fragShader);

    return prg;
}

osg::Program* createProgram(const std::string& vertFile, const std::string& geomFile,
    const std::string& fragFile, int inputType, int outputType, int maxVertices)
{
    auto prg = new osg::Program;
    prg->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, inputType);
    prg->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, outputType);
    prg->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, maxVertices);

    auto vertShader = osgDB::readShaderFile(osg::Shader::VERTEX, vertFile);
    prg->addShader(vertShader);

    auto geomShader = osgDB::readShaderFile(osg::Shader::GEOMETRY, geomFile);
    prg->addShader(geomShader);

    auto fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT, fragFile);
    prg->addShader(fragShader);

    return prg;
}

void reloadShader(osg::Shader& shader, const std::string& file)
{
    // no cache
    static auto options = new osgDB::Options();

    auto sourceFile = shader.getFileName();
    if (sourceFile.empty())
    {
        sourceFile = file;
    }

    // A new Shader is created, Shader::_computeShaderDefines will be called twice! Are
    // there any other way to do this?
    auto s = osgDB::readShaderFile(shader.getType(), sourceFile, options);
    shader.setShaderSource(s->getShaderSource());
}

osg::Camera* createPrerenderCamera(int x, int y, int width, int height)
{
    auto camera = new osg::Camera;
    camera->setViewport(x, y, width, height);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    camera->setClearMask(0);
    return camera;
}

osg::Camera* createRttCamera(int x, int y, int width, int height, int implementation)
{
    auto camera = createPrerenderCamera(x, y, width, height);
    camera->setRenderTargetImplementation(
        static_cast<osg::Camera::RenderTargetImplementation>(implementation));
    return camera;
}

osg::Camera* createFilterCamera(int x, int y, int width, int height)
{
    auto camera = createRttCamera(x, y, width, height, osg::Camera::FRAME_BUFFER_OBJECT);
    camera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    return camera;
}

osg::Camera* createOrthoCamera(
    double left, double right, double bottom, double top, double near, double far)
{
    auto camera = new osg::Camera;
    auto proj = osg::Matrix::ortho(left, right, bottom, top, near, far);
    camera->setProjectionMatrix(proj);
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setClearMask(0);
    return camera;
}

osg::FrameBufferObject* addFboRtt(
    osg::StateSet& ss, osg::Texture2D* tex0, osg::Texture2D* tex1, osg::Texture2D* tex2)
{
    assert(tex0);

    auto fbo = new osg::FrameBufferObject();
    fbo->setAttachment(osg::Camera::COLOR_BUFFER0, osg::FrameBufferAttachment(tex0));
    ss.setAttributeAndModes(fbo);
    ss.setAttributeAndModes(
        new osg::Viewport(0, 0, tex0->getTextureWidth(), tex0->getTextureHeight()));

    if (tex1)
    {
        if (tex0->getTextureWidth() != tex1->getTextureWidth() ||
            tex0->getTextureHeight() != tex1->getTextureHeight())
        {
            OSG_WARN << "inconsistent texture size." << std::endl;
        }
        fbo->setAttachment(osg::Camera::COLOR_BUFFER1, osg::FrameBufferAttachment(tex1));
    }

    if (tex2)
    {
        if (tex0->getTextureWidth() != tex2->getTextureWidth() ||
            tex0->getTextureHeight() != tex2->getTextureHeight())
        {
            OSG_WARN << "inconsistent texture size." << std::endl;
        }
        fbo->setAttachment(osg::Camera::COLOR_BUFFER2, osg::FrameBufferAttachment(tex2));
    }

    return fbo;
}

void addLineSmooth(osg::StateSet& ss, float lineWidth)
{
    ss.setAttribute(new osg::LineWidth(lineWidth));
    ss.setMode(GL_LINE_SMOOTH, 1);
    ss.setAttribute(new osg::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST));
    ss.setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

class BlankDrawable : public osg::Drawable
{
public:
    BlankDrawable() = default;

    BlankDrawable(DrawableDrawFunc func) : _drawFunc(func) { setName("BlankDrawable"); }

    BlankDrawable(const osg::Drawable& drawable,
        const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : Drawable(drawable, copyop)
    {
    }

    META_Node(toy, BlankDrawable);

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const
    {
        if (_drawFunc)
        {
            _drawFunc(renderInfo, this);
        }
        else
        {
            OSG_WARN << "Zero draw func" << std::endl;
        }
    }

    DrawableDrawFunc getDrawFunc() const { return _drawFunc; }
    void setDrawFunc(DrawableDrawFunc v) { _drawFunc = v; }

private:
    DrawableDrawFunc _drawFunc;
};

osg::Drawable* createDrawable(DrawableDrawFunc func)
{
    return new BlankDrawable(func);
}

osg::Drawable* createClearDrawable(int mask, const osg::Vec4& clearColor, float depth)
{
    auto func = [mask, clearColor, depth](osg::RenderInfo&, const osg::Drawable*) -> void {
        if (mask & GL_COLOR_BUFFER_BIT)
        {
            glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), clearColor.w());
            glClear(GL_COLOR_BUFFER_BIT);
        }

        if (mask & GL_DEPTH_BUFFER_BIT)
        {
            glClearDepth(depth);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    };

    auto drawable = new BlankDrawable(func);
    drawable->setName("ClearDrawable");
    return drawable;
}

osg::Texture2D* createTexture2D(int internalFormat, int width, int height, int minFilter,
    int magFilter, int wrapS, int wrapT)
{
    auto tex = new osg::Texture2D;
    tex->setInternalFormat(internalFormat);
    tex->setTextureSize(width, height);
    tex->setFilter(
        osg::Texture::MIN_FILTER, static_cast<osg::Texture::FilterMode>(minFilter));
    tex->setFilter(
        osg::Texture::MAG_FILTER, static_cast<osg::Texture::FilterMode>(magFilter));
    tex->setWrap(osg::Texture::WRAP_S, static_cast<osg::Texture::WrapMode>(wrapS));
    tex->setWrap(osg::Texture::WRAP_T, static_cast<osg::Texture::WrapMode>(wrapT));
    return tex;
}

osg::Drawable* createCopyTex2DDrawable(
    osg::Texture2D* tex, int xoffset, int yoffset, int x, int y, int width, int height)
{
    if (width == 0)
        width = tex->getTextureWidth();

    if (height == 0)
        height = tex->getTextureHeight();

    if (width == 0 || height == 0)
        OSG_WARN << __FUNCTION__ << " : 0 width or height" << std::endl;

    auto func = [=](osg::RenderInfo& renderInfo, const osg::Drawable*) -> void {
        tex->copyTexSubImage2D(
            *renderInfo.getState(), xoffset, yoffset, x, y, width, height);
    };

    auto drawable = new BlankDrawable(func);
    drawable->setName("CopyTex2DDrawable");
    return drawable;
}

osg::Texture2D* createChessTexture(int width, int height, int gridWidth, int gridHeight,
    const osg::Vec4& black, const osg::Vec4& white)
{
    auto tex = new osg::Texture2D;
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);

    auto img = new osg::Image;
    tex->setImage(img);

    img->allocateImage(width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    auto data = reinterpret_cast<osg::Vec4ub*>(img->data());

    auto blackub =
        osg::Vec4ub(255 * black.x(), 255 * black.y(), 255 * black.z(), 255 * black.w());
    auto whiteub =
        osg::Vec4ub(255 * white.x(), 255 * white.y(), 255 * white.z(), 255 * white.w());

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if ((i & gridHeight) ^ (j & gridWidth))
            {
                *data++ = blackub;
            }
            else
            {
                *data++ = whiteub;
            }
        }
    }
    return tex;
}

osgAnimation::Bone* createBone(const std::string& name, const osg::Vec3& translation,
    const osg::Quat& rotation, bool translateFirst)
{
    auto bone = new osgAnimation::Bone(name);
    bone->setMatrix(osg::Matrix::rotate(rotation) * osg::Matrix::translate(translation));

    auto updater = new osgAnimation::UpdateBone(name);
    bone->setUpdateCallback(updater);

    auto& st = updater->getStackedTransforms();

    if (translateFirst)
    {
        st.push_back(new osgAnimation::StackedTranslateElement("Translation", translation));
        st.push_back(new osgAnimation::StackedQuaternionElement("Quat", rotation));
    }
    else
    {
        st.push_back(new osgAnimation::StackedQuaternionElement("Quat", rotation));
        st.push_back(new osgAnimation::StackedTranslateElement("Translation", translation));
    }

    // st.push_back(
    //     new osgAnimation::StackedScaleElement( "Scale", osg::Vec3( 1, 1, 1 ) ) );

    return bone;
}

osgAnimation::QuatKeyframeContainer* createMirrorQuatKeyframes(
    const osgAnimation::QuatKeyframeContainer& ping)
{
    auto pong = new osgAnimation::QuatKeyframeContainer;
    pong->reserve(ping.size());

    std::transform(ping.begin(), ping.end(), std::back_inserter(*pong),
        [](const osgAnimation::QuatKeyframe& f) {
            return osgAnimation::QuatKeyframe(f.getTime(), f.getValue().inverse());
        });

    return pong;
}

class DrawSkeletonVisitor : public osg::NodeVisitor
{
public:
    DrawSkeletonVisitor(float axisSize)
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);

        _axes = new osg::Geode;
        _axes->addDrawable(osgf::createAxes(axisSize));
    }

    void apply(osg::Transform& node) override
    {
        auto bone = dynamic_cast<osgAnimation::Bone*>(&node);
        if (bone)
        {
            bone->addChild(_axes);
            OSG_NOTICE << "Found bone " << node.getName() << std::endl;
        }

        traverse(node);
    }

private:
    osg::ref_ptr<osg::Geode> _axes;
};

void drawSkeleton(osgAnimation::Skeleton* skeleton, float axisSize)
{
    DrawSkeletonVisitor v(axisSize);
    skeleton->accept(v);
}

void addControlPoints(osg::AnimationPath& path, int steps, double time0, double time1,
    const osg::Matrix& m0, const osg::Matrix& m1, bool addStartControlPoint)
{
    osg::Vec3 trans0;
    osg::Quat rot0;
    osg::Vec3 scale0;
    osg::Quat so0;
    m0.decompose(trans0, rot0, scale0, so0);

    osg::Vec3 trans1;
    osg::Quat rot1;
    osg::Vec3 scale1;
    osg::Quat so1;
    m1.decompose(trans1, rot1, scale1, so1);

    for (auto i = (addStartControlPoint ? 0 : 1); i <= steps; ++i)
    {
        auto p = static_cast<float>(i) / steps;
        auto time = detail::mix(time0, time1, p);
        auto trans = detail::mix(trans0, trans1, p);
        auto rot = detail::mix(rot0, rot1, p);
        auto scale = detail::mix(scale0, scale1, p);
        path.insert(time, osg::AnimationPath::ControlPoint(trans, rot, scale));
    }
}

template<typename Element>
void addLinearKeyFrames(osgAnimation::TemplateKeyframeContainer<Element>& container,
    int steps, float time0, float time1, const Element& e0, const Element& e1,
    bool addStartFrame)
{
    for (auto i = (addStartFrame ? 0 : 1); i <= steps; ++i)
    {
        auto p = static_cast<float>(i) / steps;
        auto time = detail::mix(time0, time1, p);
        auto elem = detail::mix(e0, e1, p);
        container.push_back(osgAnimation::TemplateKeyframe<Element>(time, elem));
    }
}

#define INSTANTIATE_addLinearKeyFrames(Element)                                            \
    template void addLinearKeyFrames<Element>(                                             \
        osgAnimation::TemplateKeyframeContainer<Element> & container, int steps,           \
        float time0, float time1, const Element& e0, const Element& e1,                    \
        bool addStartFrame);

INSTANTIATE_addLinearKeyFrames(float);
INSTANTIATE_addLinearKeyFrames(double);
INSTANTIATE_addLinearKeyFrames(osg::Vec2);
INSTANTIATE_addLinearKeyFrames(osg::Vec3);
INSTANTIATE_addLinearKeyFrames(osg::Vec4);
INSTANTIATE_addLinearKeyFrames(osg::Quat);

void addLinearKeyFrames(osgAnimation::QuatKeyframeContainer& container, int steps,
    float time0, float time1, float angle0, float angle1, const osg::Vec3& axis,
    bool addStartFrame)
{
    for (auto i = (addStartFrame ? 0 : 1); i <= steps; ++i)
    {
        auto p = static_cast<float>(i) / steps;
        auto time = detail::mix(time0, time1, p);
        auto angle = detail::mix(angle0, angle1, p);
        container.push_back(osgAnimation::QuatKeyframe(time, osg::Quat(angle, axis)));
    }
}

osgAnimation::QuatSphericalLinearChannel* createQuatChannel(const std::string& name,
    const std::string& targetName, float time0, float time1, const osg::Quat& q0,
    const osg::Quat& q1)
{
    auto channel = new osgAnimation::QuatSphericalLinearChannel;
    channel->setName(name);
    channel->setTargetName(targetName);

    auto frames = channel->getOrCreateSampler()->getOrCreateKeyframeContainer();
    frames->push_back(osgAnimation::QuatKeyframe(time0, q0));
    frames->push_back(osgAnimation::QuatKeyframe(time1, q1));

    return channel;
}

namespace detail
{
class AnimationActionCallbackWrapper : public osgAnimation::Action::Callback
{
public:
    AnimationActionCallbackWrapper(AnimationActionCallbackFunc func) : _func(func) {}

    void operator()(osgAnimation::Action* a, osgAnimation::ActionVisitor* av)
    {
        return _func(a, av);
    }

private:
    AnimationActionCallbackFunc _func;
};
}  // namespace detail

void* createAnimationActionCallback(AnimationActionCallbackFunc func)
{
    return new detail::AnimationActionCallbackWrapper(func);
}

}  // namespace osgf
