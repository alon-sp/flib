
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
    if(!flutiSDLcreateGLwindow("GL BM Lighted plane(Lplane)", 0, 0, &glpWindow, &glpContext)) return -1;
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
static const char* vsPathGL = "../../../_testProg/glBmLplane/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glBmLplane/fs.glsl";
static const char* tex1PathGL = "../../../_testProg/res/images/clflower.jpg";
static flglShaderProgram progGL;
static GLuint diffTexGL;
flgmBasicMesh* bmesh;
Matrix view, proj;
GLint ulLightPos, ulLightClr, ulViewPos;

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

    bmesh = flgmbmNewRectangle(2, 2, flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD);
    if(!bmesh){
        printf("\nFailed to create bmesh");
        return false;
    }
    
    bmesh->mat = (flgmbmMat){.diffTexID = diffTexGL, .specTexID = 0, .shine = 16};

    //Create a model matrix to rotate the rectangle 90 degrees about the x-axis
    Matrix model = MatrixRotateX(DEG2RAD*(90-30));
    float16 modelFv = MatrixToFloatV(model);
    flgmbmSetTransform(bmesh, &modelFv, true);

    //Setup view matrix
    view = MatrixLookAt((Vector3){0, 0, 2}, (Vector3){0, 0, 0}, (Vector3){0, 1, 0});

    //setup projection matrix
    proj = MatrixPerspective(DEG2RAD*45, glpWindowWidth/(float)glpWindowHeight, 0.1, 100);

    ulLightPos = glGetUniformLocation(progGL.id, "uLightPos");
    ulLightClr = glGetUniformLocation(progGL.id, "uLightClr");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1);
    
    return true;
}

void glpRender(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(progGL.id);
    
    glUniformMatrix4fv(progGL.ulView, 1, GL_FALSE, MatrixToFloat(view));
    glUniformMatrix4fv(progGL.ulProj, 1, GL_FALSE, MatrixToFloat(proj));
    
    glUniform3f(ulLightPos, 0, 0.5, 0);
    glUniform3f(ulLightClr, 1, 1, 1);
    
    flgmbmDraw(bmesh, progGL);

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
        flgmbmFree(bmesh);
        bmesh = NULL;
    }

    if(diffTexGL){
        glDeleteTextures(1, &diffTexGL);
        diffTexGL = 0;
    }
}