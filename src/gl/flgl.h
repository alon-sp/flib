#ifndef FLHEADERH_INCLUDED
#define FLHEADERH_INCLUDED

/*=======C STD LIBS=======*/
#include<stdbool.h>

/*=======GLU/GLEW=======*/
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

/*=======SDL2=======*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

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
GLuint flglshCreateAndCompile(GLuint shaderType, const GLchar* shaderSrc, flLog** errlogPD);

flLog* flglshGetInfolog(GLuint shader);

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
bool flglpgLink(GLuint shaderProgram, GLuint vshader, GLuint fshader, bool deleteShaders, flLog** errlogPD);

/**
 * @brief Create the openGL program object from source string and perform all neccessary cleanup operations
 * @param vertexShaderSrc
 * @param fragShaderSrc
 * @param errlogPD
 * @return The id of the openGL program object which will be 0(zero) if any error occur.
 */
GLuint flglpgCreateFromSrc(const char* vertexShaderSrc, const char* fragShaderSrc, flLog** errlogPD);

/**
 * @brief Create the openGL program object from source file and perform all neccessary cleanup operations
 * 
 * @param vertexShaderPath 
 * @param fragShaderPath 
 * @param errlogPD 
 * @return The id of the openGL program object which will be 0(zero) if any error occur.
 */
GLuint flglpgCreateFromFile(const char* vertexShaderPath, const char* fragShaderPath, flLog** errlogPD);

flLog* flglpgGetInfolog(GLuint program);

#endif