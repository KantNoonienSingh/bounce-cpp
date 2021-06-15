#ifndef _ball_hpp
#define _ball_hpp

#include <functional>

namespace widget {
    //! @class Ball
    /*! a Ball that bounces between walls
     */
    class Ball {
    public:
        //! @class Geometry
        /*! ball physical shape */
        struct Geometry {
            float radius, vertexStepSize;
        };

        //! @class Position
        /*! ball physical location */
        struct Position {
            float angle;
            float coord;
            float wall1;
            float wall2;
            float speed;
        };

        //! @class Vertex
        /*! opengl triangle vertex */
        struct Vertex {

            float x, y, z;
            inline Vertex() : x(0), y(0), z(0) {}
        };

        //! ctor.
        // @param    geometry ball shape
        Ball(const Geometry& geometry);

        //! Draws ball to screen
        //!
        //! @param px    ball x-coordinate
        //! @param px    ball y-coordinate
        void draw(Position& px, Position& py);

        //! Draws ball to screen, but takes an additional callback argument
        //!
        //! @param px          ball x-coordinate
        //! @param px          ball y-coordinate
        //! @param onBounce    callback, called on hitting a wall
        void draw(Position& px,
                  Position& py,
                  std::function<void(Ball::Geometry&, Ball::Position&, Ball::Position&)> onBounce);

    private:

        // Ball shape
        Geometry g_;
        // Default bounce callback handler
        void on_bounce(Ball::Geometry& g, Ball::Position& px, Ball::Position& py);
    };
}

#endif
