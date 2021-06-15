#include <cmath>
#include <vector>

#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "ball.hpp"

namespace
{
    /*!
     * @param angle in degrees
     * @return angle in radians
     */
    inline float deg2rad(const float ang) {
        return ang * ((std::atan(1) * 4) / 180.0f);
    }

    /*!
     * @return vertices of circular slice
     * x = sin(ϕ) * cos(θ)
     * y = sin(ϕ) * sin(θ)
     * z = cos(ϕ)
     */
    std::vector<widget::Ball::Vertex>
    make_slice(const widget::Ball::Geometry& g, float phi, const std::size_t n)
    {
        phi = deg2rad(phi);

        std::vector<widget::Ball::Vertex> vertices(n);
        std::size_t i = 0;
        for( ; i != vertices.size(); i++)
        {
            const float theta = deg2rad(i * g.vertexStepSize);

            vertices[i].x = g.radius * std::sin(phi) * std::cos(theta);
            vertices[i].y = g.radius * std::sin(phi) * std::sin(theta);
            vertices[i].z = g.radius * std::cos(phi);
        }

        return vertices;
    }

    /*! Draws a circular slice
     */
    void draw_layer(const std::vector<widget::Ball::Vertex>& topVerts,
                    const std::vector<widget::Ball::Vertex>& botVerts)
    {
        std::size_t size;
        if ((size = topVerts.size()) == 0)
            return;

        for (std::size_t i = 0; i != size - 1; ++i)
        {
            if (i % 2 == 0)
                glColor3f(0.66f, 0.00f, 0.22f);
            else
                glColor3f(0.70f, 0.70f, 0.70f);

            glBegin(GL_QUAD_STRIP);
            glVertex3f(topVerts[i+1].x, topVerts[i+1].y, topVerts[i+1].z);
            glVertex3f(topVerts[  i].x, topVerts[  i].y, topVerts[  i].z);

            glVertex3f(botVerts[i+1].x, botVerts[i+1].y, botVerts[i+1].z);
            glVertex3f(botVerts[  i].x, botVerts[  i].y, botVerts[  i].z);
            glEnd();
        }
    }
}

/*! ctor.
 */
widget::Ball::Ball(const Geometry& geometry) : g_(geometry)
{
    g_.radius = std::min<float>(std::max<float>(g_.radius, 0.1), 1.0);

    // Round step size up to the nearest factor of 360
    if (g_.vertexStepSize == 0)
        g_.vertexStepSize = 360;
    else
        g_.vertexStepSize = static_cast<unsigned>(g_.vertexStepSize) + (360 % static_cast<unsigned>(g_.vertexStepSize));
}

namespace {

    /*! Helper
     */
    void step(widget::Ball::Position& p, const float n)
    {
        if (p.angle == 0)
            return;
        p.coord += n * (p.speed * p.angle / std::abs(p.angle));
    }
}

/*! Draws ball to screen
 */
void widget::Ball::draw(Position& px, Position& py)
{
    draw(px, py, std::bind(&Ball::on_bounce,
                           this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           std::placeholders::_3));
}

/*! Draws ball to screen
 */
void widget::Ball::draw(Position& px,
                        Position& py,
                        std::function<void(Ball::Geometry&, Ball::Position&, Ball::Position&)> onBounce)
{
    glPushMatrix();

    step(px, px.speed);
    step(py, py.speed);
    glTranslatef(px.coord, py.coord, 0.0f);

    const float ticks = static_cast<float>(SDL_GetTicks());
    glRotatef(ticks * 0.05, 5, 5, 1);

    // Create and draw vertices
    const std::size_t nSteps = 360 / g_.vertexStepSize;

    for (std::size_t i = 0; i != nSteps; ++i)
    {
        const float phi = i * g_.vertexStepSize;
        // Create two circular slices
        // & combine them into a single layer
        std::vector<widget::Ball::Vertex> top = make_slice(g_, phi, 50);
        std::vector<widget::Ball::Vertex> bot = make_slice(g_, phi + g_.vertexStepSize, 50);
        draw_layer(top, bot);
    }

    glPopMatrix();

    // Handle potential bounce
    onBounce(g_, px, py);
}

namespace {
    /*! Helper, called from on_bounce()
     */
    void on_bounce_impl(const widget::Ball::Geometry& g, widget::Ball::Position& p)
    {
        if ((p.coord + g.radius) > p.wall2)
        {
            p.angle = std::abs(p.angle) * -1.0;
            p.coord = p.wall2 - g.radius; // Snap back to edge in case of resize
        }

        else if ((p.coord - g.radius) < p.wall1)
        {
            p.angle = std::abs(p.angle);
            p.coord = p.wall1 + g.radius; // Snap back to edge in case of resize
        }
    }
}

/*! Default bounce callback handler
 */
void widget::Ball::on_bounce(Ball::Geometry& g, Ball::Position& px, Ball::Position& py)
{
    on_bounce_impl(g, px);
    on_bounce_impl(g, py);
}
