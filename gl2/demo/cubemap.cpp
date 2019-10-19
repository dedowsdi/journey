/*  cubemap.c
 *
 *  this program demonstrates cube map textures.
 *  six different colored checker board textures are
 *  created and applied to a lit sphere. there is a sphere with cubemap texture
 *  in a skybox.
 *
 *  note:
 *    GL_NORMAL_MAP and GL_REFLECT_MAP works on eye space, it use:
 *      tex = inverse_transpose(model_view) * n
 *    no matter how you rotate your camera or object, the same
 *    normal in view space always retrieve the same color from cube map.
 *   
 *  To udpate cubemap after camera rotation, we should transfom normal or
 *  reflection from view space to world space by set texture matrix to
 *  v_mat_i_i_t = v_mat_t
 *  
 */

#include <app.h>
#include <algorithm>
#include <texutil.h>
#include <array>

namespace zxd {

#define image_size 16

// load http://www.custommapmakers.org/skyboxes.php style skybox
std::string skybox_image;

GLubyte image_x[image_size][image_size][4]; // red
GLubyte image_y[image_size][image_size][4]; // green
GLubyte image_z[image_size][image_size][4]; // blue
GLubyte image_nx[image_size][image_size][4]; // yellow
GLubyte image_ny[image_size][image_size][4]; // magenta
GLubyte image_nz[image_size][image_size][4]; // cyan

GLdouble ztrans = -20;
GLint yaw = 0, pitch = 0, yaw_obj = 0;

GLuint vbo, ibo, tbo;
GLint polygon_mode = GL_FILL;

GLuint skybox_size = 50;
GLfloat ball_size = 2.5f;

bool swap_front_back = false;

// clang-format off
  GLfloat vertices[][3] = {
    {-1, -1, 1},
    {1,  -1, 1},
    {1,  1,  1},
    {-1, 1,  1},
    {-1, -1, -1},
    {1,  -1, -1},
    {1,  1,  -1},
    {-1, 1,  -1},
  };
  GLubyte indices[][4] = {
    {0,1,2,3}, //front
    {1,5,6,2}, //right
    {3,2,6,7}, //top
    {4,0,3,7}, //left
    {4,5,1,0}, //bottom
    {7,6,5,4} //back
  };
// clang-format on

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "cubemap";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void reload_skybox()
  {
    if (skybox_image.empty())
    {
      create_manual_skybox();
    }
    else
    {
      load_skybox();
    }
  }

  void make_images(void) {
    int i, j, c;
    GLfloat marker_size = 0.3f;

    for (i = 0; i < image_size; i++) {
      for (j = 0; j < image_size; j++) {
        c = ((((i & 0x1) == 0) ^ ((j & 0x1) == 0))) * 255;
        // total black for s axis
        if (i < image_size * marker_size && j <= image_size * (1 - marker_size))
        {
          c *= 0.0f;
        }

        // quater tone for t axis
        if (j < image_size * marker_size && i <= image_size * (1 - marker_size))
        {
          c *= 0.25f;
        }

        image_x[i][j][0] = (GLubyte)c;
        image_x[i][j][1] = (GLubyte)0;
        image_x[i][j][2] = (GLubyte)0;
        image_x[i][j][3] = (GLubyte)255;

        image_y[i][j][0] = (GLubyte)0;
        image_y[i][j][1] = (GLubyte)c;
        image_y[i][j][2] = (GLubyte)0;
        image_y[i][j][3] = (GLubyte)255;

        image_z[i][j][0] = (GLubyte)0;
        image_z[i][j][1] = (GLubyte)0;
        image_z[i][j][2] = (GLubyte)c;
        image_z[i][j][3] = (GLubyte)255;

        image_nx[i][j][0] = (GLubyte)c;
        image_nx[i][j][1] = (GLubyte)c;
        image_nx[i][j][2] = (GLubyte)0;
        image_nx[i][j][3] = (GLubyte)255;

        image_ny[i][j][0] = (GLubyte)c;
        image_ny[i][j][1] = (GLubyte)0;
        image_ny[i][j][2] = (GLubyte)c;
        image_ny[i][j][3] = (GLubyte)255;

        image_nz[i][j][0] = (GLubyte)0;
        image_nz[i][j][1] = (GLubyte)c;
        image_nz[i][j][2] = (GLubyte)c;
        image_nz[i][j][3] = (GLubyte)255;
      }
    }

    // cube map use left top as origin by convention
    flip_vertical(image_x[0][0], image_size, image_size, 4);
    flip_vertical(image_nx[0][0], image_size, image_size, 4);
    flip_vertical(image_y[0][0], image_size, image_size, 4);
    flip_vertical(image_ny[0][0], image_size, image_size, 4);
    flip_vertical(image_z[0][0], image_size, image_size, 4);
    flip_vertical(image_nz[0][0], image_size, image_size, 4);

    // flip_horizontal(image_x[0][0], image_size, image_size, 4);
    // flip_horizontal(image_nx[0][0], image_size, image_size, 4);
    // flip_horizontal(image_y[0][0], image_size, image_size, 4);
    // flip_horizontal(image_ny[0][0], image_size, image_size, 4);
    // flip_horizontal(image_z[0][0], image_size, image_size, 4);
    // flip_horizontal(image_nz[0][0], image_size, image_size, 4);
  }

  void flip_vertical(GLubyte* img, GLint width, GLint height, GLuint pixel_size)
  {
    auto row_size = width * pixel_size;
    auto total_size = row_size * height;
    for (auto i = 0; i < height / 2; ++i)
    {
      auto r0 = img + row_size * i;
      auto r1 = img + total_size - row_size * (i + 1);
      std::swap_ranges(r0, r0 + row_size, r1);
    }
  }

