#include <memory>
#include <vector>

#include <GLES3/gl3.h>
#include <GL/glu.h>

#include "ball.hpp"
#include "grid.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace {

    struct SDLParam {
        SDL_Window* window;
        SDL_GLContext context;
    };

    bool init_sdl(SDLParam& params, unsigned widthHint, unsigned heightHint)
    {
        // Initialize sdl
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            return (printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError()), false);
        }

        // GL 3.0 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        params.window = SDL_CreateWindow("Bounce",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         widthHint,
                                         heightHint,
                                         flags);
        if (params.window == nullptr) {
            return (printf("Window could not be created! SDL Error: %s\n", SDL_GetError()), false);
        }

        // Create context
        params.context = SDL_GL_CreateContext(params.window);
        if (params.context == nullptr) {
            return (printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError()), false);
        }

        SDL_GL_MakeCurrent(params.window, params.context);

        // Use Vsync
        if(SDL_GL_SetSwapInterval(1) != 0) {
            return (printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()), false);
        }

        return true;
    }
}

namespace {
    /*! Draws ball
     */
    inline void draw_ball(Ball& ball, Position& px, Position& py) {
        ball.draw(px, py);
    }
}

//! struct BallData
/*! Defines a moving ball, to be rendered using OpenGL1 primitives
 */
struct BallData {

    BallData(float ballRadius, float ballVertexStepSize) : ball(Geometry({ ballRadius, ballVertexStepSize })) {}
    // Rendered object
    Ball ball;
    // Current ball position
    Position ballPx, ballPy;
};

namespace {

    class Runner {

        unsigned screenWidth_;
        unsigned screenHeight_;
        std::shared_ptr<BallData> ballData_;

    public:

        void on_window_event(SDL_Event e) {

            if ((e.window).event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                screenWidth_ = e.window.data1;
                screenHeight_ = e.window.data2;
                glViewport(0, 0, screenWidth_, screenHeight_);
            }
        }

        void set_data(std::shared_ptr<BallData> data) {
            ballData_ = data;
        }

        void run(const SDLParam& params) {

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

                        case SDL_WINDOWEVENT:
                        {
                            on_window_event(e);
                            break;
                        }

                        // Handle keypress with current mouse position
                        case SDL_TEXTINPUT:
                        {
                            switch (e.text.text[0])
                            {
                                case SDLK_q: {
                                    return;
                                }
                            }

                            break;
                        }
                    }
                }

                render(params);
            }
        }

        void render(const SDLParam& params) {

            glClearColor(0.5, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            BallData& refballData = *ballData_;
            // Render...
            draw_grid(screenWidth_, screenHeight_);
            draw_ball(refballData.ball, refballData.ballPx, refballData.ballPy);

            // Update screen & return
            SDL_GL_SwapWindow(params.window);
        }
    };
}

/*! Entry point
 */
int main(void)
{
    const unsigned screenWidth = 100;
    const unsigned screenHeight = 100;

    //Initialize SDL
    SDLParam params;
    if (!init_sdl(params, screenWidth, screenHeight))
    {
        printf("Error initializing SDL\n");
        return 1;
    }

    static const float ballRadius = 0.08;
    static const float ballVertexStepSize = 20;
    static const float ballXSpeed = 0.1; // > Ball speed (between 0 and 0.25)
    static const float ballYSpeed = 0.05; // > Ball speed (between 0 and 0.25)

    // Designate ball starting positions
    // X-position
    std::shared_ptr<BallData> data(new BallData(ballRadius, ballVertexStepSize));

    data->ballPx.angle = 0.02;
    data->ballPx.coord = 0.02;
    data->ballPx.wall2 = 1.0;
    data->ballPx.wall1 = -1.0;
    data->ballPx.speed = std::min<float>(std::max<float>(ballXSpeed, 0), 0.2);
    // Y-position
    data->ballPy.angle = 0.02;
    data->ballPy.coord = 0.02;
    data->ballPy.wall2 = 1.0;
    data->ballPy.wall1 = -1.0;
    data->ballPy.speed = std::min<float>(std::max<float>(ballYSpeed, 0), 0.2);

    //Enable text input
    SDL_StartTextInput();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glShadeModel(GL_FLAT);

    Runner runner;
    runner.set_data(data);
    runner.run(params);

    SDL_StopTextInput();
    SDL_Quit();

    return 0;
}
