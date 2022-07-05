
#include"flgl.h"
#include"fluti.h"

#include"flgm.h"

static SDL_Window* glpWindow = NULL;
static int glpWindowWidth = 0, glpWindowHeight = 0;
static SDL_GLContext glpContext = NULL;

static bool glpInit();
static void glpRender(float dt);
static void glpCleanup();

static int dirLR = 0, dirUD = 0;

int main(int argc, const char** argv){
    if(!flutiSDLcreateGLwindow("GL BM Lighted plane(Lplane)", 0, 0, &glpWindow, &glpContext)) return -1;
    SDL_GL_GetDrawableSize(glpWindow, &glpWindowWidth, &glpWindowHeight);

    if(!glpInit()) return -1;

    bool quit = false;
    uint32_t startMillis = SDL_GetTicks(), endMillis = SDL_GetTicks();

    while(!quit){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type == SDL_QUIT){
                quit = true;
                break;
            }
            if(ev.type == SDL_KEYDOWN){
                switch(ev.key.keysym.sym){
                    case SDLK_LEFT:
                        dirLR = -1;
                        break;
                    case SDLK_RIGHT:
                        dirLR = 1;
                        break;
                    case SDLK_DOWN:
                        dirUD = -1;
                        break;
                    case SDLK_UP:
                        dirUD = 1;
                        break;
                }
            }
            else if(ev.type == SDL_KEYUP){
                switch(ev.key.keysym.sym){
                    case SDLK_LEFT:
                        if(dirLR < 0) dirLR = 0;
                        break;
                    case SDLK_RIGHT:
                        if(dirLR > 0) dirLR = 0;
                        break;
                    case SDLK_DOWN:
                        if(dirUD < 0) dirUD = 0;
                        break;
                    case SDLK_UP:
                        if(dirUD > 0) dirUD = 0;
                        break;
                }
            }
        }

        glpRender( (SDL_GetTicks()-endMillis)/1000.0f);

        //Update screen
        SDL_GL_SwapWindow(glpWindow);

        while(SDL_GetTicks() - startMillis < 16);

        endMillis = startMillis;
        startMillis = SDL_GetTicks();
    }

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
static const char* vsPathGL = "../../../_testProg/glBmCube/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glBmCube/fs.glsl";
static flglShaderProgram progGL;
static flgmMesh *boxMesh;
static flmhMatrix proj;
static flmhOrthonormalBasis view;
static GLint ulLightDir, ulLightClr;

bool glpInit(){

    //Create and compile the vertex shader and fragment shader and link both to create the shader program
    //-------------------------------------------------------------------------------------------
    flLog* errlog = NULL;
#define _printfErrlogAndExit(errlog) do{\
    printf("%s", fllogStr(errlog));\
    fllogPfree(&errlog);\
    return false;\
}while(0)

    progGL = flglspNewFromFile(vsPathGL, fsPathGL, &errlog);
    if(!progGL.id) _printfErrlogAndExit(errlog);

    //initialize box mesh
    boxMesh = flgmmsbsBoxNew(1, 1, 1);
    if(!boxMesh){
        printf("\nFailed to create box");
        return false;
    }
    boxMesh->material = flgmmsmtbsNew(0, 0, 8, (flmhVector3){0.6, 0.6, 0.6});
    flmhMatrix boxTransf = flmhmtIdentity();
    flgmmsSetTransform(boxMesh, &boxTransf, true);

    //Setup view matrix

    view = flmhobNewPTU( (flmhVector3){0, 0, 3}, (flmhVector3){0, 0, 0}, (flmhVector3){0, 1, 0});

    //setup projection matrix
    proj = flmhmtPerspective(DEG2RAD*45, glpWindowWidth/(float)glpWindowHeight, 0.1, 100);

    ulLightDir = glGetUniformLocation(progGL.id, "uLightDir");
    ulLightClr = glGetUniformLocation(progGL.id, "uLightClr");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.2, 0.2, 0.2, 1);
    
    return true;
}

static void drawBox(flmhMatrix transf, flmhVector3 color){
    flgmmsSetTransform(boxMesh, &transf, false);
    flgmmsmtbs(boxMesh->material)->color = color;
    flgmmsDraw(boxMesh, &progGL);
}

void glpRender(float dt){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(progGL.id);

    static bool rotated = false;

    if(dirUD) flmhobOrbitUD(&view, 3, dirUD*dt*60*DEG2RAD);
    if(dirLR) flmhobOrbitLR(&view, 3, dirLR*dt*60*DEG2RAD);

    flmhMatrix vtransf = flmhobGetViewTransform(&view);
    glUniformMatrix4fv(progGL.ulView, 1, GL_FALSE, (float*)&vtransf);
    glUniformMatrix4fv(progGL.ulProj, 1, GL_FALSE, flmhmtValuePtr(proj));
    
    glUniform3f(ulLightDir, view.z.x, view.z.y, view.z.z);
    glUniform3f(ulLightClr, 1, 1, 1);
    
    //Draw brown box
    drawBox(MatrixMultiply(MatrixScale(2, 1.0f/64, 2), MatrixTranslate(0, -0.6, 0)), (flmhVector3){0.6471, 0.3216, 0.1765});
    //Draw blue box
    drawBox(MatrixMultiply(MatrixScale(1.0f/32, 0.5f/64, 2), MatrixTranslate(-1, -0.6, 0)), (flmhVector3){0, 0, 0.5});
    //Draw green box
    drawBox(MatrixMultiply(MatrixScale(2, 0.5f/64, 1.0f/32), MatrixTranslate(0, -0.6, 1)), (flmhVector3){0, 0.5, 0});

    //Draw grey box
    drawBox(MatrixIdentity(), (flmhVector3){0.6, 0.6, 0.6});

    const char* errstr;
    if(errstr = flglGetError()){
        printf("%s", flglGetError());
    }

    glUseProgram(0);
}

void glpCleanup(){
    if(progGL.id){
        glDeleteProgram(progGL.id);
        progGL.id = 0;
    }

    if(boxMesh){
        flgmmsFree(boxMesh);
        boxMesh = NULL;
    }

}