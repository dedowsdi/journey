#include <memory>
#include <osg/Texture2D>
#include <Game.h>
#include <OsgFactory.h>

// adapted from
// https://www.scratchapixel.com/code.php?id=10&origin=/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes
// Ray tracing is image centerd, rasteration is object centerd. Ray tracing  is also called
// backward tracing, the camera(primary) ray starts from camera, it went through pixel, intersect scene
// object, you can also create secondary ray from the intersection point, and use it for
// shadowing (trace light) or other things.
//
// You start this by creating a m*n image, pixels resides in Image Space, scratchpixel
// prefer to use lefttop as 0,0, I use left bottom. Image Space x ranges from 0.5 to 0.5 + m-1.
// Then you convert Image Space to ndc as: x = x / m , it belongs to [0,1], then convert it
// to Screen Space: x = 2 * x - 1, it belongs to [-1, 1], If you Image is not square, you
// need to apply aspect ratio: x = x * aspectRatio. At last convert it to camera space, your
// canvas is always 1 unit away from camera, fovy will affect how many things you will see:
// y = y * tan(halfFovy), x = x * tan(halfFovy)
//
namespace toy
{

// Tracing object
class Object
{
public:
    Object() {}
    virtual ~Object() {}

    // Method to compute the intersection of the object with a ray
    // Returns true if an intersection was found, false otherwise
    // See method implementation in children class for details
    virtual bool intersect(const osg::Vec3&, const osg::Vec3&, float&) const = 0;

    // Method to compute the surface data such as normal and texture coordnates at the
    // intersection point. See method implementation in children class for details
    virtual void getSurfaceData(
        const osg::Vec3&, osg::Vec3& normal, osg::Vec2& texcoord) const = 0;

    osg::Vec3 color;
};

osg::Vec3 castRay(const osg::Vec3& orig, const osg::Vec3& dir,
    const std::vector<std::unique_ptr<Object>>& objects);

// Returns true if the ray intersects an object. The variable tNear is set to the closest
// intersection distance and hitObject is a pointer to the intersected object. The variable
// tNear is set to infinity and hitObject is set null if no intersection was found.
bool trace(const osg::Vec3& orig, const osg::Vec3& dir,
    const std::vector<std::unique_ptr<Object>>& objects, float& tNear,
    const Object*& hitObject);

class Sphere : public Object
{
public:
    Sphere(const osg::Vec3& center, float radius)
        : _center(center), _radius(radius), _radius2(_radius * _radius)
    {
    }

    bool intersect(const osg::Vec3& orig, const osg::Vec3& dir, float& t) const;

    void getSurfaceData(const osg::Vec3& Phit, osg::Vec3& Nhit, osg::Vec2& tex) const;

    osg::Vec3 _center;
    float _radius;
    float _radius2;
};

bool Sphere::intersect(const osg::Vec3& orig, const osg::Vec3& dir, float& t) const
{
    float t0, t1;  // solutions for t if the ray intersects
#if 0 
        // geometric solution
        osg::Vec3 L = _center - orig; 
        float tca = L.dotProduct(dir); 
        if (tca < 0) return false; 
        float d2 = L.dotProduct(L) - tca * tca; 
        if (d2 > radius2) return false; 
        float thc = sqrt(radius2 - d2); 
        t0 = tca - thc; 
        t1 = tca + thc;
#else
    // analytic solution
    osg::Vec3 L = orig - _center;
    float a = dir.length2();
    float b = dir * L * 2;
    float c = L.length2() - _radius2;
    if (!solveQuadratic(a, b, c, t0, t1))
        return false;
#endif
    if (t0 > t1)
        std::swap(t0, t1);

    if (t0 < 0)
    {
        t0 = t1;  // if t0 is negative, let's use t1 instead
        if (t0 < 0)
            return false;  // both t0 and t1 are negative
    }

    t = t0;

    return true;
}

void Sphere::getSurfaceData(const osg::Vec3& Phit, osg::Vec3& Nhit, osg::Vec2& tex) const
{
    Nhit = Phit - _center;
    Nhit.normalize();
    // In this particular case, the normal is simular to a point on a unit sphere
    // centred around the origin. We can thus use the normal coordinates to compute
    // the spherical coordinates of Phit.
    // atan2 returns a value in the range [-pi, pi] and we need to remap it to range [0, 1]
    // acosf returns a value in the range [0, pi] and we also need to remap it to the range
    // [0, 1]
    tex.x() = (1 + atan2(Nhit.y(), Nhit.x()) / M_PI) * 0.5;
    tex.y() = acosf(Nhit.z()) / M_PI;
}

osg::Vec3 castRay(const osg::Vec3& orig, const osg::Vec3& dir,
    const std::vector<std::unique_ptr<Object>>& objects)
{
    osg::Vec3 hitColor;
    const Object* hitObject = nullptr;  // this is a pointer to the hit object
    float t;  // this is the intersection distance from the ray origin to the hit point
    if (trace(orig, dir, objects, t, hitObject))
    {
        osg::Vec3 Phit = orig + dir * t;
        osg::Vec3 Nhit;
        osg::Vec2 tex;
        hitObject->getSurfaceData(Phit, Nhit, tex);
        // Use the normal and texture coordinates to shade the hit point.
        // The normal is used to compute a simple facing ratio and the texture coordinate
        // to compute a basic checker board pattern
        float scale = 4;
        float pattern =
            (fmodf(tex.x() * scale, 1) > 0.5) ^ (fmodf(tex.y() * scale, 1) > 0.5);
        hitColor = mix(hitObject->color, hitObject->color * 0.4f, pattern) *
                   std::max(0.f, Nhit * -dir);
    }

    return hitColor;
}

bool trace(const osg::Vec3& orig, const osg::Vec3& dir,
    const std::vector<std::unique_ptr<Object>>& objects, float& tNear,
    const Object*& hitObject)
{
    tNear = FLT_MAX;
    std::vector<std::unique_ptr<Object>>::const_iterator iter = objects.begin();
    for (; iter != objects.end(); ++iter)
    {
        float t = FLT_MAX;
        if ((*iter)->intersect(orig, dir, t) && t < tNear)
        {
            hitObject = iter->get();
            tNear = t;
        }
    }

    return (hitObject != nullptr);
}

}  // namespace toy

