#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/Image>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <memory>

osg::Camera* createOrthoCamera(double left, double right, double bottom,
    double top, double near = -1, double far = 1)
{
    auto camera = new osg::Camera;
    auto proj = osg::Matrix::ortho(left, right, bottom, top, near, far);
    camera->setProjectionMatrix(proj);
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setClearMask(GL_COLOR_BUFFER_BIT);

    auto ss = camera->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    return camera;
}

namespace toy
{

// Tracing object
class Object
{
public:
    // virtual intersect function, needs to be overloaded by derived class
    virtual bool intersect(
        const osg::Vec3& orig, const osg::Vec3& dir, float& t) const = 0;
    virtual ~Object() {} // virtual destructor
    Object() {}          // constructor
};

bool solveQuadratic(
    const float& a, const float& b, const float& c, float& x0, float& x1);

class Sphere : public Object
{
public:

    Sphere(const osg::Vec3& center, float radius):
        _center(center),
        _radius(radius),
        _radius2(_radius * _radius)
    {
    }

    bool intersect(const osg::Vec3& orig, const osg::Vec3& dir, float& t) const;

    osg::Vec3 _center;
    float _radius;
    float _radius2;
};

// result x0 < x1
bool solveQuadratic(
    const float& a, const float& b, const float& c, float& x0, float& x1)
{
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;

    if (discriminant == 0) x0 = x1 = -0.5 * b / a;

    // avoid catastrophic cancellation
    float q = (b > 0) ? -0.5 * (b + sqrt(discriminant))
                      : -0.5 * (b - sqrt(discriminant));
    x0 = q / a;
    x1 = c / q; // x0*x1 = c / a
    if (x0 > x1) std::swap(x0, x1);

    return true;
}

bool Sphere::intersect(
    const osg::Vec3& orig, const osg::Vec3& dir, float& t) const
{
    float t0, t1; // solutions for t if the ray intersects
#if 0 
        // geometric solution
         osg::Vec3 L = _center - orig; 
         float tca = L.dotProduct(dir); 
         if (tca < 0) return false; 
         float d2 = L.dotProduct(L) - tca * tca; 
         if (d2 > _radius2) return false; 
         float thc = sqrt(_radius2 - d2); 
         t0 = tca - thc; 
         t1 = tca + thc;
#else
    // analytic solution
    osg::Vec3 L = orig - _center;
    float a = dir.length2();
    float b = dir * L * 2.0f;
    float c = L.length2() - _radius2;
    if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0)
    {
        t0 = t1;                  // if t0 is negative, let's use t1 instead
        if (t0 < 0) return false; // both t0 and t1 are negative
    }

    t = t0;

    return true;
}

} // namespace toy

// ray tracing a sphere in cpu

int main(int argc, char* argv[])
{
    auto root = new osg::Group;

    osg::ArgumentParser args(&argc, argv);
    int width = 128;
    int height = 128;
    int numSpheres = 32;
    args.read("--width", width);
    args.read("--height", height);
    args.read("--numSpheres", numSpheres);

    // create m*n image
    auto img = new osg::Image;
    img->allocateImage(width, height, 1, GL_RGBA, GL_FLOAT);

    // create spheres, this example doesn't require ray triangle test, so only
    // position and radius matter
    std::vector<std::unique_ptr<toy::Sphere>> spheres;
    for (auto i = 0; i < numSpheres; ++i)
    {

    }

    // cast rays

    // do ray sphere test

    // draw fullscreen image
    auto hudCamera = createOrthoCamera(0, 1280, 0, 720);
    root->addChild(hudCamera);

    auto quad = osg::createTexturedQuadGeometry(
        osg::Vec3(), osg::Vec3(1280, 0, 0), osg::Vec3(0, 720, 0));
    auto ss = quad->getOrCreateStateSet();
    auto tex = new osg::Texture2D(img);
    ss->setTextureAttributeAndModes(0, tex);

    hudCamera->addChild(quad);

    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setSceneData(root);

    return viewer.run();
}
