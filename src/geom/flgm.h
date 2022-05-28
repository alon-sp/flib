#ifndef FLGMHEADERH_INCLUDED
#define FLGMHEADERH_INCLUDED

#include<stdint.h>
#include<string.h>
#include"flm.h"
#include"flgl.h"

/*========================================Basic Mesh========================================*/
//-------------------------------------------------------------------------------------------

typedef struct flgmbmMat flgmbmMat;
struct flgmbmMat{
    GLuint diffTexID;
    GLuint specTexID;   
    GLint shine;
};
#define flgmbmMatInit() { .diffTexID = 0, .specTexID = 0, .shine = -1 };

#define flgmbmVTXD_POS       ( (uint8_t)1  )
#define flgmbmVTXD_NORM      ( (uint8_t)2  )
#define flgmbmVTXD_TEXCOORD  ( (uint8_t)64 )
#define flgmbmVTXD_CLR       ( (uint8_t)128 )

#define flgmbmTYPE_NIL         0
#define flgmbmTYPE_RECTANGLE   1
#define flgmbmTYPE_CUBE        2

typedef struct flgmBasicMesh flgmBasicMesh;
struct flgmBasicMesh{
    /**
     * @brief An array of vertex data: [Position, normal, texture coordinate and color, ...]
     * The flag associated with each data provided must be set in the $.vtxdFlags
     * All data types are not required but the ordering of data must be maintain, IE:
     * position data must come before normal data, normal data must come before texture coordinate,
     * and so on.
     */
    const GLfloat* const vtxd;
    #define _flgmbmGetVtxd(bm) ( (GLfloat*)(bm)->vtxd )
    const GLuint vtxdLen;
    const uint8_t vtxdFlags;
    #define _flgmbmSetVtxd(bm, vertexData, vertexDataLen, flags) do{\
        *( (GLfloat**)&(bm)->vtxd ) = (GLfloat*)(vertexData);\
        *( (GLuint*)&(bm)->vtxdLen ) = (vertexDataLen);\
        *( (uint8_t*)&(bm)->vtxdFlags ) = (flags);\
    }while(0)

    const GLuint* const indexes;
    #define _flgmbmGetIndexes(bm) ( (GLuint*)(bm)->indexes )
    const GLuint indexesLen;
    #define _flgmbmSetIndexes(bm, _indexes, _indexesLen) do{\
        *( (GLuint**)&(bm)->indexes ) = (GLuint*)(_indexes);\
        *( (GLuint*)&(bm)->indexesLen ) = (_indexesLen);\
    }while(0)

    const float16* transform_;//The underscore implies the memory associated with this pointer
                            //is not managed by this module.
    #define _flgmbmSetTransform(bm, transf)\
        (  *( (float16**)&(bm)->transform_ ) = (float16*)(transf)  )

    const float16* const _transformBuf;
    #define _flgmbmSetTransformBuf(bm, transfBuf) \
        (  *( (float16**)&(bm)->_transformBuf ) = (float16*)(transfBuf)  )

    const GLint type;//The type of mesh: any of flgmbmTYPE_* constants
    #define _flgmbmSetType(bm, _type) (  *( (GLint*)&(bm)->type ) = (_type)  )


    flgmbmMat mat;

    const GLuint vboID;
    #define flgmbmSetVboID(bm, _vboID) ( *( (GLuint*)&(bm)->vboID ) = (_vboID) )

    const GLuint iboID;
    #define flgmbmSetIboID(bm, _iboID) ( *( (GLuint*)&(bm)->iboID ) = (_iboID) )

    const GLuint vaoID;
    #define flgmbmSetVaoID(bm, _vaoID) ( *( (GLuint*)&(bm)->vaoID ) = (_vaoID) )

};

flgmBasicMesh* flgmbmNewBU(  GLfloat* vertexData, const GLuint vertexDataLen, const GLuint* indexes, 
        const GLuint indexesLen, uint8_t vtxdFlags, bool saveData, GLenum vboUsage,
        GLenum iboUsage);

#define flgmbmNew(vertexData, vertexDataLen, indexes, indexesLen, vtxdFlags, saveData)\
    flgmbmNewBU(  vertexData, vertexDataLen, indexes, indexesLen, vtxdFlags, saveData,\
        saveData? GL_DYNAMIC_DRAW : GL_STATIC_DRAW, GL_STATIC_DRAW )

void flgmbmFree(flgmBasicMesh* bm);

void flgmbmSetTransform(flgmBasicMesh* bm, float16* transform, bool saveCopy);

void flgmbmDraw(flgmBasicMesh* mesh, flglShaderProgram shaderProgram);

//void flgmbmComputeVertexNormals(flgmBasicMesh* mesh);

flgmBasicMesh* flgmbmNewRectangle(GLuint width, GLuint height, const GLfloat* vtxClrs, uint8_t vtxdFlags);

/*==========================================Utils==========================================*/
//-------------------------------------------------------------------------------------------
void* flgmComputeVertexNormal(
    const GLfloat* vtxs, GLuint vtxsStride, GLfloat* normDest, GLuint normDestStride, 
    const GLuint* indexes, GLuint indexesLen  );

#define flgmStrideCopy(dest, src, count, stride)\
    for(int i = 0; i<count; i++) *(dest + i*stride) = *(src+i)

#define flgmMemcpy(dest, src, nDataPerBlock, nBlocks, dataType, nDataStride)\
    for(int i = 0; i<nBlocks; i++)\
        memcpy( (char*)dest+i*nDataStride*sizeof(dataType), (char*)src+i*nDataPerBlock*sizeof(dataType),\
                nDataPerBlock*sizeof(dataType) )

#endif