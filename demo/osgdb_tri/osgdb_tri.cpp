#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgUtil/SmoothingVisitor>

// reader writer of *.tri
class ReaderWriterTRI : public osgDB::ReaderWriter {
public:
  ReaderWriterTRI() { supportsExtension("tri", "Triangle strip points"); }

  virtual ReadResult readNode(const std::string& file,
    const osgDB::ReaderWriter::Options* options) const {
    std::string ext = osgDB::getLowerCaseFileExtension(file);

    if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

    std::string fileName = osgDB::findDataFile(file, options);

    if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

    std::ifstream stream(fileName.c_str(), std::ios::in);
    if (!stream) return ReadResult::ERROR_IN_READING_FILE;

    return readNode(stream, options);
  }

  virtual ReadResult readNode(
    std::istream& stream, const osgDB::ReaderWriter::Options*) const {

    unsigned int index = 0, numPoints = 0;
    stream >> numPoints;

    osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array;

    //read vertices
    while (index < numPoints && !stream.eof() &&
           !(stream.rdstate() & std::istream::failbit)) {
      osg::Vec3 point;
      stream >> point.x() >> point.y() >> point.z();
      va->push_back(point);
      index++;
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray(va.get());
    geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, numPoints));

    //generate normal
    osgUtil::SmoothingVisitor smoother;
    smoother.smooth(*geom);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());

    return geode.release();
  }
};

// must be done for every osg plugin at the end of source file
REGISTER_OSGPLUGIN(tri, ReaderWriterTRI)
