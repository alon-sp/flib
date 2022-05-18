#include"fluti.h"

bool flutiSDLcreateGLwindow(const char* glWindowTitle, int width, int height, SDL_Window** glWindowPD, SDL_GLContext* glContextTD){
    //Initialization flag
    bool success = true;

    SDL_Window* glWindow = NULL;
    SDL_GLContext glContext = NULL;
    
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }else{
        //Use OpenGL 3.3 core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        //Create glWindow
        if(!(width && height)){
            SDL_DisplayMode dispMode;
            SDL_GetCurrentDisplayMode(0, &dispMode);
            width = 0.6*dispMode.w;
            height = 0.6*dispMode.h;
        }

        glWindow = SDL_CreateWindow(glWindowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (!glWindow){
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }else{
            //Create context
            glContext = SDL_GL_CreateContext(glWindow);
            if (!glContext){
                printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;

                SDL_DestroyWindow( glWindow );
                glWindow = NULL;
            }else{
                //Initialize GLEW
                glewExperimental = GL_TRUE;
                GLenum glewError = glewInit();
                if (glewError != GLEW_OK){
                    printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
                }

                //Use Vsync
                if (SDL_GL_SetSwapInterval(1) < 0){
                    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                }
            }
        }
    }

    *glWindowPD = glWindow;
    *glContextTD = glContext;

    return success;
}