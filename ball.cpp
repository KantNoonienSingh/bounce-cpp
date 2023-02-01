#include <cmath>
#include <functional>
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
    inline float radians(float ang) {
        return ang * ((::atan(1) * 4) / 180.0f);
    }

    /*!
     * @return vertices of circular slice
     * x = sin(ϕ) * cos(θ)
     * y = sin(ϕ) * sin(θ)
     * z = cos(ϕ)
     */
    std::vector<Vertex> make_slice(const Geometry& g, float phi, const ::size_t n)
    {
        phi = radians(phi);

        std::vector<Vertex> vertices(n);
        for(::size_t i = 0; i != vertices.size(); i++)
        {
            float theta = radians(i * g.vertexStepSize);

            vertices[i].x = g.radius * std::sin(phi) * std::cos(theta);
            vertices[i].y = g.radius * std::sin(phi) * std::sin(theta);
            vertices[i].z = g.radius * std::cos(phi);
        }

        return vertices;
    }

    /*! Draws a circular slice
     */
    void draw_layer(const std::vector<Vertex>& topVerts, const std::vector<Vertex>& botVerts)
    {
        ::size_t size;
        if ((size = topVerts.size()) == 0)
            return;

        for (::size_t i = 0; i != size - 1; ++i)
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


namespace {
    /*! Helper, called from on_bounce()
     */
    void on_bounce_impl(const Geometry& g, Position& p)
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

    /*! Default bounce callback handler
     */
    void on_bounce(const Geometry& g, Position& px, Position& py)
    {
        on_bounce_impl(g, px);
        on_bounce_impl(g, py);
    }
}

/*! ctor.
 */
Ball::Ball(const Geometry& geometry) : g_(geometry)
{
    g_.radius = std::min<float>(std::max<float>(g_.radius, 0.001), 1.0);

    // Round step size up to the nearest factor of 360
    if (g_.vertexStepSize == 0)
        g_.vertexStepSize = 360;
    else
        g_.vertexStepSize = static_cast<unsigned>(g_.vertexStepSize) + (360 % static_cast<unsigned>(g_.vertexStepSize));
}

namespace {

    /*! Helper
     */
    void step(Position& p, float n)
    {
        if (p.angle == 0)
            return;
        p.coord += n * (p.speed * p.angle / std::abs(p.angle));
    }
}

/*! Draws ball to screen
 */
void Ball::draw(Position& px, Position& py) {

    cb__ onBounce = std::bind(::on_bounce, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    draw(px, py, onBounce);
}

/*! Draws ball to screen
 */
void Ball::draw(Position& px, Position& py, const cb__& onBounce)
{
    glPushMatrix();

    step(px, px.speed);
    step(py, py.speed);
    glTranslatef(px.coord, py.coord, 0.0f);

    float ticks = static_cast<float>(SDL_GetTicks());
    glRotatef(ticks * 0.05, 5, 5, 1);

    // Create and draw vertices
    const ::size_t nSteps = 360 / g_.vertexStepSize;

    for (::size_t i = 0; i != nSteps; ++i)
    {
        float phi = i * g_.vertexStepSize;
        // Create two circular slices
        // & combine them into a single layer
        const std::vector<Vertex> topVerts = make_slice(g_, phi, 50);
        const std::vector<Vertex> botVerts = make_slice(g_, phi + g_.vertexStepSize, 50);
        draw_layer(topVerts, botVerts);
    }

    glPopMatrix();
    onBounce(g_, px, py);
}
