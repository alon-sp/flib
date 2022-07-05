#include"flgm.h"

static flgmmsMaterial* flgmmsmtNew();
static void flgmmsmtbsFree(flgmmsMaterial* mat);

//->flgm
//==========================================================================================
void* flgmComputeVertexNormal(
    const GLfloat* vtxs, GLuint vtxsStride, GLfloat* normDest, GLuint normDestStride, 
    const GLuint* indexes, GLuint indexesLen  ){
    //check if data is tightly packed
    if(!vtxsStride) vtxsStride = 3;//floats
    if(!normDestStride) normDestStride = 3;//floats

    //Clear $normDest buffer before computing normals
    for(GLuint i = 0; i<indexesLen; i++){
        * ((flmhVector3*)(normDest+indexes[i]*normDestStride) ) = flmhv3Zero();
    }

    //Loop through each triangle(3 indexes that form a face)
    for(GLuint i = 0; i<indexesLen; i += 3){
        //Get handle to the 3 vertexes of the triangle
        const flmhVector3* p0 = (const flmhVector3*)(vtxs+indexes[i+0]*vtxsStride);
        const flmhVector3* p1 = (const flmhVector3*)(vtxs+indexes[i+1]*vtxsStride);
        const flmhVector3* p2 = (const flmhVector3*)(vtxs+indexes[i+2]*vtxsStride);
        //Compute the triangle normal using p0, p1 and p2
        flmhVector3 tnorm = flmhv3CrossProduct(flmhv3Subtract(*p1, *p0), flmhv3Subtract(*p2, *p0));
        //Update the corresponding normals of the vertexes in $normDest
        *( (flmhVector3*)(normDest+indexes[i+0]*normDestStride) ) = flmhv3Add(
                *(flmhVector3*)(normDest+indexes[i+0]*normDestStride), tnorm  );
        *( (flmhVector3*)(normDest+indexes[i+1]*normDestStride) ) = flmhv3Add(
                *(flmhVector3*)(normDest+indexes[i+1]*normDestStride), tnorm  );
        *( (flmhVector3*)(normDest+indexes[i+2]*normDestStride) ) = flmhv3Add(
                *(flmhVector3*)(normDest+indexes[i+2]*normDestStride), tnorm  );
    }

    //Normalize computed normals in $normDest buffer
    for(GLuint i = 0; i<indexesLen; i++){
        *( (flmhVector3*)(normDest+indexes[i]*normDestStride) ) = flmhv3Normalize(
                *(flmhVector3*)(normDest+indexes[i]*normDestStride)  );
    }
}

//->flgmMesh
//==========================================================================================
static flutImplNew(flgmmsNew, flgmMesh, flgmms_)

void flgmmsFree(flgmMesh* mesh){
    if(!mesh) return;
    if(mesh->_free) mesh->_free(mesh);

    if(mesh->_transfc){
        flmmFree(mesh->_transfc);
        mesh->_transfc = NULL;
    }

    if(mesh->material){
        flgmmsmtFree(mesh->material);
        mesh->material = NULL;
    }

    flmmFree(mesh);
}

void flgmmsSetTransform(flgmMesh* mesh, flmhMatrix* transform, bool saveCopy){
    if(saveCopy){
        if(!mesh->_transfc){
            mesh->_transfc = flmmMalloc(sizeof(flmhMatrix));
            if(!mesh->_transfc) goto SET_TRANSFORM;
        }
        *mesh->_transfc = *transform;
    }

SET_TRANSFORM:
    mesh->transform =  (saveCopy && mesh->_transfc)? mesh->_transfc : transform;
}

//->flgmmsBasic
//==========================================================================================

static void flgmmsbsFree(flgmMesh* mesh);
static void flgmmsbsDraw(flgmMesh* mesh, flglShaderProgram* prog);
static bool flgmmsbsVtxdIsSupported(uint8_t flags);
static GLuint flgmmsbsGetIndex(uint8_t vtxdFlags, uint8_t targetFlag, GLuint vtxCount);
static GLuint flgmmsbsGetVtxdLen(uint8_t vtxdFlags, GLuint vtxCount);

