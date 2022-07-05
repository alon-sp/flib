#ifndef FLGMHEADERH_INCLUDED
#define FLGMHEADERH_INCLUDED

#include<stdint.h>
#include<string.h>
#include"flmh.h"
#include"flgl.h"

typedef struct flgmmsMaterial flgmmsMaterial;

//->flgm
//==========================================================================================
void* flgmComputeVertexNormal(
    const GLfloat* vtxs, GLuint vtxsStride, GLfloat* normDest, GLuint normDestStride, 
    const GLuint* indexes, GLuint indexesLen  );

//->flgmMesh
//==========================================================================================
typedef struct flgmMesh flgmMesh;
struct flgmMesh{
    void (*_free)(flgmMesh* mesh);
    void (*_draw)(flgmMesh* mesh, flglShaderProgram* shaderProgram);

    flmhMatrix* transform;
    flmhMatrix* _transfc;//If not NULL, content is a copy of the transform set by the user

    flgmmsMaterial* material;

    void* _;
};
#define flgmms_ ._free = NULL, ._draw = NULL, .transform = NULL,\
                ._transfc = NULL, .material = NULL, ._ = NULL
void flgmmsFree(flgmMesh* mesh);

void flgmmsSetTransform(flgmMesh* bm, flmhMatrix* transform, bool saveCopy);

#define flgmmsDraw(mesh, shaderProgram) mesh->_draw(mesh, shaderProgram)

//->flgmmsMaterial
//==========================================================================================
struct flgmmsMaterial{
    void* _;
    void (*_free)(flgmmsMaterial* mat);
};
#define flgmmsmt_ ._free = NULL, ._ = NULL
void flgmmsmtFree(flgmmsMaterial* mat);

//->flgmmsmtBasic
//==========================================================================================
typedef struct flgmmsmtBasic flgmmsmtBasic;
struct flgmmsmtBasic{
    #define flgmmsmtbs(mat) ((flgmmsmtBasic*)mat->_)
    GLuint diffTexID;
    GLuint specTexID;   
    GLint shine;
    flmhVector3 color;
};
#define flgmmsmtbs_ .diffTexID = 0, .specTexID = 0, .shine = -1

flgmmsMaterial* flgmmsmtbsNew(GLuint diffTexID, GLuint specTexID, GLint shine, flmhVector3 clrRGB);

//->flgmmsBasic
//==========================================================================================
#define flgmmsbsVTXD_POS       ( (uint8_t)1  )
#define flgmmsbsVTXD_NORM      ( (uint8_t)2  )
#define flgmmsbsVTXD_TEXCOORD  ( (uint8_t)64 )

typedef struct flgmmsBasic flgmmsBasic;
struct flgmmsBasic{
    #define flgmmsbs(mesh) ((flgmmsBasic*)mesh->_)
    /**
     * @brief An array of vertex data: [Position, normal, texture coordinate and color, ...]
     * The flag associated with each data provided must be set in the $.vtxdFlags
     * All data types are not required but the ordering of data must be maintain, IE:
     * position data must come before normal data, normal data must come before texture coordinate,
     * and so on.
     */
    const GLfloat* const vtxd;
    #define _flgmmsbsGetVtxd(bm) ( (GLfloat*)(bm)->vtxd )
    const GLuint vtxdLen;
    const GLuint vtxCount;
    const uint8_t vtxdFlags;
    #define _flgmmsbsSetVtxd(bm, vertexData, vertexDataLen, vertexCount, flags) do{\
        *( (GLfloat**)&(bm)->vtxd ) = (GLfloat*)(vertexData);\
        *( (GLuint*)&(bm)->vtxdLen ) = (vertexDataLen);\
        *( (GLuint*)&(bm)->vtxCount ) = (vertexCount);\
        *( (uint8_t*)&(bm)->vtxdFlags ) = (flags);\
    }while(0)

    const GLuint* const indexes;
    #define _flgmmsbsGetIndexes(bm) ( (GLuint*)(bm)->indexes )
    const GLuint indexesLen;
    #define _flgmmsbsSetIndexes(bm, _indexes, _indexesLen) do{\
        *( (GLuint**)&(bm)->indexes ) = (GLuint*)(_indexes);\
        *( (GLuint*)&(bm)->indexesLen ) = (_indexesLen);\
    }while(0)


    const GLuint vboID;
    #define flgmmsbsSetVboID(bm, _vboID) ( *( (GLuint*)&(bm)->vboID ) = (_vboID) )

    const GLuint iboID;
    #define flgmmsbsSetIboID(bm, _iboID) ( *( (GLuint*)&(bm)->iboID ) = (_iboID) )

    const GLuint vaoID;
    #define flgmmsbsSetVaoID(bm, _vaoID) ( *( (GLuint*)&(bm)->vaoID ) = (_vaoID) )

};

flgmMesh* flgmmsbsNew(  
    GLfloat* vertexData, const GLuint vertexDataLen, GLuint vertexCount, uint8_t vtxdFlags,
     const GLuint* indexes, const GLuint indexesLen, bool saveData, 
     GLenum vboUsage, GLenum iboUsage );

#define flgmmsbsNewDU(vertexData, vertexDataLen, vertexCount, vtxdFlags, indexes, indexesLen, saveData)\
    flgmmsbsNew(  vertexData, vertexDataLen, vertexCount, vtxdFlags, indexes, indexesLen, saveData,\
        saveData? GL_DYNAMIC_DRAW : GL_STATIC_DRAW, GL_STATIC_DRAW )

flgmMesh* flgmmsbsRectangleNew(GLuint w, GLuint h, uint8_t vtxdFlags);

flgmMesh* flgmmsbsBoxNew(GLuint width, GLuint height, GLuint breadth);

#endif