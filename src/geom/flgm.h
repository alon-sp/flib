#ifndef FLGMHEADERH_INCLUDED
#define FLGMHEADERH_INCLUDED

#include<stdint.h>
#include"flm.h"
#include"flgl.h"

typedef struct flgmBmeshMat flgmBmeshMat;
struct flgmBmeshMat{
    uint diffTexID;
    uint diffTexLoc;

    uint specTexID;
    uint specTexLoc;
    
    float shininess;
};
#define flgmBmeshMatInit() {.diffTexID = 0, diffTexLoc = 0, specTexID = 0, specTexLoc};

flgmBmeshMat flgmBmeshMatNew(uint shaderProgram, uint diffTexID, uint specTexID, float shininess);

#define flgmVTXD_POS       1
#define flgmVTXD_NORM      2
#define flgmVTXD_TEXCOORD  64
#define flgmVTXD_CLR       128

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
    #define _flgmBmeshGetVtxd(bmesh) ( (float*)(bmesh)->vtxd )
    const uint vtxdLen;
    const uint8_t vtxdFlags;
    #define _flgmBmeshSetVtxd(bmesh, vertexData, vertexDataLen, flags) do{\
        *( (float**)&(bmesh)->vtxd ) = (vertexData);\
        *( (uint*)&(bmesh)->vtxdLen ) = (vertexDataLen);\
        *( (uint8_t*)&(bmesh)->vtxdFlags ) = (flags);\
    }while(0)

    const uint* const indices;
    #define _flgmBmeshGetIndices(bmesh) ( (uint*)(bmesh)->indices )
    const uint indicesLen;
    #define _flgmBmeshSetIndices(bmesh, _indices, _indicesLen) do{\
        *( (uint**)&(bmesh)->indices ) = (_indices);\
        *( (uint*)&(bmesh)->indicesLen ) = (_indicesLen);\
    }while(0)

    flgmBmeshMat mat;

    const uint vboID;
    #define flgmBmeshSetVboID(bmesh, _vboID) ( *( (uint*)&(bmesh)->vboID ) = (_vboID) )

    const uint iboID;
    #define flgmBmeshSetIboID(bmesh, _iboID) ( *( (uint*)&(bmesh)->iboID ) = (_iboID) )

    const uint vaoID;
    #define flgmBmeshSetVaoID(bmesh, _vaoID) ( *( (uint*)&(bmesh)->vaoID ) = (_vaoID) )

};

flgmBmesh* flgmBmeshNew(const float* vertexData, const uint vetexDataLen, 
                       const uint* indices, const uint indicesLen, bool computeNormal, uint8_t vtxdFlags);

void flgmBmeshDraw(flgmBmesh* bmesh);

//void flgmBmeshComputeVertexNormals(flgmBmesh* bmesh);

#endif