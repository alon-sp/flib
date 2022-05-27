#ifndef FLGLHEADERH_INCLUDED
#define FLGLHEADERH_INCLUDED

/*=======C STD LIBS=======*/
#include<stdbool.h>

/*=======GLU/GLEW=======*/
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

/*=======STB IMAGE======*/
#include"stb_image.h"

#include"flArray.h"
#include"fllog.h"
#include"flfi.h"

//Default shader attribute locations that are binded during linking program.
#define FLGL_ATTRIBLOC_VTXPOS        0
#define FLGL_ATTRIBLOC_VTXNORM       1
#define FLGL_ATTRIBLOC_VTXTEXCOORD   2
#define FLGL_ATTRIBLOC_VTXCLR        3

#define FLGL_UNIFORM_NAME_MODEL      "uModel"
#define FLGL_UNIFORM_NAME_VIEW       "uView"
#define FLGL_UNIFORM_NAME_PROJ       "uProj"
#define FLGL_UNIFORM_NAME_MATDIFF    "uMat.diff"
#define FLGL_UNIFORM_NAME_MATSPEC    "uMat.spec"
#define FLGL_UNIFORM_NAME_MATSHINE   "uMat.shine"

/*=========Shader==========*/
//-------------------------
/**
 * 
 * @param shaderType 
 * @param shaderSrc 
 * @param errLogDP Destination for pointer to the err log object that will be created
 * during error condition
 * @return non-zero value representing the id of the created shader object | zero otherwise
 */
GLuint flglCreateAndCompileShader(GLuint shaderType, const GLchar* shaderSrc, flLog** errlogPD);

flLog* flglGetShaderInfolog(GLuint shader);

/*=========Program==========*/
//-------------------------

/**
 * @note this function does nothing if any of it's parameter evaluates to false.
 * @param shaderProgram 
 * @param vshader
 * @param fshader
 * @param deleteShaders Whether the shaders should be deleted after successful linking of program
 * @param errlogPD Destination for pointer to the err log object that will be created
 * during error condition
 * @return true if no error occur during linking | false otherwise. 
 */
bool flglLinkProgram(GLuint shaderProgram, GLuint vshader, GLuint fshader, bool deleteShaders, flLog** errlogPD);

/**
 * @brief Create the openGL program object from source string and perform all neccessary cleanup operations
 * @param vertexShaderSrc
 * @param fragShaderSrc
 * @param errlogPD
 * @return The id of the openGL program object which will be 0(zero) if any error occur.
 */
GLuint flglCreateProgramFromSrc(const char* vertexShaderSrc, const char* fragShaderSrc, flLog** errlogPD);

/**
 * @brief Create the openGL program object from source file and perform all neccessary cleanup operations
 * 
 * @param vertexShaderPath 
 * @param fragShaderPath 
 * @param errlogPD 
 * @return The id of the openGL program object which will be 0(zero) if any error occur.
 */
GLuint flglCreateProgramFromFile(const char* vertexShaderPath, const char* fragShaderPath, flLog** errlogPD);

flLog* flglGetProgramInfolog(GLuint program);

/*==========flglShaderProgram==========*/
//-------------------------------------
typedef struct flglShaderProgram flglShaderProgram;
struct flglShaderProgram{
    GLuint id;
    GLint ulModel, ulView, ulProj; //ul -> uniform location
    GLint ulMatDiff, ulMatSpec, ulMatShine;
};
#define flglShaderProgramInit() {  .id = 0, .ulModel = -1, .ulView = -1, .ulProj = -1,\
    .ulMatDiff = -1, .ulMatSpec = -1, .ulMatShine = -1  }

flglShaderProgram flglShaderProgramNew(const char* vertexShaderSrc, const char* fragShaderSrc, flLog** errlogPD);

flglShaderProgram flglShaderProgramNewFromFile(const char* vertexShaderPath, const char* fragShaderPath, flLog** errlogPD);

/*==========Buffer==========*/
//----------------------------
GLuint flglGenBuffer(GLenum target, GLsizeiptr dataSize, const void* data, GLenum usage);

/*==========Texture==========*/
//----------------------------
GLuint flglGenTexture(const uint8_t* data, int width, int height, uint8_t nChannels);

GLuint flglGenTextureFromFile(const char* filePath, flLog** errlogPD);

const char* flglGetError();

#endif
