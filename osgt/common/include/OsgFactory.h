#ifndef BOUNCINGBALL_OSGFACTORY_H
#define BOUNCINGBALL_OSGFACTORY_H

// Create util for osg, don't include large head files.
#include <functional>
#include <string>
#include <vector>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

// forward declare {{{1
namespace osg
{
class Geometry;
class Geode;
class Node;
class Program;
class Camera;
class Texture2D;
class StateSet;
class FrameBufferObject;
class Drawable;
class RenderInfo;
class Quat;
class Object;
class Callback;
class Vec3d;
class MatrixTransform;
class AnimationPath;
class Shader;
class NodeVisitor;

template<class T>
class ref_ptr;

class Matrixf;
class Matrixd;
#ifdef OSG_USE_FLOAT_MATRIX
typedef Matrixf Matrix;
#else
typedef Matrixd Matrix;
#endif

template<typename VT>
class BoundingBoxImpl;

typedef BoundingBoxImpl<Vec3f> BoundingBoxf;
typedef BoundingBoxImpl<Vec3d> BoundingBoxd;

#ifdef OSG_USE_FLOAT_BOUNDINGBOX
typedef BoundingBoxf BoundingBox;
#else
typedef BoundingBoxd BoundingBox;
#endif

template<typename VT>
class BoundingSphereImpl;

typedef BoundingSphereImpl<Vec3f> BoundingSpheref;
typedef BoundingSphereImpl<Vec3d> BoundingSphered;

#ifdef OSG_USE_FLOAT_BOUNDINGSPHERE
typedef BoundingSpheref BoundingSphere;
#else
typedef BoundingSphered BoundingSphere;
#endif

}  // namespace osg

namespace osgAnimation
{
class Skeleton;
class Bone;
class Action;
class ActionVisitor;

template<typename T>
class TemplateKeyframe;

template<typename T>
class TemplateKeyframeContainer;

typedef TemplateKeyframe<osg::Quat> QuatKeyframe;
typedef TemplateKeyframeContainer<osg::Quat> QuatKeyframeContainer;

template<class TYPE, class KEY>
class TemplateSphericalLinearInterpolator;

typedef TemplateSphericalLinearInterpolator<osg::Quat, osg::Quat>
    QuatSphericalLinearInterpolator;

template<class F>
class TemplateSampler;

typedef TemplateSampler<QuatSphericalLinearInterpolator> QuatSphericalLinearSampler;

template<typename SamplerType>
class TemplateChannel;

typedef TemplateChannel<QuatSphericalLinearSampler> QuatSphericalLinearChannel;
}  // namespace osgAnimation

namespace osgWidget
{
class Widget;
}

