
#include"flgl.h"
#include"fluti.h"

#include"flgm.h"

static SDL_Window* glpWindow = NULL;
static int glpWindowWidth = 0, glpWindowHeight = 0;
static SDL_GLContext glpContext = NULL;

static bool glpInit();
static void glpRender(float dt);
static void glpCleanup();

static bool rightKeyDown = false, upKeyDown = false;

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
            // if(ev.type == SDL_KEYDOWN){
            //     int dir = 0;
            //     switch(ev.key.keysym.sym){
            //         case SDLK_RIGHT:
            //             rightKeyDown = true;
            //             break;
            //         case SDLK_UP:
            //             upKeyDown = true;
            //             break;
            //     }
            // }
            // else if(ev.type == SDL_KEYUP){
            //     int dir = 0;
            //     switch(ev.key.keysym.sym){
            //         case SDLK_RIGHT:
            //             rightKeyDown = false;
            //             break;
            //         case SDLK_UP:
            //             upKeyDown = false;
            //             break;
            //     }
            // }
        }

        glpRender( (SDL_GetTicks()-endMillis)/1000.0f);

        //Update screen
        SDL_GL_SwapWindow(glpWindow);

        while(SDL_GetTicks() - startMillis < 16);

        endMillis = startMillis;
        startMillis = SDL_GetTicks();
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
static const char* vsPathGL = "../../../_testProg/glBmLplane/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glBmLplane/fs.glsl";
static const char* tex1PathGL = "../../../_testProg/res/images/clflower.jpg";
static flglShaderProgram progGL;
static GLuint diffTexGL;
static flgmMesh* bmesh;
static flmhMatrix proj;
static flmhOrthonormalBasis view;
static GLint ulLightPos, ulLightClr, ulViewPos;

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

    //==Create GL textures
    diffTexGL = flglGenTextureFromFile(tex1PathGL, &errlog);
    if(!diffTexGL) _printfErrlogAndExit(errlog);

    bmesh = flgmmsbsRectangleNew(2, 2, flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM|flgmmsbsVTXD_TEXCOORD);
    if(!bmesh){
        printf("\nFailed to create bmesh");
        return false;
    }
    
    bmesh->material = flgmmsmtbsNew(diffTexGL, 0, 16, flmhv3Zero());

    //Create a model matrix to rotate the rectangle about the x-axis
    flmhMatrix model = flmhmtRotateX(DEG2RAD*(90-30));
    flgmmsSetTransform(bmesh, &model, true);

    //Setup view matrix
    view = flmhobNewPTU( (flmhVector3){0, 0, 3}, (flmhVector3){0, 0, 0}, (flmhVector3){0, 1, 0});

    //setup projection matrix
    proj = flmhmtPerspective(DEG2RAD*45, glpWindowWidth/(float)glpWindowHeight, 0.1, 100);

    ulLightPos = glGetUniformLocation(progGL.id, "uLightPos");
    ulLightClr = glGetUniformLocation(progGL.id, "uLightClr");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.5, 0.5, 0.5, 1);
    
    return true;
}

void glpRender(float dt){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(progGL.id);

    static bool rotated = false;

    flmhobOrbitXZ(&view, 3, dt*60*DEG2RAD);

    glUniformMatrix4fv(progGL.ulView, 1, GL_FALSE, flmhmtValuePtr(flmhobGetViewTransform(&view)));
    glUniformMatrix4fv(progGL.ulProj, 1, GL_FALSE, flmhmtValuePtr(proj));
    
    glUniform3f(ulLightPos, 0, 0.5, 0);
    glUniform3f(ulLightClr, 1, 1, 1);
    
    flgmmsDraw(bmesh, &progGL);

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

    if(bmesh){
        flgmmsFree(bmesh);
        bmesh = NULL;
    }

    if(diffTexGL){
        glDeleteTextures(1, &diffTexGL);
        diffTexGL = 0;
    }
}