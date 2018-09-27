#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/SingleWindow>
#include <stack>
#include <iostream>
#include "glm.h"

#define WIDTH 800.0f
#define HEIGHT 800.0f

#define NUM_ROWS 30
#define NUM_COLS 30
#define CELL_WIDTH WIDTH/NUM_COLS
#define CELL_HEIGHT HEIGHT/NUM_ROWS
#define HALF_CELL_WIDTH CELL_WIDTH * 0.5
#define HALF_CELL_HEIGHT CELL_HEIGHT * 0.5
#define CELL_COLOR osg::Vec4(0,0,0,1)
#define PATH_COLOR osg::Vec4(0.5,0,1,1)
#define LINE_COLOR osg::Vec4(1,1,1,1)
#define HEAD_COLOR osg::Vec4(1,0,0,1)
#define STEP_TIME 0.02

struct Cell;
class Maze;
typedef std::vector<Cell> CellVector;
typedef std::stack<Cell*> CellStack;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<Maze> maze;
Cell* currentCell;
CellStack cellStack;

struct Cell
{
public:
  GLuint row;
  GLuint col;
  GLboolean leftWall = GL_TRUE;
  GLboolean rightWall = GL_TRUE;
  GLboolean topWall = GL_TRUE;
  GLboolean bottomWall = GL_TRUE;
  GLboolean visited = GL_FALSE;

  Cell(GLuint r, GLuint c):
    row(r),
    col(c)
  {
  }

};

class Maze : public osg::Geometry
{
public:
  GLuint mNumRow;
  GLuint mNumCol;
  GLuint mNumCellColor;
  GLuint mNumPathColor;
  GLuint mNumHeadColor;
  GLuint mNumLines;

  CellVector mCells;

  Maze(GLuint num_row, GLuint num_col):
    mNumRow(num_row),
    mNumCol(num_col)
  {
    mCells.reserve(mNumRow * mNumCol);
    for (GLuint row = 0; row < mNumRow; ++row) {
      for (GLuint col = 0; col < mNumCol; ++col) {
        mCells.push_back(Cell(row, col));
      }
    }

    osg::ref_ptr<osg::Vec2Array> vertices  = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
    vertices->resize(size() * (8+6));
    this->setVertexArray(vertices);

    osg::ref_ptr<osg::Vec4Array> colors  = new osg::Vec4Array(osg::Array::BIND_PER_PRIMITIVE_SET);
    colors->push_back(HEAD_COLOR);
    colors->push_back(PATH_COLOR);
    colors->push_back(CELL_COLOR);
    colors->push_back(LINE_COLOR);
    this->setColorArray(colors);

    this->setDataVariance(osg::Object::DYNAMIC);
  }

  GLuint index(GLuint row, GLuint col)
  {
    if(row == -1 || col == -1 || row >= mNumRow || col >= mNumCol )
      return -1;
    return row * mNumCol + col;
  }

  GLuint size()
  {
    return mCells.size();
  }

  Cell* getCell(GLuint row, GLuint col)
  {
    return getCell(index(row, col));
  }

  Cell* getCell(GLuint idx)
  {
    return idx == -1 ? 0 : &mCells[idx];
  }

  Cell* getUnvisitedNeighbour(Cell& c)
  {
    Cell* left   = getCell(c.row, c.col - 1);
    Cell* right  = getCell(c.row, c.col + 1);
    Cell* bottom = getCell(c.row - 1, c.col);
    Cell* top    = getCell(c.row + 1, c.col);
    std::vector<Cell*> candidates;
    if(left && !left->visited)
      candidates.push_back(left);
    if(right && !right->visited)
      candidates.push_back(right);
    if(bottom && !bottom->visited)
      candidates.push_back(bottom);
    if(top && !top->visited)
      candidates.push_back(top);

    if(candidates.empty())
      return 0;

    return candidates[glm::linearRand(0.0f, static_cast<GLfloat>(candidates.size()))];
  }

  void remove_wall(Cell& lhs, Cell& rhs)
  {
    if(rhs.col - lhs.col == 1)
      rhs.leftWall = lhs.rightWall = GL_FALSE;
    else if(rhs.col - lhs.col == -1)
      lhs.leftWall = rhs.rightWall = GL_FALSE;
    else if(rhs.row - lhs.row == 1)
      rhs.bottomWall = lhs.topWall = GL_FALSE;
    else if(rhs.row - lhs.row == -1)
      lhs.bottomWall = rhs.topWall = GL_FALSE;
    else
      std::cout << "error, illegal remove wall between  " 
          << lhs.row << ":" << lhs.col << "  and  " << rhs.row << ":" << rhs.col << std::endl;
  }

