#ifndef FLGMHEADERH_INCLUDED
#define FLGMHEADERH_INCLUDED

#include<stdint.h>
#include"flm.h"
#include"flgl.h"

typedef struct flgmBmeshVtx flgmBmeshVtx;
struct flgmBmeshVtx{
    Vector3 pos;
    Vector3 normal;
    Vector2 texCoord;
};

typedef struct flgmBmeshMat flgmBmeshMat;
struct flgmBmeshMat{
    GLuint diffTexID;
    GLuint diffTexLoc;
    bool _ownDiffTex;

    GLuint specTexID;
    GLuint specTexLoc;
    bool _ownSpecTex;
    
    float shininess;
};

typedef struct flgmBmesh flgmBmesh;
struct flgmBmesh{
    flgmBmeshVtx* vtxs;
    uint vtxsLen;
    bool _ownVtxs;

    GLuint* indices;
    GLuint indicesLen;
    bool _ownIndices;

    flgmBmeshMat mat;

    GLuint vboID;
    GLuint iboID;
    GLuint vaoID;
};

void flgmBmeshSetVtxs(flgmBmesh* bmesh, float* vtxDataBuffer, uint vtxDataBufferSize, bool copy);
void flgmBmeshSetIndices(flgmBmesh* bmesh, uint* indexBuffer, uint indexBufferSize, bool copy);
void flgmBmeshSetupProgParams(flgmBmesh* bmesh, GLuint shaderProgram)

#endif