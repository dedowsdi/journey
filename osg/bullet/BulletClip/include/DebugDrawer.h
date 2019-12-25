#ifndef BULLETCLIP_DEBUGDRAWER_H
#define BULLETCLIP_DEBUGDRAWER_H

#include <vector>

#include <LinearMath/btIDebugDraw.h>
#include <osg/Group>
#include <osg/Geometry>
#include <osgText/Font>

class DebugDrawer : public osg::Group, public btIDebugDraw
{
public:

    DebugDrawer();

    void drawLine( const btVector3& from, const btVector3& to,
        const btVector3& fromColor, const btVector3& toColor ) override;

    void drawLine( const btVector3& from, const btVector3& to,
        const btVector3& color ) override;

    void drawContactPoint( const btVector3& PointOnB,
        const btVector3& normalOnB, btScalar distance, int lifeTime,
        const btVector3& color ) override;

    void reportErrorWarning( const char* warningString ) override;

    void draw3dText(
        const btVector3& location, const char* textString ) override;

    void begin();

    void end();

    void setDebugMode( int debugMode ) override { _debugMode = debugMode; }
    int getDebugMode() const override { return _debugMode; }

    int getCharacterSize() const { return _characterSize; }
    void setCharacterSize(int v){ _characterSize = v; }

    osgText::Font* getFont() const { return _font.get(); }
    void setFont(osgText::Font* v){ _font = v; }

    bool getDrawCotactLifeTime() const { return _drawCotactLifeTime; }
    void setDrawCotactLifeTime(bool v){ _drawCotactLifeTime = v; }

    bool getDrawContactNormal() const { return _drawContactNormal; }
    void setDrawContactNormal(bool v){ _drawContactNormal = v; }

    int getTextPoolSize() const { return _textPoolSize; }
    void setTextPoolSize(int v){ _textPoolSize = v; }

private:

    osgText::Text* createText();

    bool _drawCotactLifeTime = false;
    bool _drawContactNormal = false;
    int _debugMode = -1;
    int _characterSize = 16;
    int _textPoolSize = 32;
    int _numTexts = 0;

    osg::ref_ptr<osgText::Font> _font;
    osg::ref_ptr<osg::Geode> _leaf;
    osg::ref_ptr<osg::Geometry> _points;
    osg::ref_ptr<osg::Geometry> _lines;
    osg::ref_ptr<osg::Geometry> _triangles; // not used

    // it's too slow to create n texts in every frame
    std::vector<osg::ref_ptr<osgText::Text>> _textPool;
};

#endif // BULLETCLIP_DEBUGDRAWER_H
