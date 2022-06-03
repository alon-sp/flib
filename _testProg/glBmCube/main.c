
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

    // float tmp[3] = {0.1, 0.2, 0.3};
    // Vector3 tmpS = *(Vector3*)tmp;
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
static const char* vsPathGL = "../../../_testProg/glBmCube/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glBmCube/fs.glsl";
static const char* tex1PathGL = "../../../_testProg/res/images/clflower.jpg";
static flglShaderProgram progGL;
static GLuint diffTexGL;
static flgmBasicMesh* planeMesh, *boxMesh;
static Matrix proj;
static flmhOrthonormalBasis view;
static GLint ulLightDir, ulLightClr, ulHasTex;

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

    //Initialize plane mesh
    planeMesh = flgmbmNewRectangle(2, 2, flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD);
    if(!planeMesh){
        printf("\nFailed to create plane");
        return false;
    }
    planeMesh->mat = (flgmbmMat){.diffTexID = diffTexGL, .specTexID = 0, .shine = 16};
    //Create a model matrix to rotate the rectangle about the x-axis
    Matrix planeModel = MatrixRotateX(DEG2RAD*90);
    float16 planeModelV = MatrixToFloatV(planeModel);
    flgmbmSetTransform(planeMesh, &planeModelV, true);

    //initialize box mesh
    boxMesh = flgmbmNewBox(1, 1, 1);
    if(!boxMesh){
        printf("\nFailed to create box");
        return false;
    }
    boxMesh->mat = (flgmbmMat){.diffTexID = 0, .specTexID = 0, .shine = 32};
    float16 boxModellv = MatrixToFloatV(MatrixIdentity());
    flgmbmSetTransform(boxMesh, &boxModellv, true);
    flgmbmSetColor(boxMesh, ((Vector3){0.6, 0.6, 0.6}));

    //Setup view matrix

    flmhobCopy( (flmhOrthonormalBasis*)&view, 
        flmhobNewPTU( (Vector3){0, 0, 3}, (Vector3){0, 0, 0}, (Vector3){0, 1, 0})  );

    //setup projection matrix
    proj = MatrixPerspective(DEG2RAD*45, glpWindowWidth/(float)glpWindowHeight, 0.1, 100);

    ulLightDir = glGetUniformLocation(progGL.id, "uLightDir");
    ulLightClr = glGetUniformLocation(progGL.id, "uLightClr");
    ulHasTex = glGetUniformLocation(progGL.id, "uHasTex");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.5, 0.5, 0.5, 1);
    
    return true;
}

void glpRender(float dt){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(progGL.id);

    static bool rotated = false;

    if(dirUD) flmhobOrbitYZ(&view, 3, dirUD*dt*60*DEG2RAD);
    if(dirLR) flmhobOrbitXZ(&view, 3, dirLR*dt*60*DEG2RAD);

    glUniformMatrix4fv(progGL.ulView, 1, GL_FALSE, flmhobGetViewTransform(&view));
    glUniformMatrix4fv(progGL.ulProj, 1, GL_FALSE, MatrixToFloat(proj));
    
    glUniform3f(ulLightDir, -3, -3, -3);
    glUniform3f(ulLightClr, 1, 1, 1);
    
    glUniform1i(ulHasTex, GL_TRUE);
    flgmbmDraw(planeMesh, progGL);

    glUniform1i(ulHasTex, GL_FALSE);
    flgmbmDraw(boxMesh, progGL);

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

    if(planeMesh){
        flgmbmFree(planeMesh);
        planeMesh = NULL;
    }
    if(boxMesh){
        flgmbmFree(boxMesh);
        boxMesh = NULL;
    }

    if(diffTexGL){
        glDeleteTextures(1, &diffTexGL);
        diffTexGL = 0;
    }
}