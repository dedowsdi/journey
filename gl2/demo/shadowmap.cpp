/* shadowmap.c
 *
 * this is directional shadow map, only works for directional light.
 *
 * create shadow texture:
 *   use ortho projection to create shadow map for dir light
 *
 *   record depthBSVP = bias * scale * light_proj * light_view
 *
 *   bias and scale are used to transform ndc to [0,1]
 *
 *   copy depth texture to iamge.
 *
 * use shadow texture:
 *
 *  (s,t,r,q)^t = bias * scale * light_proj * light_view * model * obj_vertex
 *
 *  method 1: generate in object space
 *
 *    use glTexGen to generate above tex in object space.
 *    as every renderable has it's own model matrix, you need to set different
 *    texgen for every renderable, it's tedious.
 *
 *  method 2: generate in eye space
 *    (s,t,r,q) = bias * scale * light_proj * light_view * world_vertex
 *              = bias * scale * light_proj * light_view * inv_camera_view *
 * camera_vertex
 *    as glTexGen in eye space use following equation:
 *
 *        p^t * inv_model_view * eye_vertex
 *
 *    we must call glTexGen after camera is in position, but before reaching
 *    renderable transform.
 *
 *
 *   in both method, after we obtain the preceding matrix M, call:
 *     glTexGen(GL_S, ... , row0 of M)
 *     glTexGen(GL_T, ... , row1 of M)
 *     glTexGen(GL_R, ... , row2 of M)
 *     glTexGen(GL_Q, ... , row3 of M)
 *
 * reult:
 *
 *   cmp_result =  r/q <= tex(s/q, t/q) ? 1 : 0
 *   according to depth texture mode,  cmp_result will be  mutiplied relevant
 *   component in fragment..
 *
 *   set GL_TEXTURE_COMPARE_MODE to GL_COMPARE_R_TO_TEXTURE
 *   set GL_TEXTURE_COMPARE_FUNC to GL_LEQUAL
 *   set GL_DEPTH_TEXTURE_MODE to GL_LUMINANCE or GL_INTENSITY
 *
 *   issue : cast shadow on self.
 *
 */

#include "app.h"

