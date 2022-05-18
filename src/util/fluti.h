#ifndef FLUTIHEADERH_INCLUDED
#define FLUTIHEADERH_INCLUDED

#include<stdio.h>

#include"flgl.h"

/*=======SDL2=======*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

bool flutiSDLcreateGLwindow(const char* glWindowTitle, int width, int height, SDL_Window** glWindowPD, SDL_GLContext* glContextTD);

#endif