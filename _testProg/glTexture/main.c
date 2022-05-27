
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
    if(!flutiSDLcreateGLwindow("GL Rectangle", 0, 0, &glpWindow, &glpContext)) return -1;
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
static const char* vsPathGL = "../../../_testProg/glTexture/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glTexture/fs.glsl";
static const char* tex1PathGL = "../../../_testProg/res/images/container.jpg";
static const char* tex2PathGL = "../../../_testProg/res/images/awesomeface.png";
static GLuint progGL;
static GLuint vboGL;
static GLuint iboGL;
static GLuint vaoGL;
static GLuint tex1GL;
static GLuint ulTex1GL;
static GLuint tex2GL;
static GLuint ulTex2GL;

bool glpInit(){

    //Create and compile the vertex shader and fragment shader and link both to create the shader program
    //-------------------------------------------------------------------------------------------
    flLog* errlog;
#define _printfErrlogAndExit(errlog) do{\
    printf("%s", fllogStr(errlog));\
    fllogPfree(&errlog);\
    return false;\
}while(0)

    progGL = flglCreateProgramFromFile(vsPathGL, fsPathGL, &errlog);
    if(!progGL) _printfErrlogAndExit(errlog);

    //==Create GL textures
    tex1GL = flglGenTextureFromFile(tex1PathGL, &errlog);
    if(!tex1GL) _printfErrlogAndExit(errlog);
    ulTex1GL = glGetUniformLocation(progGL, "uTex1");

    tex2GL = flglGenTextureFromFile(tex2PathGL, &errlog);
    if(!tex2GL) _printfErrlogAndExit(errlog);
    ulTex2GL = glGetUniformLocation(progGL, "uTex2");
    //--

    //Retrieve attribute locations
    //----------------------------
    GLint alVtxPosGL = glGetAttribLocation(progGL, "aVtxPos");
    GLint alVtxClrGL = glGetAttribLocation(progGL, "aVtxClr");
    GLint alVtxTexCoordGL = glGetAttribLocation(progGL, "aVtxTexCoord");

    //Set up openGL buffers
    //---------------------
    GLfloat vertices[] = {//Rectangle
        //vertex(x,y,z)     vertex color(rgb)  texture coord
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f
    };
    //Vertex buffer object
    vboGL = flglGenBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Index buffer object
    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    iboGL = flglGenBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Set up vertex Array object and link various attributes with respective buffers.
    //------------------------------------------------------------------------------
    glGenVertexArrays(1, &vaoGL);
    glBindVertexArray(vaoGL);
    //bind vbo
    glBindBuffer(GL_ARRAY_BUFFER, vboGL);
    //link and enable vertex position attribute
    glVertexAttribPointer(alVtxPosGL, 3, GL_FLOAT, false, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(alVtxPosGL);
    //link and enable vertex color attribute
    glVertexAttribPointer(alVtxClrGL, 3, GL_FLOAT, false, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(alVtxClrGL);
    //link and enable vertex texture coord attribute
    glVertexAttribPointer(alVtxTexCoordGL, 2, GL_FLOAT, false, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(alVtxTexCoordGL);

    //bind corresponding ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboGL);

    //unbind vao and buffers
    glBindVertexArray(0);//vao first
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Set up default color for clearing the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //glViewport(0, 0, glpWindowWidth, glpWindowHeight);

    return true;
}

void glpRender(){
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(progGL);
    //==Configure textures
    glUniform1i(ulTex1GL, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1GL);

    glUniform1i(ulTex2GL, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2GL);
    //--
    glBindVertexArray(vaoGL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void glpCleanup(){
    if(progGL){
        glDeleteProgram(progGL);
        progGL = 0;
    }
    if(vboGL){
        glDeleteBuffers(1, &vboGL);
        vboGL = 0;
    }
    if(iboGL){
        glDeleteBuffers(1, &iboGL);
        iboGL = 0;
    }
    if(tex1GL){
        glDeleteTextures(1, &tex1GL);
        tex1GL = 0;
    }
    if(tex2GL){
        glDeleteTextures(1, &tex2GL);
        tex2GL = 0;
    }
}