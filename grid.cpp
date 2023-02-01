#include <GLES3/gl3.h>
#include <GL/glu.h>

/*! Draws a background grid 
 */
void draw_grid(int width, int height)
{
    glColor3f(0.6f, 0.6f, 0.0f);
    glPushMatrix();
    glBegin(GL_LINES);

    for (float i = -width ; i <= width; i += 0.5)
    {
        // Horizontal lines
        glVertex3f(+width, i, 0.0f);
        glVertex3f(-width, i, 0.0f);
        // Vertical  lines
        glVertex3f(i, +height, 0.0f);
        glVertex3f(i, -height, 0.0f);
    }

    glEnd();
    glPopMatrix();
}
