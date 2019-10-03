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

namespace zxd {

#define image_size 16
GLubyte image1[image_size][image_size][4];
GLubyte image2[image_size][image_size][4];
GLubyte image3[image_size][image_size][4];
GLubyte image4[image_size][image_size][4];
GLubyte image5[image_size][image_size][4];
GLubyte image6[image_size][image_size][4];

GLdouble ztrans = -20;
GLint yaw = 0, pitch = 0, yaw_obj = 0;

GLuint vbo, ibo, tbo;
GLint polygon_mode = GL_FILL;

GLuint skybox_size = 50;

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

  void make_images(void) {
    int i, j, c;

    for (i = 0; i < image_size; i++) {
      for (j = 0; j < image_size; j++) {
        c = ((((i & 0x1) == 0) ^ ((j & 0x1) == 0))) * 255;
        image1[i][j][0] = (GLubyte)c;
        image1[i][j][1] = (GLubyte)c;
        image1[i][j][2] = (GLubyte)c;
        image1[i][j][3] = (GLubyte)255;

        image2[i][j][0] = (GLubyte)c;
        image2[i][j][1] = (GLubyte)c;
        image2[i][j][2] = (GLubyte)0;
        image2[i][j][3] = (GLubyte)255;

        image3[i][j][0] = (GLubyte)c;
        image3[i][j][1] = (GLubyte)0;
        image3[i][j][2] = (GLubyte)c;
        image3[i][j][3] = (GLubyte)255;

        image4[i][j][0] = (GLubyte)0;
        image4[i][j][1] = (GLubyte)c;
        image4[i][j][2] = (GLubyte)c;
        image4[i][j][3] = (GLubyte)255;

        image5[i][j][0] = (GLubyte)255;
        image5[i][j][1] = (GLubyte)c;
        image5[i][j][2] = (GLubyte)c;
        image5[i][j][3] = (GLubyte)255;

        image6[i][j][0] = (GLubyte)c;
        image6[i][j][1] = (GLubyte)c;
        image6[i][j][2] = (GLubyte)255;
        image6[i][j][3] = (GLubyte)255;
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

  void create_scene(void) {
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    GLfloat diffuse[4] = {1.0, 1.0, 1.0, 1.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    make_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image4);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image5);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, image_size,
      image_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, image6);

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
    glLoadIdentity();
    glRotatef(-yaw, 0, 1, 0);
    glRotatef(-pitch, 1, 0, 0);
    
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

    glutSolidSphere(5.0, 40, 20);

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
      "pP : polygon mode\n";
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
        ztrans = ztrans - 0.2;
        break;
      case 'R':
        ztrans = ztrans + 0.2;
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
      default:
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