flgmMesh* flgmmsbsNew(  
    GLfloat* vertexData, const GLuint vertexDataLen, GLuint vertexCount, uint8_t vtxdFlags,
     const GLuint* indexes, const GLuint indexesLen, bool saveData, 
     GLenum vboUsage, GLenum iboUsage ){
    
    if( !(vertexData && indexes && vertexDataLen && indexesLen) ) return NULL;
    if( !flgmmsbsVtxdIsSupported(vtxdFlags) ) return NULL;

    flgmmsBasic* bm = flmmMalloc(sizeof(flgmmsBasic));
    if(!bm) return NULL;

    if(saveData){
        GLfloat* vtxdBuf = flmmMalloc(vertexDataLen*sizeof(GLfloat));
        if(!vtxdBuf){
            flmmFree(bm);
            return NULL;
        }
        memcpy(vtxdBuf, vertexData, vertexDataLen*sizeof(GLfloat));

        GLuint* indxBuf = flmmMalloc(indexesLen*sizeof(GLuint));
        if(!indxBuf){
            flmmFree(bm);
            flmmFree(vtxdBuf);
            return NULL;
        }
        memcpy(indxBuf, indexes, indexesLen*sizeof(GLuint));

        _flgmmsbsSetVtxd(bm, vtxdBuf, vertexDataLen, vertexCount, vtxdFlags);
        _flgmmsbsSetIndexes(bm, indxBuf, indexesLen);
    }else{
        _flgmmsbsSetVtxd(bm, vertexData, vertexDataLen, vertexCount, vtxdFlags);
        _flgmmsbsSetIndexes(bm, indexes, indexesLen);
    }

    flgmmsbsSetVboID(bm, 0);
    flgmmsbsSetIboID(bm, 0);
    flgmmsbsSetVaoID(bm, 0);

    #define _fltmpFlagIsSet(_bm, targetFlag) ( (_bm)->vtxdFlags & (targetFlag) )
    
    if(_fltmpFlagIsSet(bm, flgmmsbsVTXD_NORM)){
        flgmComputeVertexNormal(  bm->vtxd, 0, 
            (GLfloat*)bm->vtxd + flgmmsbsGetIndex(bm->vtxdFlags, flgmmsbsVTXD_NORM, bm->vtxCount), 
            0, bm->indexes, bm->indexesLen  );
    }

    //Generate GL buffers
    flgmmsbsSetVboID( bm, 
        flglGenBuffer(GL_ARRAY_BUFFER, bm->vtxdLen * sizeof(*bm->vtxd), bm->vtxd, vboUsage)  );

    flgmmsbsSetIboID(bm, 
        flglGenBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->indexesLen * sizeof(*bm->indexes),
            bm->indexes, iboUsage
        ));

    //Generate GL vertex Array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    flgmmsbsSetVaoID(bm, vao);

    //Configure GL buffers
    //--------------------
    glBindVertexArray(bm->vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, bm->vboID);
    //position attribute
    if(_fltmpFlagIsSet(bm, flgmmsbsVTXD_POS)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXPOS, 3, GL_FLOAT, false,  0,
            (GLvoid*)(flgmmsbsGetIndex(bm->vtxdFlags, flgmmsbsVTXD_POS, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXPOS);
    }
    //normal attribute
    if(_fltmpFlagIsSet(bm, flgmmsbsVTXD_NORM)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXNORM, 3, GL_FLOAT, false, 0, 
            (GLvoid*)(flgmmsbsGetIndex(bm->vtxdFlags, flgmmsbsVTXD_NORM, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXNORM);
    }
    //texture coordinate
    if(_fltmpFlagIsSet(bm, flgmmsbsVTXD_TEXCOORD)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXTEXCOORD, 2, GL_FLOAT, false,  0, 
            (GLvoid*)(flgmmsbsGetIndex(bm->vtxdFlags, flgmmsbsVTXD_TEXCOORD, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXTEXCOORD);
    }

    //bind ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->iboID);

    //unbind GL objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if(!saveData){
        _flgmmsbsSetVtxd(bm, NULL, 0, 0, bm->vtxdFlags);
        _flgmmsbsSetIndexes(bm, NULL, bm->indexesLen);//@note: storing the index buffer
            //length despise setting it to NULL is important since it will be use later in
            //drawing the mesh(in call to glDrawElements)
    }
    
    //Mesh object
    flgmMesh* mesh = flgmmsNew();
    if(mesh){
        mesh->_ = bm;
        mesh->_free = flgmmsbsFree;
        mesh->_draw = flgmmsbsDraw;
    }else{
        flgmMesh ms = (flgmMesh){flgmms_, ._ = bm};
        flgmmsbsFree(&ms);
    }

    return mesh;

#undef _fltmpFlagIsSet
}

static void flgmmsbsFree(flgmMesh* mesh){
    flgmmsBasic* bm = flgmmsbs(mesh);

    if(!bm) return;

    if(bm->vtxd){
        flmmFree((void*)bm->vtxd);
        _flgmmsbsSetVtxd(bm, NULL, 0, 0, 0);
    }
    if(bm->indexes){
        flmmFree((void*)bm->indexes);
        _flgmmsbsSetIndexes(bm, NULL, 0);
    }

    if(bm->vboID){
        glDeleteBuffers(1, &bm->vboID);
        flgmmsbsSetVboID(bm, 0);
    }
    if(bm->iboID){
        glDeleteBuffers(1, &bm->iboID);
        flgmmsbsSetIboID(bm, 0);
    }
    if(bm->vaoID){
        glDeleteVertexArrays(1, &bm->vaoID);
        flgmmsbsSetVaoID(bm, 0);
    }

    flmmFree(bm);
    mesh->_ = NULL;

}

static void flgmmsbsDraw(flgmMesh* mesh, flglShaderProgram* prog){
    if(!(mesh->_draw == flgmmsbsDraw && mesh->material->_free == flgmmsmtbsFree)) return;

    flgmmsBasic* bmesh = mesh->_;
    flgmmsmtBasic* bmat = mesh->material->_;

    if(bmat->diffTexID && prog->ulMatDiff >= 0){
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(prog->ulMatDiff, 0);
        glBindTexture(GL_TEXTURE_2D, bmat->diffTexID);
    }
    if(bmat->specTexID && prog->ulMatSpec >= 0){
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(prog->ulMatSpec, 1);
        glBindTexture(GL_TEXTURE_2D, bmat->specTexID);
    }

    if(bmat->shine >= 0 && prog->ulMatShine >= 0){
        glUniform1f(prog->ulMatShine, bmat->shine);
    }

    if(prog->ulMatClr >= 0){
        glUniform3f(prog->ulMatClr, bmat->color.x, bmat->color.y, bmat->color.z);
    }

    if(mesh->transform && prog->ulModel >= 0){
        glUniformMatrix4fv(prog->ulModel, 1, false, flmhmtValuePtr(*mesh->transform));
    }
    
    glBindVertexArray(bmesh->vaoID);
    glDrawElements(GL_TRIANGLES, bmesh->indexesLen, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
}

static bool flgmmsbsVtxdIsSupported(uint8_t flags){
    switch(flags){
        case flgmmsbsVTXD_POS:
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM:
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_TEXCOORD:
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM|flgmmsbsVTXD_TEXCOORD:
            return true;
        default:
            return false;
    }
}

static GLuint flgmmsbsGetIndex(uint8_t vtxdFlags, uint8_t targetFlag, GLuint vtxCount){
    switch(vtxdFlags){
        case flgmmsbsVTXD_POS:
            break;
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM:
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_TEXCOORD:
            if(targetFlag != flgmmsbsVTXD_POS) return 3*vtxCount;
            break;
        
        case flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM|flgmmsbsVTXD_TEXCOORD:
            if(targetFlag == flgmmsbsVTXD_NORM) return 3*vtxCount;
            if(targetFlag == flgmmsbsVTXD_TEXCOORD) return (3*2)*vtxCount;
            break;
    }

    return 0;
}

static GLuint flgmmsbsGetVtxdLen(uint8_t vtxdFlags, GLuint vtxCount){
    GLuint activeFlagsFloatCount = 0;
    if(vtxdFlags & flgmmsbsVTXD_POS) activeFlagsFloatCount += 3;
    if(vtxdFlags & flgmmsbsVTXD_NORM) activeFlagsFloatCount += 3;
    if(vtxdFlags & flgmmsbsVTXD_TEXCOORD) activeFlagsFloatCount += 2;

    return activeFlagsFloatCount * vtxCount;
}

//->flgmmsRectangle
//==========================================================================================
flgmMesh* flgmmsbsRectangleNew(GLuint w, GLuint h, uint8_t vtxdFlags){
    if(!flgmmsbsVtxdIsSupported(vtxdFlags)) return NULL;

    //Setup vertices for a rectangle centre about the origin of the x-y plane
    #define _fltShVtxCount 4
    GLfloat shVtxs[_fltShVtxCount*3] = {
          w/2.0f,    h/2.0f,  0, //top right corner vertex
        -(w/2.0f),   h/2.0f,  0, //top left corner vertex
        -(w/2.0f), -(h/2.0f), 0, //bottom left corner vertex
          w/2.0f,  -(h/2.0f), 0  //bottom right corner vertex
    };

    //rectangle indexes
    GLuint shIndexes[] = {
        0, 1, 2,
        2, 3, 0
    };

    //Allocate large enough memory that can contain maximum vertex data
    GLfloat shVtxd[_fltShVtxCount*(3/*POS*/ + 3/*NORM*/ + 2/*TEXCOORD*/)];
    
    //Write all vertex data into $shVtxd
    //------------------------------------
        //position data
    if(vtxdFlags & flgmmsbsVTXD_POS){
        memcpy(shVtxd, shVtxs, sizeof(shVtxs));
    }
        //normal data
    //Vertex normals will be automatically computed

        //texture coordinate
    if(vtxdFlags & flgmmsbsVTXD_TEXCOORD){
        //Setup texture coordinate for rectangle using openGL texture coordinate system
        GLfloat shTexCoords[_fltShVtxCount*2/*floats per texture coord*/] = {
            1, 1, //top right corner vertex
            0, 1, //top left corner vertex
            0, 0, //bottom left corner vertex
            1, 0  //bottom right corner vertex
        };
        memcpy(shVtxd+flgmmsbsGetIndex(vtxdFlags, flgmmsbsVTXD_TEXCOORD, _fltShVtxCount),
            shTexCoords, sizeof(shTexCoords));
    }
    //--

    return flgmmsbsNewDU(shVtxd, flgmmsbsGetVtxdLen(vtxdFlags, _fltShVtxCount), _fltShVtxCount,
    vtxdFlags, shIndexes, sizeof(shIndexes)/sizeof(*shIndexes), false);

    #undef _fltShVtxCount
}

//->flgmmsBox
//==========================================================================================
flgmMesh* flgmmsbsBoxNew(GLuint w, GLuint h, GLuint b){
    //Setup vertices for a box centre about the origin(openGL RHS) such that:
    //w is along the x-axis, h is along the y-axis, b is along the z-axis
    #define _fltShVtxCount 24
    GLfloat shVtxs[_fltShVtxCount*3] = {
        //TOP FACE
          w/2.0f ,    h/2.0f,  -(b/2.0f), //top right corner back vertex
        -(w/2.0f),    h/2.0f,  -(b/2.0f), //top left corner back vertex
        -(w/2.0f),    h/2.0f,    b/2.0f , //top left corner front vertex
          w/2.0f ,    h/2.0f,    b/2.0f , //top right corner front vertex
        //BOTTOM FACE
          w/2.0f ,  -(h/2.0f), -(b/2.0f), //bottom right corner back vertex
        -(w/2.0f),  -(h/2.0f), -(b/2.0f), //bottom left corner back vertex
        -(w/2.0f),  -(h/2.0f),   b/2.0f , //bottom left corner front vertex
          w/2.0f ,  -(h/2.0f),   b/2.0f , //bottom right corner front vertex

        //LEFT FACE
        -(w/2.0f),    h/2.0f,    b/2.0f ,
        -(w/2.0f),    h/2.0f,  -(b/2.0f),
        -(w/2.0f),  -(h/2.0f), -(b/2.0f),
        -(w/2.0f),  -(h/2.0f),   b/2.0f ,
        //RIGHT FACE
          w/2.0f ,    h/2.0f,    b/2.0f ,
          w/2.0f ,    h/2.0f,  -(b/2.0f),
          w/2.0f ,  -(h/2.0f), -(b/2.0f),
          w/2.0f ,  -(h/2.0f),   b/2.0f ,

        //BACK FACE
          w/2.0f ,    h/2.0f,  -(b/2.0f),
        -(w/2.0f),    h/2.0f,  -(b/2.0f),
        -(w/2.0f),  -(h/2.0f), -(b/2.0f),
          w/2.0f ,  -(h/2.0f), -(b/2.0f),
        //FRONT FACE
          w/2.0f ,    h/2.0f,    b/2.0f ,
        -(w/2.0f),    h/2.0f,    b/2.0f ,
        -(w/2.0f),  -(h/2.0f),   b/2.0f ,
          w/2.0f ,  -(h/2.0f),   b/2.0f 
    };
    //box indexes
    GLuint shIndexes[] = {
        0,  1,  2,  2,  3,  0,  //TOP FACE
        4,  7,  6,  6,  5,  4,  //BOTTOM FACE
        8,  9,  10, 10, 11, 8, //LEFT FACE
        12, 15, 14, 14, 13, 12, //RIGHT FACE
        16, 19, 18, 18, 17, 16, //BACK FACE 
        20, 21, 22, 22, 23, 20  //FRONT FACE
    };

    //Allocate large enough memory that can contain maximum vertex data
    GLfloat shVtxd[_fltShVtxCount*(3/*POS*/ + 3/*NORM*/)];

    //Write all vertex data into allocated memory
    //------------------------------------
        //position data
    memcpy(shVtxd, shVtxs, sizeof(shVtxs));
        //normal data
    //Vertex normals will be automatically computed

    uint8_t vtxdFlags = flgmmsbsVTXD_POS|flgmmsbsVTXD_NORM;
    return flgmmsbsNewDU(shVtxd, flgmmsbsGetVtxdLen(vtxdFlags, _fltShVtxCount), _fltShVtxCount,
    vtxdFlags, shIndexes, sizeof(shIndexes)/sizeof(*shIndexes), false);

    #undef _fltShVtxCount
}

//->flgmmsMaterial
//==========================================================================================
static flutImplNew(flgmmsmtNew, flgmmsMaterial, flgmmsmt_)

void flgmmsmtFree(flgmmsMaterial* mat){
    if(!mat) return;
    if(mat->_free) mat->_free(mat);
    flmmFree(mat);
}

//->flgmmsmtBasic
//==========================================================================================
flgmmsMaterial* flgmmsmtbsNew(GLuint diffTexID, GLuint specTexID, GLint shine, flmhVector3 clrRGB){
    flgmmsmtBasic* bsMat = flmmMalloc(sizeof(flgmmsmtBasic));
    if(!bsMat) return NULL;

    *bsMat = (flgmmsmtBasic){flgmmsmtbs_, 
        .diffTexID = diffTexID, .specTexID = specTexID, .shine = shine, .color = clrRGB};

    flgmmsMaterial* mat = flgmmsmtNew();
    if(!mat){
        flmmFree(bsMat);
        return NULL;
    }
    mat->_ = bsMat;
    mat->_free = flgmmsmtbsFree;

    return mat;
}

static void flgmmsmtbsFree(flgmmsMaterial* mat){
    if(!mat->_) return;

    flmmFree(mat->_);
    mat->_ = NULL;
}