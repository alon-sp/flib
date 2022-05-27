#ifndef FLGMHEADERH_INCLUDED
#define FLGMHEADERH_INCLUDED

#include<stdint.h>
#include<string.h>
#include"flm.h"
#include"flgl.h"

typedef struct flgmBmeshMat flgmBmeshMat;
struct flgmBmeshMat{
    uint diffTexID;
    uint specTexID;   
    int shine;
};
#define flgmBmeshMatInit() { .diffTexID = 0, .specTexID = 0, .shine = -1 };

#define flgmVTXD_POS       ( (uint8_t)1  )
#define flgmVTXD_NORM      ( (uint8_t)2  )
#define flgmVTXD_TEXCOORD  ( (uint8_t)64 )
#define flgmVTXD_CLR       ( (uint8_t)128 )

typedef struct flgmBmesh flgmBmesh;
struct flgmBmesh{
    /**
     * @brief An array of vertex data: [Position, normal, texture coordinate and color, ...]
     * The flag associated with each data provided must be set in the $.vtxdFlags
     * All data types are not required but the ordering of data must be maintain, IE:
     * position data must come before normal data, normal data must come before texture coordinate,
     * and so on.
     */
    const float* const vtxd;
    #define _flgmBmeshGetVtxd(bm) ( (float*)(bm)->vtxd )
    const uint vtxdLen;
    const uint8_t vtxdFlags;
    #define _flgmBmeshSetVtxd(bm, vertexData, vertexDataLen, flags) do{\
        *( (float**)&(bm)->vtxd ) = (float*)(vertexData);\
        *( (uint*)&(bm)->vtxdLen ) = (vertexDataLen);\
        *( (uint8_t*)&(bm)->vtxdFlags ) = (flags);\
    }while(0)

    const uint* const indexes;
    #define _flgmBmeshGetIndexes(bm) ( (uint*)(bm)->indexes )
    const uint indexesLen;
    #define _flgmBmeshSetIndexes(bm, _indexes, _indexesLen) do{\
        *( (uint**)&(bm)->indexes ) = (uint*)(_indexes);\
        *( (uint*)&(bm)->indexesLen ) = (_indexesLen);\
    }while(0)

    const float16* transform_;//The underscore implies the memory associated with this pointer
                            //is not managed by this module.
    #define _flgmBmeshSetTransform(bm, transf)\
        (  *( (float16**)&(bm)->transform_ ) = (float16*)(transf)  )

    const float16* const _transformBuf;
    #define _flgmBmeshSetTransformBuf(bm, transfBuf) \
        (  *( (float16**)&(bm)->_transformBuf ) = (float16*)(transfBuf)  )


    flgmBmeshMat mat;

    const uint vboID;
    #define flgmBmeshSetVboID(bm, _vboID) ( *( (uint*)&(bm)->vboID ) = (_vboID) )

    const uint iboID;
    #define flgmBmeshSetIboID(bm, _iboID) ( *( (uint*)&(bm)->iboID ) = (_iboID) )

    const uint vaoID;
    #define flgmBmeshSetVaoID(bm, _vaoID) ( *( (uint*)&(bm)->vaoID ) = (_vaoID) )

};

flgmBmesh* flgmBmeshNew(  float* vertexData, const uint vertexDataLen, 
    const uint* indexes, const uint indexesLen, uint8_t vtxdFlags, bool saveData  );

void flgmBmeshFree(flgmBmesh* bm);

void flgmBmeshSetTransform(flgmBmesh* bm, float16* transform, bool saveCopy);

void flgmBmeshDraw(flgmBmesh* mesh, flglShaderProgram shaderProgram);

//void flgmBmeshComputeVertexNormals(flgmBmesh* mesh);

void* flgmComputeVertexNormal(
    const float* vtxs, uint vtxsStride, float* normDest, uint normDestStride, 
    const uint* indexes, uint indexesLen  );
#endif