namespace zxd {

#define PI 3.14159265359
#define SHADOW_MAP_WIDTH 256
#define SHADOW_MAP_HEIGHT 256

GLdouble fovy = 60.0;
GLdouble near_plane = 10.0;
GLdouble far_plane = 100.0;
GLdouble light_aspect = 1.0, light_near = 1, light_far = 80;
GLdouble light_top = 8;
GLdouble depthBSVP[16];  // depth bias * scale * proj * view
GLdouble mat_view[16];
GLboolean animate = GL_TRUE;

GLfloat angle = 0.0;
GLfloat torus_angle = 0.0;

GLfloat light_pos[] = {25.0, 25.0, 25.0, 1.0};
GLfloat lookat[] = {0.0, 0.0, 0.0};
GLfloat up[] = {0.0, 0.0, 1.0};  // gound on xy plane

GLboolean show_shadow = GL_FALSE;
GLboolean align_camera_to_light = GL_FALSE;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "shadowmap";
    m_info.display_mode = GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.5, 1.0, 0.0);
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

    // init light
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // set up shadow map
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH,
      SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // specify depth texture behavior
    glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
    /*glEnable(GL_NORMALIZE);*/
    ZCGEA;
  }

  void reshape(int width, int height) {
    app::reshape(width, height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLdouble)width / height, near_plane, far_plane);
    light_aspect = (GLdouble)width / height;
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0, 0, -30);
  }

  void update(void) {
    if (!animate) return;
    angle += PI / 10000;
    torus_angle += .1;
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q': {
        static GLboolean texture_on = GL_TRUE;
        texture_on = !texture_on;
        if (texture_on)
          glEnable(GL_TEXTURE_2D);
        else
          glDisable(GL_TEXTURE_2D);
      } break;

      case 'w': {
        static GLboolean compare_mode = GL_TRUE;
        compare_mode = !compare_mode;
        printf("compare mode %s\n", compare_mode ? "on" : "off");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
          compare_mode ? GL_COMPARE_R_TO_TEXTURE : GL_NONE);
      } break;

      case 'e': {
        static GLboolean func_mode = GL_TRUE;
        func_mode = !func_mode;
        printf("operator %s\n", func_mode ? "GL_LEQUAL" : "GL_GEQUAL");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,
          func_mode ? GL_LEQUAL : GL_GEQUAL);
      } break;

      case 'r':
        show_shadow = !show_shadow;
        break;

      case 'u': {
        int i;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, &i);
        if (i == GL_LUMINANCE) {
          printf("GL_DEPTH_TEXTURE_MODE : INTENSITY\n");
          glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        } else if (i == GL_INTENSITY) {
          printf("GL_DEPTH_TEXTURE_MODE : ALPHA\n");
          glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
        } else {
          printf("GL_DEPTH_TEXTURE_MODE : LUMINANCE\n");
          glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
        }
      } break;
      case 'i':
        light_top += 0.5;
        break;

      case 'I':
        light_top -= 0.5;
        break;

      case 'j':
        light_near += 0.5;
        break;

      case 'J':
        light_near -= 0.5;
        break;

      case 'k':
        light_far += 0.5;
        break;

      case 'K':
        light_far -= 0.5;
        break;

      case 'o': {
        animate = !animate;
      } break;
      case 'p': {
        align_camera_to_light = !align_camera_to_light;
      } break;
      default:
        break;
    }
  }

  void t_mat(GLdouble m[16]) {
    GLdouble tmp;
#define swap(a, b) \
  tmp = a;         \
  a = b;           \
  b = tmp
    swap(m[1], m[4]);
    swap(m[2], m[8]);
    swap(m[3], m[12]);
    swap(m[6], m[9]);
    swap(m[7], m[13]);
    swap(m[11], m[14]);
#undef swap
  }

  void setup_tex_gen() {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    t_mat(depthBSVP);
    glTexGendv(GL_S, GL_EYE_PLANE, &depthBSVP[0]);
    glTexGendv(GL_T, GL_EYE_PLANE, &depthBSVP[4]);
    glTexGendv(GL_R, GL_EYE_PLANE, &depthBSVP[8]);
    glTexGendv(GL_Q, GL_EYE_PLANE, &depthBSVP[12]);
  }

  void draw_objects(GLboolean shadow_render) {
    GLboolean texture_on = glIsEnabled(GL_TEXTURE_2D);

    // avoid shadow map take effect on 1st pass depth buffer
    if (shadow_render) glDisable(GL_TEXTURE_2D);

    if (!shadow_render) {
      // draw ground
      glNormal3f(0, 0, 1);
      glColor3f(1, 1, 1);
      glRectf(-30.0, -30.0, 30.0, 30.0);
    }
    // get model matrix

    glPushMatrix();
    glTranslatef(11, 11, 11);
    glRotatef(54.73, -5, 5, 0);
    glRotatef(torus_angle, 1, 0, 0);
    glColor3f(1, 0, 0);
    glutSolidTorus(1, 4, 8, 36);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2, 2, 2);
    glColor3d(0, 0, 1);
    glutSolidCube(4);
    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    // draw light
    glPushMatrix();
    glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
    glColor3f(0, 1, 0);
    glutWireSphere(0.5, 6, 6);
    glPopMatrix();

    if (shadow_render && texture_on) glEnable(GL_TEXTURE_2D);
  }

  void generate_shadow_map(void) {
    GLint viewport[4];

    // set viewport size to shadow map size
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT);
    /*glDrawBuffer(GL_NONE);*/
    /*glEnable(GL_POLYGON_OFFSET_FILL);*/
    /*glPolygonOffset(7, 1);*/

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // set up light view and projection
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(-light_top * light_aspect, light_top * light_aspect, -light_top,
      light_top, light_near, light_far);

    gluLookAt(light_pos[0], light_pos[1], light_pos[2], lookat[0], lookat[1],
      lookat[2], up[0], up[1], up[2]);
    glGetDoublev(g_l__m_o_d_e_l_v_i_e_w__m_a_t_r_i_x, depthBSVP);

    // render scene, generate depth buffer
    draw_objects(GL_TRUE);

    // get bias * scale * light_proj * light_view
    glLoadIdentity();
    glTranslated(0.5f, 0.5f, 0.5f);
    glScaled(0.5f, 0.5f, 0.5f);
    glMultMatrixd(depthBSVP);
    glGetDoublev(g_l__m_o_d_e_l_v_i_e_w__m_a_t_r_i_x, depthBSVP);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // copy depth buffer to shadow map
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0,
      SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0);

    glEnable(GL_LIGHTING);
    glCullFace(GL_BACK);
    /*glDrawBuffer(GL_BACK_LEFT);*/
    /*glPolygonOffset(0, 0);*/
    glDisable(GL_POLYGON_OFFSET_FILL);

    // restore viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // draw shadow image
    if (show_shadow) {
      int et2 = glIsEnabled(GL_TEXTURE_2D);
      if (et2) glDisable(GL_TEXTURE_2D);
      // depth
      GLfloat depth_image[SHADOW_MAP_WIDTH][SHADOW_MAP_HEIGHT];
      glReadPixels(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
        GL_DEPTH_COMPONENT, GL_FLOAT, depth_image);
      glWindowPos2f(viewport[2] / 2, 0);

      glDrawPixels(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT,
        GL_FLOAT, depth_image);

      // alpha
      glReadPixels(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_ALPHA,
        GL_FLOAT, depth_image);
      glWindowPos2f(viewport[2] / 2, viewport[3] / 2);
      glDrawPixels(
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_ALPHA, GL_FLOAT, depth_image);

      // color
      GLfloat color_image[SHADOW_MAP_WIDTH][SHADOW_MAP_HEIGHT][3];
      glReadPixels(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB, GL_FLOAT,
        color_image);
      glWindowPos2f(0, viewport[3] / 2);
      glDrawPixels(
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB, GL_FLOAT, color_image);

      if (et2) glEnable(GL_TEXTURE_2D);
      glutSwapBuffers();
    }
  }

  void display(void) {
    ZCGEA;
    GLfloat radius = 30;

    generate_shadow_map();

    if (show_shadow) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glLoadIdentity();
    if (align_camera_to_light) {
      gluLookAt(light_pos[0], light_pos[1], light_pos[2], lookat[0], lookat[1],
        lookat[2], up[0], up[1], up[2]);
    } else {
      gluLookAt(radius * cos(angle), radius * sin(angle), 30, lookat[0],
        lookat[1], lookat[2], up[0], up[1], up[2]);
    }
    // render scene with shadow map
    setup_tex_gen();
    draw_objects(GL_FALSE);
    glPopMatrix();

    glColor3f(1.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);
    char info[512];
    sprintf(info,
      "q : toggle GL_TEXTURE_2D\n"
      "w : toggle GL_TEXTURE_COMPARE_MODE\n"
      "e : toggle GL_TEXTURE_COMPARE_FUNC\n"
      "r : toggle scene depth alpha color views(ccw)\n"
      "u : toggle GL_DEPTH_TEXTURE_MODE\n"
      "iI: light_top : %.2f\n"
      "jJ: light_near : %.2f\n"
      "KK: light_far : %.2f\n"
      "o : toggle animation\n"
      "p : align camera to light\n",
      light_top, light_near, light_far);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