int main(int argc, char* argv[])
{
    toy::Game game;
    game.init(argc, argv);

    auto sceneRoot = game.getSceneRoot();
    // There has no scene drawable, must set initial bound for camera manipulator
    sceneRoot->setInitialBound(osg::BoundingSphere(osg::Vec3(), 10));

    osg::ArgumentParser args(&argc, argv);
    int width = 1280;
    int height = 720;
    int numSpheres = 8;
    float fovy = 90;
    args.read("--width", width);
    args.read("--height", height);
    args.read("--numSpheres", numSpheres);
    args.read("--fovy", fovy);

    // create width*height image
    auto img = new osg::Image;
    img->allocateImage(width, height, 1, GL_RGB, GL_FLOAT);

    // create objects
    std::vector<std::unique_ptr<toy::Object>> objects;
    for (auto i = 0; i < numSpheres; ++i)
    {
        auto pos = toy::linearRand(osg::Vec3(-8, -8, -8), osg::Vec3(8, 8, 8));
        auto sphere = std::make_unique<toy::Sphere>(pos, 2);
        sphere->color = toy::linearRand(osg::Vec3(), osg::Vec3(1, 1, 1));
        objects.push_back(move(sphere));
    }

    // cast rays from main camera in wordspace
    auto imageAspectRatio =
        static_cast<float>(width) / height;  // it's greater than 1 most of time
    auto camera = game.getMainCamera();
    auto tanHalfFovy = tan(osg::DegreesToRadians(fovy * 0.5f));
    auto traceCallback = osgf::createCallback([=, &objects](osg::Object* obj, osg::Object* data) {
        auto viewMatrixInverse = camera->getInverseViewMatrix();
        auto orig = viewMatrixInverse.getTrans();
        auto cv = data->asNodeVisitor()->asCullVisitor();

        // loop pixels
        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                // assume left bottom as 0,0 in in image space (scratchpixel use
                // left top), image space x belongs to [0, width - 1]
                auto x =
                    (i + 0.5) / width;  // normalize into ndc space, it belongs to [0, 1]
                x = x * 2 - 1;          // remap it to [-1, 1], it's called Screen Space
                x *= imageAspectRatio;  // make sure pixel is square, this will make x
                                        // greater than 1. Maya choose to apply it to y
                                        // instead, which make y smaller than 1.
                x *= tanHalfFovy;  // convert to camera space, note canvas is 1 unit away
                                   // from camera, fovy affect how much things we see.
                auto y = (j + 0.5) / height;
                y = y * 2 - 1;
                y *= tanHalfFovy;

                // get dir in world space
                auto dir = osg::Vec3(x, y, -1);
                dir = osg::Matrix::transform3x3(dir, viewMatrixInverse);
                dir.normalize();

                // if (i == width * 0.5 && j == height * 0.5 &&
                //     cv->getFrameStamp()->getFrameNumber() % 120 == 0)
                // {
                //     OSG_NOTICE << orig << "\t" << dir << std::endl;
                // }

                auto pixel = reinterpret_cast<osg::Vec3*>(img->data(i, j));
                *(pixel) = castRay(orig, dir, objects);
            }
        }
        img->dirty();
    });
    camera->setCullCallback(traceCallback);

    // draw fullscreen image
    auto hudCamera = game.getHudCamera();
    hudCamera->setClearMask(GL_COLOR_BUFFER_BIT);

    auto quad = osg::createTexturedQuadGeometry(
        osg::Vec3(), osg::Vec3(1280, 0, 0), osg::Vec3(0, 720, 0));
    auto ss = quad->getOrCreateStateSet();
    auto tex = new osg::Texture2D(img);
    tex->setResizeNonPowerOfTwoHint(false);
    ss->setTextureAttributeAndModes(0, tex);

    hudCamera->addChild(quad);

    return game.run();
}
