#define STB_IMAGE_IMPLEMENTATION

#include"flgl.h"

GLuint flglCreateAndCompileShader(GLuint shaderType, const GLchar* shaderSrc, flLog** errlogPD){
    GLuint shader = glCreateShader(shaderType);
    if(!shader){
        if(errlogPD) *errlogPD = fllogNew("Failed to create shader object");
        return 0;
    }

    const GLchar* shaderSrcs[1] = {shaderSrc};
    glShaderSource(shader, 1, shaderSrcs, NULL);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if(compileStatus != GL_TRUE){
        if(errlogPD) *errlogPD = flglGetShaderInfolog(shader);
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

flLog* flglGetShaderInfolog(GLuint shader){
    GLchar infolog[512];
    GLint infologLength;
    glGetShaderInfoLog(shader, 512-1, &infologLength, infolog);
    *(infolog+infologLength) = '\0';
    
    return fllogNew(infolog);
}

bool flglLinkProgram(GLuint shaderProgram, GLuint vshader, GLuint fshader, bool deleteShaders, flLog** errlogPD){
    if(!(shaderProgram && vshader && fshader)) return false;

    //Bind default attribute locations
    glBindAttribLocation(shaderProgram, FLGL_ATTRIBLOC_VTXPOS, "vtxPos");
    glBindAttribLocation(shaderProgram, FLGL_ATTRIBLOC_VTXNORM, "vtxNorm");
    glBindAttribLocation(shaderProgram, FLGL_ATTRIBLOC_VTXTEXCOORD, "vtxTexCoord");

    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, fshader);
    glLinkProgram(shaderProgram);

    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

    if(linkStatus && deleteShaders){
        glDeleteShader(vshader);
        glDeleteShader(fshader);
    }

    if(linkStatus != GL_TRUE){
        if(errlogPD) *errlogPD = flglGetProgramInfolog(shaderProgram);
        return false;
    }

    return true;
}

GLuint flglCreateProgramFromSrc(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc, flLog** errlogPD){
    flLog* errlog = fllogNew("");

    GLuint progGL = 0;
    //Create and compile the vertex shader
    flLog* vertexShaderLog = NULL;
    GLuint vertexShader = flglCreateAndCompileShader(GL_VERTEX_SHADER, vertexShaderSrc, &vertexShaderLog);
    if(!vertexShader){
        fllogPushs(errlog, 2, "\n\nvertex shader\n", fllogStr(vertexShaderLog));
        fllogPfree(&vertexShaderLog);
    }

    //Create and compile the fragment shader
    flLog* fragShaderLog = NULL;
    GLuint fragShader = flglCreateAndCompileShader(GL_FRAGMENT_SHADER, fragShaderSrc, &fragShaderLog);
    if(!fragShader){
        fllogPushs(errlog, 2, "\n\nfragment shader\n", fllogStr(fragShaderLog));
        fllogPfree(&fragShaderLog);
    }

    //Create and link the program if there is no error so far
    if(vertexShader && fragShader){
        progGL = glCreateProgram();
        if(progGL){
            flLog* progLog = NULL;
            if(!flglLinkProgram(progGL, vertexShader, fragShader, false, &progLog)){
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

GLuint flglCreateProgramFromFile(const GLchar* vertexShaderPath, const GLchar* fragShaderPath, flLog** errlogPD){
    flLog* errlog = fllogNew("");
    GLuint progGL = 0;

    flLog* vertexShaderSrcLog = NULL;
    const GLchar *vertexShaderSrc = flfiRead(vertexShaderPath, &vertexShaderSrcLog);
    if( !vertexShaderSrc ){
        fllogPushs(errlog, 2, "\n\nvertex shader\n", fllogStr(vertexShaderSrcLog));
        fllogPfree(&vertexShaderSrcLog);
    }

    flLog* fragShaderSrcLog = NULL;
    const GLchar *fragShaderSrc = flfiRead(fragShaderPath, &fragShaderSrcLog);
    if( !fragShaderSrc ){
        fllogPushs(errlog, 2, "\n\nfragment shader\n", fllogStr(fragShaderSrcLog));
        fllogPfree(&fragShaderSrcLog);
    }

    if(vertexShaderSrc && fragShaderSrc){
        flLog* progLog = NULL;
        progGL = flglCreateProgramFromSrc(vertexShaderSrc, fragShaderSrc, &progLog);
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

flLog* flglGetProgramInfolog(GLuint program){
    GLchar infolog[512];
    GLint infologLength;
    glGetProgramInfoLog(program, 512-1, &infologLength, infolog);
    *(infolog+infologLength) = '\0';
    
    return fllogNew(infolog);
}

static void flglspSetupUniforms(flglShaderProgram* sp){
    //Query default uniform locations
    sp->ulModel = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_MODEL);
    sp->ulView = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_VIEW);
    sp->ulProj = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_PROJ);
    sp->ulMatDiff = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_MATDIFF);
    sp->ulMatSpec = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_MATSPEC);
    sp->ulMatShine = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_MATSHINE);
    sp->ulMatClr = glGetUniformLocation(sp->id, FLGL_UNIFORM_NAME_CLR);
}

flglShaderProgram flglspNew(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc, flLog** errlogPD){
    flglShaderProgram sp = (flglShaderProgram){flglShaderProgram_};
    GLuint progGL = flglCreateProgramFromSrc(vertexShaderSrc, fragShaderSrc, errlogPD);
    if(progGL){
        sp.id = progGL;
        //Setup default uniforms
        flglspSetupUniforms(&sp);
    }
    return sp;
}

flglShaderProgram flglspNewFromFile(const GLchar* vertexShaderPath, const GLchar* fragShaderPath, flLog** errlogPD){
    flglShaderProgram sp = (flglShaderProgram){flglShaderProgram_};
    GLuint progGL = flglCreateProgramFromFile(vertexShaderPath, fragShaderPath, errlogPD);
    if(progGL){
        sp.id = progGL;
        //Setup default uniforms
        flglspSetupUniforms(&sp);
    }
    return sp;
}

GLuint flglGenBuffer(GLenum target, GLsizeiptr dataSize, const void* data, GLenum usage){
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(target, bufferID);
    glBufferData(target, dataSize, data, usage);
    glBindBuffer(target, 0);

    return bufferID;
}

GLuint flglGenTexture(const uint8_t* data, GLint width, GLint height, uint8_t nChannels){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    //Set the texture wrapping/filtering options
    //------------------------------------------
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load texture data
    //------------------
    GLuint format = GL_RGB;
    if(nChannels == 4) format = GL_RGBA;
    else if(nChannels == 1) format = GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

GLuint flglGenTextureFromFile(const GLchar* filePath, flLog** errlogPD){
    GLuint textureID = 0;
    GLint width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *data = stbi_load(filePath, &width, &height, &nChannels, 0);
    stbi_set_flip_vertically_on_load(false);

    if(data){
        textureID = flglGenTexture(data, width, height, nChannels);
        stbi_image_free(data);
        data = NULL;
    }else{
        if(errlogPD) *errlogPD = fllogNews(2, "Failed to load image: ", filePath);
    }

    return textureID;
}

const GLchar* flglGetError(){
    GLenum errcode;
    switch(errcode = glGetError()){
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
    }

    return errcode == GL_NO_ERROR? NULL : "";
}