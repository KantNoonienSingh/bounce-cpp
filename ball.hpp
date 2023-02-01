#pragma once

#ifndef BALL_HPP
#define BALL_HPP

#include <functional>

//! @struct Geometry
/*! ball physical shape */
struct Geometry {
    float radius, vertexStepSize;
};

//! @struct Position
/*! ball physical location */
struct Position {
    float angle;
    float coord;
    float speed;
    float wall1, wall2;
};

//! @struct Vertex
/*! opengl triangle vertex */
struct Vertex {

    float x, y, z;
    inline Vertex() : x(0), y(0), z(0) {}
};

//! @class Ball
/*! A Ball that bounces between walls
 */
class Ball {

    // Ball shape
    Geometry g_;

public:

    typedef std::function<void(const Geometry&, Position&, Position&)> cb__;

    //! ctor.
    // @param    geometry ball shape
    explicit Ball(const Geometry& geometry);

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
    void draw(Position& px, Position& py, const cb__& onBounceCB);
};

#endif
