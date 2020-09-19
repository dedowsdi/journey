/*
 * picksquare.c
 * use of multiple names and picking are demonstrated.
 * A 3x3 grid of squares is drawn.  when the left mouse
 * button is pressed, all squares under the cursor position
 * have their color changed.
 */
#include <app.h>

namespace zxd
{

int board[3][3]; /*  amount of color for each square	*/

class app0 : public app
{
    void init_info()
    {
        app::init_info();
        m_info.title = "picksquare";
        m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
        m_info.wnd_width = 100;
        m_info.wnd_height = 100;
    }

    /*  clear color value for every square on the board   */
    void create_scene(void)
    {
        int i, j;
        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++) board[i][j] = 0;
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }

    /*  the nine squares are drawn.  in selection mode, each
     *  square is given two names:  one for the row and the
     *  other for the column on the grid.  the color of each
     *  square is determined by its position on the grid, and
     *  the value in the board[][] array.
     */
    void draw_squares(GLenum mode)
    {
        GLuint i, j;
        for (i = 0; i < 3; i++)
        {
            if (mode == GL_SELECT) glLoadName(i);
            for (j = 0; j < 3; j++)
            {
                if (mode == GL_SELECT) glPushName(j);
                glColor3f((GLfloat)i / 3.0, (GLfloat)j / 3.0,
                    (GLfloat)board[i][j] / 3.0);
                glRecti(i, j, i + 1, j + 1);
                if (mode == GL_SELECT) glPopName();
            }
        }
    }

    /*  process_hits prints out the contents of the
     *  selection array.
     */
    void process_hits(GLint hits, GLuint buffer[])
    {
        unsigned int i, j;
        GLuint ii, jj, names, *ptr;

        printf("hits = %d\n", hits);
        ptr = (GLuint*)buffer;
        for (i = 0; i < hits; i++)
        { /*  for each hit  */
            names = *ptr;
            printf(" number of names for this hit = %d\n", names);
            ptr++;
            printf("  z1 is %g;", (float)*ptr / 0x7fffffff);
            ptr++;
            printf(" z2 is %g\n", (float)*ptr / 0x7fffffff);
            ptr++;
            printf("   names are ");
            for (j = 0; j < names; j++)
            { /*  for each name */
                printf("%d ", *ptr);
                if (j == 0)    /*  set row and column  */
                    ii = *ptr; // row
                else if (j == 1)
                    jj = *ptr; // col
                ptr++;
            }
            printf("\n");
            board[ii][jj] = (board[ii][jj] + 1) % 3;
        }
    }

/*  pick_squares() sets up selection mode, name stack,
 *  and projection matrix for picking.  then the
 *  objects are drawn.
 */
#define BUFSIZE 512

    void pick_squares(int button, int state, int x, int y)
    {
        if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

        GLuint select_buf[BUFSIZE];
        GLint hits;
        GLint viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);

        glSelectBuffer(BUFSIZE, select_buf);
        (void)glRenderMode(GL_SELECT);

        glInitNames();
        glPushName(0);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        /*  create 5x5 pixel picking region near cursor location	*/
        gluPickMatrix(
            (GLdouble)x, (GLdouble)(viewport[3] - y - 1), 5.0, 5.0, viewport);
        gluOrtho2D(0.0, 3.0, 0.0, 3.0);
        draw_squares(GL_SELECT);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glFlush();

        hits = glRenderMode(GL_RENDER);
        process_hits(hits, select_buf);
    }

    void display(void) override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        draw_squares(GL_RENDER);
        glFlush();
    }

    void reshape(int w, int h) override
    {
        app::reshape(w, h);
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, 3.0, 0.0, 3.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void keyboard(unsigned char key, int x, int y) override
    {
        app::keyboard(key, x, y);
        switch (key)
        {
        }
    }

    void mouse(int button, int state, int x, int y) override
    {
        pick_squares(button, state, x, y);
    }

    /* main loop */
};
} // namespace zxd
int main(int argc, char** argv)
{
    zxd::app0 _app0;
    _app0.run(argc, argv);
    return 0;
}
