#include"flgl.h"

GLuint flglshCreateAndCompile(GLuint shaderType, const GLchar* shaderSrc, flLog** errlogPD){
    GLuint shader = glCreateShader(shaderType);
    if(!shader){
        if(errlogPD) *errlogPD = fllogNew("Failed to create shader object");
        return 0;
    }

    const GLchar* shaderSrcs[1] = {shaderSrc};
    glShaderSource(shader, 1, shaderSrcs, NULL);
    glCompileShader(shader);

    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if(compileStatus != GL_TRUE){
        if(errlogPD) *errlogPD = flglshGetInfolog(shader);
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

flLog* flglshGetInfolog(GLuint shader){
    GLchar infolog[512];
    int infologLength;
    glGetShaderInfoLog(shader, 512-1, &infologLength, infolog);
    *(infolog+infologLength) = '\0';
    
    return fllogNew(infolog);
}

bool flglpgLink(GLuint shaderProgram, GLuint vshader, GLuint fshader, bool deleteShaders, flLog** errlogPD){
    if(!(shaderProgram && vshader && fshader)) return false;

    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, fshader);
    glLinkProgram(shaderProgram);

    int linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

    if(linkStatus && deleteShaders){
        glDeleteShader(vshader);
        glDeleteShader(fshader);
    }

    if(linkStatus != GL_TRUE){
        if(errlogPD) *errlogPD = flglpgGetInfolog(shaderProgram);
        return false;
    }

    return true;
}

GLuint flglpgCreateFromSrc(const char* vertexShaderSrc, const char* fragShaderSrc, flLog** errlogPD){
    flLog* errlog = fllogNew("");

    GLuint progGL = 0;
    //Create and compile the vertex shader
    flLog* vertexShaderLog = NULL;
    GLuint vertexShader = flglshCreateAndCompile(GL_VERTEX_SHADER, vertexShaderSrc, &vertexShaderLog);
    if(!vertexShader){
        fllogPushs(errlog, 2, "\n\nvertex shader\n", fllogStr(vertexShaderLog));
        fllogPfree(&vertexShaderLog);
    }

    //Create and compile the fragment shader
    flLog* fragShaderLog = NULL;
    GLuint fragShader = flglshCreateAndCompile(GL_FRAGMENT_SHADER, fragShaderSrc, &fragShaderLog);
    if(!fragShader){
        fllogPushs(errlog, 2, "\n\nfragment shader\n", fllogStr(fragShaderLog));
        fllogPfree(&fragShaderLog);
    }

    //Create and link the program if there is no error so far
    if(vertexShader && fragShader){
        progGL = glCreateProgram();
        if(progGL){
            flLog* progLog = NULL;
            if(!flglpgLink(progGL, vertexShader, fragShader, false, &progLog)){
                fllogPushs(errlog, 2, "\n\n", fllogStr(progLog));
                fllogPfree(&progLog);
            }
        }else{
            fllogPushs(errlog, 2, "\n\n", "Failed to create shader program");
        }
    }

    //Perform cleanup operation
    if(vertexShader)glDeleteShader(vertexShader);
    if(fragShader)glDeleteShader(fragShader);

    if(errlog->length){
        if(progGL){
            glDeleteProgram(progGL);
            progGL = 0;
        }

        if(errlogPD) *errlogPD = errlog;
    }else{
        fllogPfree(&errlog);
    }

    return progGL;
}

GLuint flglpgCreateFromFile(const char* vertexShaderPath, const char* fragShaderPath, flLog** errlogPD){
    flLog* errlog = fllogNew("");
    GLuint progGL = 0;

    flLog* vertexShaderSrcLog = NULL;
    const char *vertexShaderSrc = flfiRead(vertexShaderPath, &vertexShaderSrcLog);
    if( !vertexShaderSrc ){
        fllogPushs(errlog, 2, "\n\nvertex shader\n", fllogStr(vertexShaderSrcLog));
        fllogPfree(&vertexShaderSrcLog);
    }

    flLog* fragShaderSrcLog = NULL;
    const char *fragShaderSrc = flfiRead(fragShaderPath, &fragShaderSrcLog);
    if( !fragShaderSrc ){
        fllogPushs(errlog, 2, "\n\nfragment shader\n", fllogStr(fragShaderSrcLog));
        fllogPfree(&fragShaderSrcLog);
    }

    if(vertexShaderSrc && fragShaderSrc){
        flLog* progLog = NULL;
        progGL = flglpgCreateFromSrc(vertexShaderSrc, fragShaderSrc, &progLog);
        if(!progGL){
            fllogPush(errlog, fllogStr(progLog));
            fllogPfree(&progLog);
        }
    }

    //perform cleanup operations
    if(vertexShaderSrc) flfiFree(vertexShaderSrc);
    if(fragShaderSrc) flfiFree(fragShaderSrc);

    if(errlog->length){
        if(errlogPD) *errlogPD = errlog;
    }else{
        fllogPfree(&errlog);
    }

    return progGL;
}

flLog* flglpgGetInfolog(GLuint program){
    GLchar infolog[512];
    int infologLength;
    glGetProgramInfoLog(program, 512-1, &infologLength, infolog);
    *(infolog+infologLength) = '\0';
    
    return fllogNew(infolog);
}

GLuint flglGenBuffer(GLenum target, GLsizeiptr dataSize, const void* data, GLenum usage){
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(target, bufferID);
    glBufferData(target, dataSize, data, usage);
    glBindBuffer(target, 0);

    return bufferID;
}