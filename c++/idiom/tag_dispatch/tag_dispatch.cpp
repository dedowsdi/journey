#include <iostream>

struct as_radius{};
struct as_diameter{};

struct radius_type
{
  float value;
};

struct diameter_type
{
  float value;
};

class circle
{
protected:
  float m_radius;

public:

  // you have to constructor with the same type parameter, you need a sed "tag"
  // to help you distinguish them. This is just a demonstration, it's not a good
  // example, you should wrap radius to Radius, diameter to Diameter instead.
  circle(float radius, as_radius):
    m_radius(radius)
  {
  }

  circle(float diameter, as_diameter):
    m_radius(diameter * 0.5f)
  {
  }

  circle(radius_type r):
    m_radius(r.value)
  {
  }

  circle(diameter_type d):
    m_radius(d.value * 0.5f)
  {
  }
};

int main(int argc, char *argv[])
{
  // all the same, 3 4 is prefered
  circle c0(0.5f, as_radius());
  circle c1(1.0f, as_diameter());
  circle c2(radius_type{0.5f});
  circle c3(diameter_type{1.0f});
  
  return 0;
}
