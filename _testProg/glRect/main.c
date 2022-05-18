
#include"flgl.h"
#include"fluti.h"

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
static const char* vsPathGL = "../../../_testProg/glRect/vs.glsl";
static const char* fsPathGL = "../../../_testProg/glRect/fs.glsl";
static GLuint rectProgGL;
static GLuint rectVaoGL;

bool glpInit(){

    //Create and compile the vertex shader and fragment shader and link both to create the shader program
    //-------------------------------------------------------------------------------------------
    flLog* errlog;
    rectProgGL = flglpgCreateFromFile(vsPathGL, fsPathGL, &errlog);
    if(!rectProgGL){
        printf("%s", fllogStr(errlog));
        fllogPfree(&errlog);
        return false;
    }

    //Retrieve attribute locations
    //----------------------------
    GLuint alRectVertexPosGL = glGetAttribLocation(rectProgGL, "aRectVertexPos");
    GLuint alRectVertexColorGL = glGetAttribLocation(rectProgGL, "aRectVertexColor");

    //Set up openGL buffers
    //---------------------
    GLfloat rectVertices[] = {
        //vertex(x,y,z)     vertex color(rgb)
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f
    };
    //Vertex buffer object
    GLuint rectVboGL = flglGenBuffer(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    //Index buffer object
    GLuint rectIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint rectIboGL = flglGenBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    //Set up vertex Array object and link various attributes with respective buffers.
    //------------------------------------------------------------------------------
    glGenVertexArrays(1, &rectVaoGL);
    glBindVertexArray(rectVaoGL);
    //bind vbo
    glBindBuffer(GL_ARRAY_BUFFER, rectVboGL);
    //link and enable vertex position attribute
    glVertexAttribPointer(alRectVertexPosGL, 3, GL_FLOAT, false, 6*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(alRectVertexPosGL);
    //link and enable vertex color attribute
    glVertexAttribPointer(alRectVertexColorGL, 3, GL_FLOAT, false, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(alRectVertexColorGL);

    //bind corresponding ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectIboGL);

    //unbind vao and buffers
    glBindVertexArray(0);//vao first
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Set up default color for clearing the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

void glpRender(){
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rectProgGL);
    glBindVertexArray(rectVaoGL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void glpCleanup(){
    if(rectProgGL) glDeleteProgram(rectProgGL);
}