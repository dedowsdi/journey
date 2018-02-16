#include <iostream>
#include <osg/Matrix>

std::ostream& operator<<(std::ostream& os, const osg::Matrix& m) {
  os.precision(2);
  os << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << " " << m(0, 3) << std::endl;
  os << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << " " << m(1, 3) << std::endl;
  os << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << " " << m(2, 3) << std::endl;
  os << m(3, 0) << " " << m(3, 1) << " " << m(3, 2) << " " << m(3, 3) << std::endl;
  return os;
}

int main(int argc, char* argv[]) {
  osg::Matrix m;
  m = osg::Matrix::scale(osg::Vec3f(0.5, 0.5, 0.5));
  m.postMultTranslate(osg::Vec3f(1, 2, 3));

  std::cout << m << std::endl;

  return 0;
}
