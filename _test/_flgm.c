
#include"_flgm.h"

static bool _flgmTestComputeVertexNormal(){
    float vertices[] = {//Rectangle
        //vertex(x,y,z)     vertex norms(random)  texture coord
        0.5f,  0.5f, 0.0f,  0.53f, 0.1f, 0.0f,    0.0f, 0.0f,
       -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,  0.7f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,     0.0f, 0.0f
    };

    int vertexCount = sizeof(vertices)/(8*sizeof(*vertices));
    uint stride = 8;

    float* texCoords = vertices+6;
    float *normDest = vertices+3;

    uint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    flgmComputeVertexNormal(vertices, stride, normDest, stride, indices, sizeof(indices)/sizeof(*indices));

    //Check to ensure that all vertex normals are equal to [0, 0, 1] and texture coords
    //remains unchanged.
    bool status = true;
    
    for(int i = 0; i<vertexCount; i++){
        status = flmhv3CompareEqual(*(flmhVector3*)(normDest+indices[i]*stride), (flmhVector3){0.0, 0.0, 1.0});
        if(!status) break;
        status = flmhv2CompareEqual(*(flmhVector2*)(texCoords+indices[i]*stride), (flmhVector2){0.0, 0.0});
        if(!status)break;
    }

    return status;
}

bool _flgmRunTests(){
    if(_flgmTestComputeVertexNormal()){
        printf("\n_flgmTestComputeVertexNormal: TEST OK");
    }else{
        flerrHandle("\nTESf _flgmRunTests: Test Failed !1(_flgmTestComputeVertexNormal)");
    }

    return true;
}