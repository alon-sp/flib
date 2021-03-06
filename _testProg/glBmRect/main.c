
#include"flgl.h"
#include"fluti.h"

#include"flgm.h"

static SDL_Window* glpWindow = NULL;
static int glpWindowWidth = 0, glpWindowHeight = 0;
static SDL_GLContext glpContext = NULL;

static bool glpInit();
static void glpRender();
static void glpCleanup();

int main(int argc, const char** argv){
    if(!flutiSDLcreateGLwindow("GL BM Rectangle", 0, 0, &glpWindow, &glpContext)) return -1;
    SDL_GL_GetDrawableSize(glpWindow, &glpWindowWidth, &glpWindowHeight);

    if(!glpInit()) return -1;

    bool quit = false;
    while(!quit){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type == SDL_QUIT){
                quit = true;
                break;
            }
        }

        glpRender();

        //Update screen
        SDL_GL_SwapWindow(glpWindow);
    }

    // float tmp[3] = {0.1, 0.2, 0.3};
    // flmhVector3 tmpS = *(flmhVector3*)tmp;
    // printf("\ntmps: %f, %f, %f", tmpS.x, tmpS.y, tmpS.z);

    //Perform cleanup operations
    //--------------------------

    glpCleanup();

	//Destroy window
	SDL_DestroyWindow( glpWindow );
    glpWindow = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}

//=====================================================================================
static const char* vsPathGL = "../../../_testProg/glBmRect/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glBmRect/fs.glsl";
static const char* tex1PathGL = "../../../_testProg/res/images/awesomeface.png";
static GLuint tex1GL;
static flglShaderProgram progGL;
flgmMesh* bmesh;

bool glpInit(){

    //Create and compile the vertex shader and fragment shader and link both to create the shader program
    //-------------------------------------------------------------------------------------------
    flLog* errlog;
#define _printfErrlogAndExit(errlog) do{\
    printf("%s", fllogStr(errlog));\
    fllogPfree(&errlog);\
    return false;\
}while(0)

    progGL = flglspNewFromFile(vsPathGL, fsPathGL, &errlog);
    if(!progGL.id) _printfErrlogAndExit(errlog);

    //==Create GL textures
    tex1GL = flglGenTextureFromFile(tex1PathGL, &errlog);
    if(!tex1GL) _printfErrlogAndExit(errlog);

    bmesh = flgmmsbsRectangleNew(1, 1, flgmmsbsVTXD_POS|flgmmsbsVTXD_TEXCOORD);
    if(!bmesh){
        printf("\nFailed to create bmesh");
        return false;
    }
    
    bmesh->material = flgmmsmtbsNew(tex1GL, 0, -1, (flmhVector3){1, 0, 0});

    //glViewport(0, 0, glpWindowWidth, glpWindowHeight);

    return true;
}

void glpRender(){
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(progGL.id);
    
    flgmmsDraw(bmesh, &progGL);

    glUseProgram(0);
}

void glpCleanup(){
    if(progGL.id){
        glDeleteProgram(progGL.id);
        progGL.id = 0;
    }
    if(tex1GL){
        glDeleteTextures(1, &tex1GL);
        tex1GL = 0;
    }

    if(bmesh){
        flgmmsFree(bmesh);
        bmesh = NULL;
    }
}