  void flip_horizontal(GLubyte* img, GLint width, GLint height, GLuint pixel_size)
  {
    auto row_size = width * pixel_size;
    auto total_size = row_size * height;
    for (auto i = 0; i < height; ++i)
    {
      auto r = img + row_size * i;
      for (auto j = 0; j < width/2; ++j)
      {
        auto c0 = r + j * pixel_size;
        auto c1 = r + (width - 1 - j) * pixel_size;
        std::swap_ranges(c0, c0 + pixel_size, c1);
      }
    }
  }

  void create_sky_box() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // array buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  }

  void create_manual_skybox()
  {
    make_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_nx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_ny);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_z);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_nz);
  }

  void load_skybox()
  {
    // http://www.custommapmakers.org/skyboxes.php use different naming
    // convension? it's very confusing what's front, front(+z) for cubemap is back
    // for skybox, some says one should name this files as
    // basename_n?[xyz].suffix
    std::array<std::string, 6> names = {
      "ft",    // right
      "bk",    // left
      "up",    // up
      "dn",    // down
      "rt",    // front
      "lf"     // back
    };

    if (swap_front_back)
    {
      // swap back, front, flip horizontal for left, right, front, back. flip
      // vertical for top, bottom.
      // 
      // Another way is to rotate cube by 180(swap back and front, left and
      // right, rotate top and bottom by 180), although everything still look
      // flipped horizontally.
      // 
      // The best way is probably to do this outside of application
      std::swap(names[4], names[5]);
    }

    for (auto i = 0; i < 6; ++i)
    {
      std::string fname = skybox_image + "_" + names[i] + ".tga";
      auto img = fipLoadResource32(fname);
      // cube map use left top as origin by convention
      img.flipVertical();

      if (swap_front_back)
      {
        if (i == 2 || i == 3)
        {
          img.flipVertical();
        }
        else
        {
          img.flipHorizontal();
        }
      }

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img.getWidth(),
        img.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());
    }
  }

  void create_scene(void) {
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    GLfloat diffuse[4] = {1.0, 1.0, 1.0, 1.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    reload_skybox();

    // generate s,t,r texture, use normal in view space as texcoord
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    create_sky_box();
  }

  void transpose(float* matrix)
  {
    std::swap(matrix[1], matrix[4]);
    std::swap(matrix[2], matrix[8]);
    std::swap(matrix[3], matrix[12]);

    std::swap(matrix[6], matrix[9]);
    std::swap(matrix[7], matrix[13]);

    std::swap(matrix[11], matrix[14]);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);

    glPushMatrix();
    glLoadIdentity();

    // camera transform : rot(pitch) * rot(yaw) * trans(0, 0, distance)
    glTranslatef(0, 0, ztrans);
    glRotatef(-yaw, 0, 1, 0);
    glRotatef(-pitch, 1, 0, 0);

    // draw skybox
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);

    glPushMatrix();
    // some people prefer to erase skybox translation
    // glLoadIdentity();
    // glRotatef(-yaw, 0, 1, 0);
    // glRotatef(-pitch, 1, 0, 0);
    
    glDisable(GL_LIGHTING);
    glScalef(skybox_size, skybox_size, skybox_size);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    // use vertex as texcoord for sky box
    glTexCoordPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
    glEnable(GL_LIGHTING);
    glPopMatrix();
    ZCGEA

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    // draw scene

    glPushMatrix();

    // transform reflect or normal back to world space
    // (v_mat_i)_i_t = v_t
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    m[12] = m[13] = m[14] = 0;
    transpose(m);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadMatrixf(m);

    glMatrixMode(GL_MODELVIEW);
    glRotatef(yaw_obj, 0, 1, 0);

    glutSolidSphere(ball_size, 32, 32);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_CUBE_MAP);
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    glWindowPos2i(20, 480);
    const GLubyte info[] =
      "q : use GL_NORMAL_MAP generate s,t,r\n"
      "w : use GL_REFLECTION_MAP to generate s,t,r\n"
      "eE : change skybox size\n"
      "rR : change camera distance\n"
      "uU : yaw camera\n"
      "iI : pitch camera\n"
      "oO : yaw object\n"
      "pP : polygon mode\n"
      "jJ : change ball size\n"
      "k  : swap front and back\n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_CUBE_MAP);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 800.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
      case 'Q':
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        break;
      case 'w':
      case 'W':
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        break;
      case 'e':
        skybox_size += 5;
        break;
      case 'E':
        if (skybox_size > 5) {
          skybox_size -= 5;
        }
        break;
      case 'r':
        ztrans = ztrans * 1.1;
        break;
      case 'R':
        ztrans = ztrans * 0.9;
        break;
      case 'u':
        yaw += 5;
        break;
      case 'U':
        yaw -= 5;
        break;
      case 'i':
        pitch += 5;
        break;
      case 'I':
        pitch -= 5;
        break;
      case 'o':
        yaw_obj += 5;
        break;
      case 'O':
        yaw_obj -= 5;
        break;
      case 'p': {
        polygon_mode = GL_POINT + (polygon_mode + 1 - GL_POINT) % 3;
      } break;
      case 'j':
        ball_size += 0.1;
        break;
      case 'J':
        if (ball_size > 0.5) {
          ball_size -= 0.1;
        }
        break;
      case 'k':
        swap_front_back ^= 1;
        reload_skybox();
      default:
        break;
    }
  }
};
}
int main(int argc, char** argv) {

  if (argc >= 2)
  {
    zxd::skybox_image = argv[1];
  }

  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