// create*  Create something, caller is in charge of the memory.
// get*     Get something, mostly a function local static.
// add*     Add something.
// set*     Change something.
// draw*    Mostly for debug purpose, add some drawables with a custom visitor.
namespace osgf
{

// Object {{{1

template<typename T, typename... Args>
osg::ref_ptr<T> make_ref(Args... args)
{
    return new T(std::forward<Args>(args)...);
}

// Node {{{1

// culling is disabled for this ndc quad.
osg::Geometry* getNdcQuad();

osg::Geode* createNdcQuadLeaf();

// mostly for debug purpose, all sphere and box have center (0, 0, 0).
osg::MatrixTransform* createSphereAt(
    const osg::Vec3& pos, float radius, const osg::Vec4& color = osg::Vec4(1, 1, 1, 1));
osg::MatrixTransform* createSphere(
    const osg::BoundingSphere& sphere, const osg::Vec4& color = osg::Vec4(1, 1, 1, 1));

osg::MatrixTransform* createBoxAt(const osg::Vec3& pos, float nx, float ny, float nz,
    const osg::Vec4& color = osg::Vec4(1, 1, 1, 1));
osg::MatrixTransform* createBoxAt(
    const osg::Vec3& pos, float width, const osg::Vec4& color = osg::Vec4(1, 1, 1, 1));
osg::MatrixTransform* createBox(
    const osg::BoundingBox& box, const osg::Vec4& color = osg::Vec4(1, 1, 1, 1));

// use GL_LINE polygin mode, no depth test, no blend, no texture. Render at custom bin num
osg::MatrixTransform* createBoundingBoxWireFrame(const osg::BoundingBox& box,
    const osg::Vec4& color = osg::Vec4(1, 1, 1, 1), int binNum = 99);

// Bake transform into geometry.
void bakeTransform(osg::Node& node);

osg::Node* readNodeFile(
    const std::string& name, float desiredRadius, bool shiftToCenter = true);

// You have stacks+1 rows of vertices, each row has slices+1 cols, this function
// create triangle strip elements as adbecf... or daebfc... if reversed:
//  a b c ...
//  d e f ...
//
// This function also works if your grid is created slice by slice, it results in adbecf...
// for following vertices.
//  . .
//  c f
//  b e
//  a d
template<typename T>
void addGridElements(
    osg::Geometry& geom, int stacks, int slices, int offset = 0, bool reverse = false);

// texcoord0 : texcoord of 1st vertex in 1st stack
// texcoord1 : texcoord of last vertex in last stack
template<typename T>
void addGridTexcoords(T& texcoords, int stacks, int slices,
    const osg::Vec2& texcoord0 = osg::Vec2(0, 1),
    const osg::Vec2& texcoord1 = osg::Vec2(1, 0));

template<typename T>
void addGridTexcoordsSliceBySlice(T& texcoords, int slices, int stacks,
    const osg::Vec2& texcoord0 = osg::Vec2(0, 0),
    const osg::Vec2& texcoord1 = osg::Vec2(1, 1));

// Only used for debug purpose. All axes created by this function share a
// function local static StateSet.
// StateSet:
//      GL_DEPTH_TEST  0
//      GL_LIGHTING 0
//      LineWidth
osg::Geometry* createAxes(float size = 1.0f, float lineWidth = 1.5f);

osg::Geometry* createPoints(const std::vector<osg::Vec3>& points);

osg::Geometry* createLines(const std::vector<osg::Vec3>& lines);

osg::Geometry* createTorus(float innerRadius, float outerRadius, int sides, int rings);

// The 1st point has texcoord (r, (b+t) * 0.5)
osg::Geometry* createCircle(const osg::Vec3& origin, const osg::Vec3& point0,
    const osg::Vec3& normal, int sides = 32, bool createNormal = true,
    bool createTexcoord = true, float l = 0, float b = 0, float r = 1, float t = 1);

osg::Geometry* createCircleLines(const osg::Vec3& origin, const osg::Vec3& point0,
    const osg::Vec3& normal, int sides = 32);

using CallbackFunction = std::function<void(osg::Object*, osg::Object*)>;

osg::Callback* createCallback(CallbackFunction callback);

using StateSetCallbackFunction = std::function<void(osg::StateSet*, osg::NodeVisitor*)>;

void* createStateSetCallback(StateSetCallbackFunction func);

// resize quad created by osg::createTexturedQuadGeometry, assume corner is osg::Vec3(0,0,0)
void addQuadResizeCallback(osg::Geometry* quad, const osg::Vec3& widthDir = osg::X_AXIS,
    const osg::Vec3& heightDir = osg::Y_AXIS);

// execute call back after certain time, once only, removed immediately after that.
osg::Callback* createTimerUpdateCallback(double time, CallbackFunction callback);

osg::Callback* getPruneCallback();

osg::Callback* createTimerRemoveNodeUpdateCallback(double time, osg::Node* node);

using ComputeBoundingBoxCallbackFunction =
    std::function<osg::BoundingBox(const osg::Drawable&)>;
void* createComputeBoundingBoxCallback(ComputeBoundingBoxCallbackFunction func);

void* getInvalidComputeBoundingBoxCallback();

void* getInvalidComputeBoundingSphereCallback();

void addConstantComputeBoundingBoxCallback(osg::Drawable& d, const osg::BoundingBox& bb);

using ComputeBoundingSphereCallbackFunction =
    std::function<osg::BoundingSphere(const osg::Node&)>;

void* createComputeBoundingSphereCallback(ComputeBoundingSphereCallbackFunction func);

void addConstantComputeBoundingSphereCallback(osg::Node& d, const osg::BoundingSphere& bb);

using CameraDrawCallbackFunction = std::function<void(osg::RenderInfo&)>;
// func:  void(osg::RenderInfo&);
void* createCameraDrawCallback(CameraDrawCallbackFunction func);

// Program {{{1

osg::Program* createProgram(const std::string& fragFile);

osg::Program* createProgram(const std::string& vertFile, const std::string& fragFile);

osg::Program* createProgram(const std::string& vertFile, const std::string& geomFile,
    const std::string& fragFile, int inputType, int outputType, int maxVertices);

// use file is shader source file is empty
void reloadShader(osg::Shader& shader, const std::string& file = "");

// Camera {{{1

// 0 clear mask.
osg::Camera* createPrerenderCamera(int x, int y, int width, int height);

// 0 clear mask, pre render.
osg::Camera* createRttCamera(int x, int y, int width, int height, int implementation);

// 0 clear mask, pre render, framebufferobject, no depth test.
osg::Camera* createFilterCamera(int x, int y, int width, int height);

// 0 clear mask, absolute, identity view.
osg::Camera* createOrthoCamera(
    double left, double right, double bottom, double top, double near = -1, double far = 1);

// Fbo {{{1

// tex0 must not be empty
osg::FrameBufferObject* addFboRtt(osg::StateSet& ss, osg::Texture2D* tex0,
    osg::Texture2D* tex1 = 0, osg::Texture2D* tex2 = 0);

void addLineSmooth(osg::StateSet& ss, float lineWidth);

using DrawableDrawFunc = std::function<void(osg::RenderInfo&, const osg::Drawable*)>;
osg::Drawable* createDrawable(DrawableDrawFunc func);

osg::Drawable* createClearDrawable(
    int mask, const osg::Vec4& clearColor = osg::Vec4(0, 0, 0, 1), float depth = 1.0f);

// Texture {{{1

osg::Texture2D* createTexture2D(int internalFormat, int width, int height, int minFilter,
    int magFilter, int wrapS, int wrapT);

// tex is not stored. Use texture width height as copy width height if they are
// 0.
osg::Drawable* createCopyTex2DDrawable(osg::Texture2D* tex, int xoffset = 0,
    int yoffset = 0, int x = 0, int y = 0, int width = 0, int height = 0);

osg::Texture2D* createChessTexture(int width, int height, int gridWidth, int gridHeight,
    const osg::Vec4& black = osg::Vec4(0, 0, 0, 1),
    const osg::Vec4& white = osg::Vec4(1, 1, 1, 1));

// Rig {{{1

// create bone, setup initial stacked translation and rotation, no scale.
osgAnimation::Bone* createBone(const std::string& name, const osg::Vec3& translation,
    const osg::Quat& rotation, bool translateFirst = true);

// 0 0.25pi 0.5pi -> 0 -0.25pi -0.5pi
osgAnimation::QuatKeyframeContainer* createMirrorQuatKeyframes(
    const osgAnimation::QuatKeyframeContainer& ping);

void drawSkeleton(osgAnimation::Skeleton* skeleton, float axisSize);

// Animation {{{1

void addControlPoints(osg::AnimationPath& path, int steps, double time0, double time1,
    const osg::Matrix& m0, const osg::Matrix& m1, bool addStartControlPoint = 1);

template<typename Element>
void addLinearKeyFrames(osgAnimation::TemplateKeyframeContainer<Element>& container,
    int steps, float time0, float time1, const Element& e0, const Element& e1,
    bool addStartFrame = 1);

// Use this function if you want to slerp between quat with angles no lesser
// than 2pi.
void addLinearKeyFrames(osgAnimation::QuatKeyframeContainer& container, int steps,
    float time0, float time1, float angle0, float angle1, const osg::Vec3& axis,
    bool addStartFrame = 1);

// This is the most simple case: you have only 2 frames, each frame has it's own rotation.
osgAnimation::QuatSphericalLinearChannel* createQuatChannel(const std::string& name,
    const std::string& targetName, float time0, float time1, const osg::Quat& q0,
    const osg::Quat& q1);

using AnimationActionCallbackFunc =
    std::function<void(osgAnimation::Action*, osgAnimation::ActionVisitor*)>;
void* createAnimationActionCallback(AnimationActionCallbackFunc func);

}  // namespace osgf

// vim:set foldmethod=marker:
#endif  // BOUNCINGBALL_OSGFACTORY_H