  void update_buffer()
  {
    mNumCellColor = 0;
    mNumPathColor = 0;
    mNumHeadColor = 0;
    mNumLines = 0;

    osg::Vec2Array* vertices = static_cast<osg::Vec2Array*>(getVertexArray());
    osg::Vec2Array::iterator iter = vertices->begin();

    // add quads
    {
      // place head first
      osg::Vec2 lb = osg::Vec2(CELL_WIDTH * currentCell->col, CELL_HEIGHT * currentCell->row);
      osg::Vec2 rt = lb + osg::Vec2(CELL_WIDTH, CELL_HEIGHT);
      osg::Vec2 lt = lb + osg::Vec2(0, CELL_HEIGHT);
      osg::Vec2 rb = lb + osg::Vec2(CELL_WIDTH, 0);
      *iter++ = lb;
      *iter++ = rt;
      *iter++ = lt;
      *iter++ = lb;
      *iter++ = rb;
      *iter++ = rt;
      ++mNumHeadColor;
    }

    for (size_t i = 0; i < mCells.size(); ++i) 
    {
      const Cell& c = mCells[i];
      osg::Vec2 lb = osg::Vec2(CELL_WIDTH * c.col, CELL_HEIGHT * c.row);
      osg::Vec2 rt = lb + osg::Vec2(CELL_WIDTH, CELL_HEIGHT);
      osg::Vec2 lt = lb + osg::Vec2(0, CELL_HEIGHT);
      osg::Vec2 rb = lb + osg::Vec2(CELL_WIDTH, 0);

      if(&c == currentCell)
        continue;

      if(c.visited)
      {
        ++mNumPathColor;
        *iter++ = lb;
        *iter++ = rt;
        *iter++ = lt;
        *iter++ = lb;
        *iter++ = rb;
        *iter++ = rt;
      }
    }

    for (size_t i = 0; i < mCells.size(); ++i) 
    {
      const Cell& c = mCells[i];
      osg::Vec2 lb = osg::Vec2(CELL_WIDTH * c.col, CELL_HEIGHT * c.row);
      osg::Vec2 rt = lb + osg::Vec2(CELL_WIDTH, CELL_HEIGHT);
      osg::Vec2 lt = lb + osg::Vec2(0, CELL_HEIGHT);
      osg::Vec2 rb = lb + osg::Vec2(CELL_WIDTH, 0);

      if(&c == currentCell)
        continue;

      if(!c.visited)
      {
        ++mNumCellColor;
        *iter++ = lb;
        *iter++ = rt;
        *iter++ = lt;
        *iter++ = lb;
        *iter++ = rb;
        *iter++ = rt;
      }
    }

    if(mNumHeadColor + mNumCellColor + mNumPathColor != size())
      std::cout << "illegal buffer : " << mNumHeadColor << " "
        << mNumPathColor << " " <<  mNumCellColor  << std::endl;

    // add lines
    for (size_t i = 0; i < mCells.size(); ++i) 
    {
      const Cell& c = mCells[i];
      osg::Vec2 lb = osg::Vec2(CELL_WIDTH * c.col, CELL_HEIGHT * c.row);
      osg::Vec2 rt = lb + osg::Vec2(CELL_WIDTH, CELL_HEIGHT);
      osg::Vec2 lt = lb + osg::Vec2(0, CELL_HEIGHT);
      osg::Vec2 rb = lb + osg::Vec2(CELL_WIDTH, 0);

      if(c.leftWall)
      {
        *iter++ = lb;
        *iter++ = lt;
        ++mNumLines;
      }
      if(c.rightWall)
      {
        *iter++ = rb;
        *iter++ = rt;
        ++mNumLines;
      }
      if(c.bottomWall)
      {
        *iter++ = lb;
        *iter++ = rb;
        ++mNumLines;
      }
      if(c.topWall)
      {
        *iter++ = lt;
        *iter++ = rt;
        ++mNumLines;
      }
    }

    removePrimitiveSet(0, getNumPrimitiveSets());
    GLuint next = 0;
    GLuint numVertices = mNumHeadColor * 6;
    addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, next, numVertices));

    next += numVertices;
    numVertices = mNumPathColor * 6;
    addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, next, numVertices));

    next += numVertices;
    numVertices = mNumCellColor * 6;
    addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, next, numVertices));

    next += numVertices;
    numVertices= mNumLines * 2;
    addPrimitiveSet(new osg::DrawArrays(GL_LINES, next, numVertices));

    vertices->dirty();
    dirtyBound();
    dirtyDisplayList();
    std::cout << "update finished" << std::endl;
  }

};

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    static GLfloat time = STEP_TIME;
    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;


    time -= deltaTime;
    if(time > 0)
      return;

    time += STEP_TIME;
    //if(num_visited == m_maze->size())
      //return;

    Cell* neighbour = maze->getUnvisitedNeighbour(*currentCell);
    if(neighbour != 0)
    {
      cellStack.push(currentCell);
      maze->remove_wall(*currentCell, *neighbour);
      currentCell = neighbour;
      currentCell->visited = true;
      maze->update_buffer();
    }
    else if(!cellStack.empty())
    {
      currentCell = cellStack.top();
      cellStack.pop();
      maze->update_buffer();
    }

    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  maze = new Maze(NUM_ROWS, NUM_COLS);
  leaf->addDrawable(maze);
  root->addChild(leaf);
  root->addUpdateCallback(new RootUpdate);

  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  currentCell = maze->getCell(0, 0);
  currentCell->visited = true;

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  // single window will set camera as perspective, so it should be called first
  viewer.apply(new osgViewer::SingleWindow(0, 0, WIDTH, HEIGHT));

  osg::Camera* camera  = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0, WIDTH, 0, HEIGHT));


  return viewer.run();
}
