#!/bin/bash
OSG_HOME=/usr/local/source/osg3.6_gl2/include/

ctags -R -f - --excmd=number --sort=no --fields=Ks --kinds-c++=sc --language-force=c++ \
    ${OSG_HOME}osg/* \
    ${OSG_HOME}osgAnimation/* \
    ${OSG_HOME}osgDB/* \
    ${OSG_HOME}osgFX/* \
    ${OSG_HOME}osgGA/* \
    ${OSG_HOME}osgManipulator/* \
    ${OSG_HOME}osgParticle/* \
    ${OSG_HOME}osgPresentation/* \
    ${OSG_HOME}osgQt/* \
    ${OSG_HOME}osgShadow/* \
    ${OSG_HOME}osgSim/* \
    ${OSG_HOME}osgTerrain/* \
    ${OSG_HOME}osgText/* \
    ${OSG_HOME}osgUI/* \
    ${OSG_HOME}osgUtil/* \
    ${OSG_HOME}osgViewer/* \
    ${OSG_HOME}osgVolume/* \
    ${OSG_HOME}osgWidget/* \
 | cut -f 1,5 | \
while  read name scope; do

  # skip inner
  if [[ "$scope" =~ class: || "$scope" =~ struct: ]]; then
    continue
  fi

  if [[ "$scope" =~ namespace: ]]; then
    scope=${scope:10}
  else
    continue
  fi

  abbrevation=$(echo "${name//[[:lower:]]/}" | tr '[:upper:]' '[:lower:]' )
  namespaceAbbrevation=o$(echo "${scope//[[:lower:]]/}" | tr '[:upper:]' '[:lower:]' )
  if [[ "$scope" = osgSim ]]; then
    namespaceAbbrevation=osi
  elif [[ "$scope" = osgTerrain ]]; then
    namespaceAbbrevation=ote
  elif [[ "$scope" = osgVolume ]]; then
    namespaceAbbrevation=ovo
  elif [[ "$scope" == osgPresentation ]]; then
    namespaceAbbrevation=opr
  fi

  echo ':iab <buffer>' "$namespaceAbbrevation$abbrevation"  "$scope"::"$name"
done

# manual
echo ':iab <buffer>' oaa     osgAnimation::Animation
echo ':iab <buffer>' oadcbc  osgAnimation::DoubleCubicBezierChannel
echo ':iab <buffer>' oadlc   osgAnimation::DoubleLinearChannel
echo ':iab <buffer>' oadsc   osgAnimation::DoubleStepChannel
echo ':iab <buffer>' oafcbc  osgAnimation::FloatCubicBezierChannel
echo ':iab <buffer>' oaflc   osgAnimation::FloatLinearChannel
echo ':iab <buffer>' oafsc   osgAnimation::FloatStepChannel
echo ':iab <buffer>' oaibf   osgAnimation::InBounceFunction
echo ':iab <buffer>' oaibm   osgAnimation::InBackMotion
echo ':iab <buffer>' oaibm   osgAnimation::InBounceMotion
echo ':iab <buffer>' oaicm   osgAnimation::InCircMotion
echo ':iab <buffer>' oaicm   osgAnimation::InCubicMotion
echo ':iab <buffer>' oaief   osgAnimation::InExpoFunction
echo ':iab <buffer>' oaiem   osgAnimation::InElasticMotion
echo ':iab <buffer>' oaiem   osgAnimation::InExpoMotion
echo ':iab <buffer>' oaiobf  osgAnimation::InOutBounceFunction
echo ':iab <buffer>' oaiobm  osgAnimation::InOutBackMotion
echo ':iab <buffer>' oaiobm  osgAnimation::InOutBounceMotion
echo ':iab <buffer>' oaiocf  osgAnimation::InOutCubicFunction
echo ':iab <buffer>' oaiocm  osgAnimation::InOutCircMotion
echo ':iab <buffer>' oaiocm  osgAnimation::InOutCubicMotion
echo ':iab <buffer>' oaioef  osgAnimation::InOutExpoFunction
echo ':iab <buffer>' oaioem  osgAnimation::InOutElasticMotion
echo ':iab <buffer>' oaioem  osgAnimation::InOutExpoMotion
echo ':iab <buffer>' oaioqf  osgAnimation::InOutQuartFunction
echo ':iab <buffer>' oaioqm  osgAnimation::InOutQuadMotion
echo ':iab <buffer>' oaioqm  osgAnimation::InOutQuartMotion
echo ':iab <buffer>' oaiosm  osgAnimation::InOutSineMotion
echo ':iab <buffer>' oaiqm   osgAnimation::InQuadMotion
echo ':iab <buffer>' oaiqm   osgAnimation::InQuartMotion
echo ':iab <buffer>' oaism   osgAnimation::InSineMotion
echo ':iab <buffer>' oamlc   osgAnimation::MatrixLinearChannel
echo ':iab <buffer>' oaobm   osgAnimation::OutBackMotion
echo ':iab <buffer>' oaobm   osgAnimation::OutBounceMotion
echo ':iab <buffer>' oaocm   osgAnimation::OutCircMotion
echo ':iab <buffer>' oaocm   osgAnimation::OutCubicMotion
echo ':iab <buffer>' oaoef   osgAnimation::OutExpoFunction
echo ':iab <buffer>' oaoem   osgAnimation::OutElasticMotion
echo ':iab <buffer>' oaoem   osgAnimation::OutExpoMotion
echo ':iab <buffer>' oaoqf   osgAnimation::OutQuartFunction
echo ':iab <buffer>' oaoqm   osgAnimation::OutQuadMotion
echo ':iab <buffer>' oaoqm   osgAnimation::OutQuartMotion
echo ':iab <buffer>' oaosm   osgAnimation::OutSineMotion
echo ':iab <buffer>' oap     osg::AnimationPath
echo ':iab <buffer>' oap     osg::ArgumentParser
echo ':iab <buffer>' oaqsc   osgAnimation::QuatStepChannel
echo ':iab <buffer>' oaqslc  osgAnimation::QuatSphericalLinearChannel
echo ':iab <buffer>' oas     osg::AudioStream
echo ':iab <buffer>' oaste   osgAnimation::StackedTranslateElement
echo ':iab <buffer>' oat     osg::AutoTransform
echo ':iab <buffer>' oav2cbc osgAnimation::Vec2CubicBezierChannel
echo ':iab <buffer>' oav2lc  osgAnimation::Vec2LinearChannel
echo ':iab <buffer>' oav2sc  osgAnimation::Vec2StepChannel
echo ':iab <buffer>' oav3cbc osgAnimation::Vec3CubicBezierChannel
echo ':iab <buffer>' oav3lc  osgAnimation::Vec3LinearChannel
echo ':iab <buffer>' oav3sc  osgAnimation::Vec3StepChannel
echo ':iab <buffer>' oav4cbc osgAnimation::Vec4CubicBezierChannel
echo ':iab <buffer>' oav4lc  osgAnimation::Vec4LinearChannel
echo ':iab <buffer>' oav4sc  osgAnimation::Vec4StepChannel
echo ':iab <buffer>' obb     osg::BoundingBox
echo ':iab <buffer>' obd     osg::Billboard
echo ':iab <buffer>' obe     osg::BlendEquationi
echo ':iab <buffer>' obf     osg::BlendFunc
echo ':iab <buffer>' obo     osg::BufferObject
echo ':iab <buffer>' ob      osg::Box
echo ':iab <buffer>' obs     osg::BoundingSphere
echo ':iab <buffer>' occ     osg::ClipControl
echo ':iab <buffer>' ocm     osg::ColorMatrix
echo ':iab <buffer>' ocn     osg::ClipNode
echo ':iab <buffer>' oco     osg::CopyOp
echo ':iab <buffer>' oc      osg::Camera
echo ':iab <buffer>' ocs     osg::CullSettings
echo ':iab <buffer>' odbb64  osgDB::Base64encoder
echo ':iab <buffer>' odbdr   osgDB::DatabaseRevisions
echo ':iab <buffer>' odbip   osgDB::ImageProcessor
echo ':iab <buffer>' odbis   osgDB::InputStream
echo ':iab <buffer>' odbms   osgDB::MatrixSerializer
echo ':iab <buffer>' odbo    osgDB::Output
echo ':iab <buffer>' odb     osgDB::
echo ':iab <buffer>' odbos   osgDB::OutputStream
echo ':iab <buffer>' od      osg::Drawable
echo ':iab <buffer>' oet     osgPresentation::Timeout
echo ':iab <buffer>' ogce    osg::GraphicsCostEstimator
echo ':iab <buffer>' oge     osg::Geode
echo ':iab <buffer>' ogm     osg::Geometry
echo ':iab <buffer>' ogsm    osgGA::StandardManipulator
echo ':iab <buffer>' ogtm    osgGA::TrackballManipulator
echo ':iab <buffer>' oies    osgSim::ElevationSlice
echo ':iab <buffer>' ois     osgSim::Sector
echo ':iab <buffer>' ols     osg::LineSegment
echo ':iab <buffer>' oma     osg::Material
echo ':iab <buffer>' om      osg::Matrix
echo ':iab <buffer>' omt     osg::MatrixTransform
echo ':iab <buffer>' omtpd   osgManipulator::TranslatePlaneDragger
echo ':iab <buffer>' ool     osgVolume::Layer
echo ':iab <buffer>' oo      osg::Object
echo ':iab <buffer>' o_      osg::ref_ptr
echo ':iab <buffer>' oovs    osgVolume::VolumeSettings
echo ':iab <buffer>' oovt    osgVolume::VolumeTile
echo ':iab <buffer>' opbo    osg::PixelBufferObject
echo ':iab <buffer>' opcp    osgParticle::CompositePlacer
echo ':iab <buffer>' opdo    osgParticle::DomainOperator
echo ':iab <buffer>' opo     osg::PolygonOffset
echo ':iab <buffer>' op      osg::Program
echo ':iab <buffer>' oppe    osgParticle::PrecipitationEffect
echo ':iab <buffer>' oppp    osgParticle::PointPlacer
echo ':iab <buffer>' ops     osg::PointSprite
echo ':iab <buffer>' orb     osg::RenderBuffer
echo ':iab <buffer>' orcl    osgTerrain::ContourLayer
echo ':iab <buffer>' orl     osgTerrain::Locator
echo ':iab <buffer>' orm     osg::RefMatrixf
echo ':iab <buffer>' ortt    osgTerrain::TerrainTile
echo ':iab <buffer>' osa     osg::StateAttribute
echo ':iab <buffer>' osc     osg::ShaderComposer
echo ':iab <buffer>' osm     osg::ShadeModel
echo ':iab <buffer>' os      osg::Shader
echo ':iab <buffer>' osssm   osgShadow::StandardShadowMap
echo ':iab <buffer>' osss    osgShadow::ShadowSettings
echo ':iab <buffer>' osst    osgShadow::ShadowTexture
echo ':iab <buffer>' otb     osg::TextureBuffer
echo ':iab <buffer>' otf     osg::TriangleFunctor
echo ':iab <buffer>' otg     osg::TexGen
echo ':iab <buffer>' otm     osg::TexMat
echo ':iab <buffer>' ot      osg::Texture
echo ':iab <buffer>' otr     osg::TextureRectangle
echo ':iab <buffer>' oucv    osgUtil::CullVisitor
echo ':iab <buffer>' oudc    osgUtil::DelaunayConstraint
echo ':iab <buffer>' ouiv    osgUtil::IntersectVisitor
echo ':iab <buffer>' oupi    osgUtil::PolytopeIntersector
echo ':iab <buffer>' oupv    osgUtil::PrintVisitor
echo ':iab <buffer>' ous     osgUtil::Statistics
echo ':iab <buffer>' ous     osgUtil::Statistics
echo ':iab <buffer>' ousv    osgUtil::StatsVisitor
echo ':iab <buffer>' ov2a    osg::Vec2Array
echo ':iab <buffer>' ov2     osg::Vec2
echo ':iab <buffer>' ov3a    osg::Vec3Array
echo ':iab <buffer>' ov3     osg::Vec3
echo ':iab <buffer>' ov4a    osg::Vec4Array
echo ':iab <buffer>' ov4     osg::Vec4
echo ':iab <buffer>' ov      osg::Viewport
echo ':iab <buffer>' ovv     osgViewer::Viewer
echo ':iab <buffer>' owb     osgWidget::Browser
echo ':iab <buffer>' owc     osgWidget::Canvas
echo ':iab <buffer>' ownw    osgWidget::NullWidget
echo ':iab <buffer>' oww     osgWidget::Window
