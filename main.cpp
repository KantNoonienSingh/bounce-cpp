#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ball.hpp"

namespace {
    // Modify these parameters to adjust screen size
    static const int screenWidth = 800;
    static const int screenHeight = 800;

    static const float ballRadius = 0.2; // > Ball radius (between 0.1 and 1)
    static const float ballXSpeed = 0.1; // > Ball speed (between 0 and 0.25)
    static const float ballYSpeed = 0.25; // > Ball speed (between 0 and 0.25)
}

namespace {

    struct SDLParam {
        SDL_Window* window;
        SDL_GLContext context;
        int screenWidth;
        int screenHeight;
    };

    bool init_sdl(SDLParam& params)
    {
        // Initialize sdl
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            return false;
        }

        // Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // Create window
        params.window = SDL_CreateWindow("Infinite Space",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         params.screenWidth,
                                         params.screenHeight,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if (params.window == nullptr)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            return false;
        }

        // Create context
        params.context = SDL_GL_CreateContext(params.window);

        if (params.context == nullptr)
        {
            printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
            return false;
        }

        // Use Vsync
        if(SDL_GL_SetSwapInterval(1) != 0)
        {
            printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
            return false;
        }

        return true;
    }

    GLenum init_gl()
    {
        // Initialize Projection Matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Initialize Modelview Matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Initialize clear color
        glClearColor(0.2, 0.2, 0.2, 1.f);

        // Check for error
        return glGetError();
    }

    void on_key_press(const unsigned char key)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        (void)x;
        (void)y;

        // Toggle quad
        switch (key)
        {
            default:
                break;
        }
    }
}

namespace {
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
}

namespace {

    void loop(struct SDLParam& params,
              widget::Ball& ball,
              widget::Ball::Position& px,
              widget::Ball::Position& py)
    {
        //While application is running
        while (true)
        {
            //Handle events on queue
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type)
                {
                    // User quits
                    case SDL_QUIT: {
                        return;
                    }

                        // Handle keypress with current mouse position
                    case SDL_TEXTINPUT:
                    {
                        on_key_press(e.text.text[0]);
                        break;
                    }
                }
            }

            // Render...
            // Clear color buffer
            glClear(GL_COLOR_BUFFER_BIT);
            // Redraw the grad
            draw_grid(params.screenWidth, params.screenHeight);
            // Redraw the ball
            ball.draw(px, py);
            // Update screen
            SDL_GL_SwapWindow(params.window);
        }
    }
}


namespace {

    // Loads ball coordinates at start
    void load_position(widget::Ball::Position p[2])
    {
        // X-position
        p[0].angle = 0.02;
        p[0].coord = 0.02;
        p[0].wall2 = 1.0;
        p[0].wall1 = -1.0;
        p[0].speed = std::min<float>(std::max<float>(ballXSpeed, 0), 0.2);
        // Y-position
        p[1].angle = 0.02;
        p[1].coord = 0.02;
        p[1].wall2 = 1.0;
        p[1].wall1 = -1.0;
        p[1].speed = std::min<float>(std::max<float>(ballYSpeed, 0), 0.2);
    }
}

/*! Entry point
 */
int main(void)
{
    SDLParam params;
    params.screenWidth = screenWidth;
    params.screenHeight = screenHeight;

    widget::Ball::Geometry g;
    g.radius = ballRadius;
    g.vertexStepSize = 20;

    //Initialize SDL
    if (!init_sdl(params))
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize OpenGL
    GLenum error;
    if ((error = init_gl()) != GL_NO_ERROR)
    {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
        SDL_Quit();
        return 1;
    }

    // Create ball
    widget::Ball ball(g);

    // Designate ball starting position
    widget::Ball::Position p[2];
    load_position(p);

    //Enable text input
    SDL_StartTextInput();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glShadeModel(GL_FLAT);

    // Enter main program loop...
    loop(params, ball, p[0], p[1]);

    SDL_StopTextInput();
    SDL_Quit();

    return 0;
}
