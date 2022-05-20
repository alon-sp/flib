#ifndef FLGLHEADERH_INCLUDED
#define FLGLHEADERH_INCLUDED

/*=======C STD LIBS=======*/
#include<stdbool.h>

/*=======GLU/GLEW=======*/
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include"flArray.h"
#include"fllog.h"
#include"flfi.h"

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

GLuint flglGenBuffer(GLenum target, GLsizeiptr dataSize, const void* data, GLenum usage);

GLuint flglGenTexture(const uint8_t* data, int width, int height, uint8_t nChannels);

GLuint flglGenTextureFromFile(const char* filePath, flLog** errlogPD);

#endif
