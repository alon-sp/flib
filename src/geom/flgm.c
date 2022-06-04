#include"flgm.h"

static bool flgmbmVtxdIsSupported(uint8_t flags){
    switch(flags){
        case flgmbmVTXD_POS:
        case flgmbmVTXD_POS|flgmbmVTXD_NORM:
        case flgmbmVTXD_POS|flgmbmVTXD_TEXCOORD:
        case flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD:
            return true;
        default:
            return false;
    }
}

// static GLuint flgmbmGetStride(uint8_t vtxdFlags){
//     switch(vtxdFlags){
//         case flgmbmVTXD_POS:
//             return 3;
//         case flgmbmVTXD_POS|flgmbmVTXD_NORM:
//         case flgmbmVTXD_POS:
//             return 3+3;
//         case flgmbmVTXD_POS|flgmbmVTXD_TEXCOORD:
//             return 3+2;
//         case flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD:
//             return 3+3+2;
//         case flgmbmVTXD_POS|flgmbmVTXD_NORM:
//             return 3+3+3;
//         case flgmbmVTXD_POS|flgmbmVTXD_TEXCOORD:
//             return 3+2+3;
//         case flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD:
//             return 3+3+2+3;
//     }
//     return 0;
// }

static GLuint flgmbmGetIndex(uint8_t vtxdFlags, uint8_t targetFlag, GLuint vtxCount){
    switch(vtxdFlags){
        case flgmbmVTXD_POS:
            break;
        case flgmbmVTXD_POS|flgmbmVTXD_NORM:
        case flgmbmVTXD_POS|flgmbmVTXD_TEXCOORD:
            if(targetFlag != flgmbmVTXD_POS) return 3*vtxCount;
            break;
        
        case flgmbmVTXD_POS|flgmbmVTXD_NORM|flgmbmVTXD_TEXCOORD:
            if(targetFlag == flgmbmVTXD_NORM) return 3*vtxCount;
            if(targetFlag == flgmbmVTXD_TEXCOORD) return (3*2)*vtxCount;
            break;
    }

    return 0;
}

static GLuint flgmbmGetVtxdLen(uint8_t vtxdFlags, GLuint vtxCount){
    GLuint activeFlagsFloatCount = 0;
    if(vtxdFlags & flgmbmVTXD_POS) activeFlagsFloatCount += 3;
    if(vtxdFlags & flgmbmVTXD_NORM) activeFlagsFloatCount += 3;
    if(vtxdFlags & flgmbmVTXD_TEXCOORD) activeFlagsFloatCount += 2;

    return activeFlagsFloatCount * vtxCount;
}

flgmBasicMesh* flgmbmNewBU(  
    GLfloat* vertexData, const GLuint vertexDataLen, GLuint vertexCount, uint8_t vtxdFlags,
     const GLuint* indexes, const GLuint indexesLen, bool saveData, 
     GLenum vboUsage, GLenum iboUsage ){
    
    if( !(vertexData && indexes && vertexDataLen && indexesLen) ) return NULL;
    if( !flgmbmVtxdIsSupported(vtxdFlags) ) return NULL;

    flgmBasicMesh* bm = flmemMalloc(sizeof(flgmBasicMesh));

    if(saveData){
        GLfloat* vtxdBuf = flmemMalloc(vertexDataLen*sizeof(GLfloat));
        memcpy(vtxdBuf, vertexData, vertexDataLen*sizeof(GLfloat));

        GLuint* indxBuf = flmemMalloc(indexesLen*sizeof(GLuint));
        memcpy(indxBuf, indexes, indexesLen*sizeof(GLuint));

        _flgmbmSetVtxd(bm, vtxdBuf, vertexDataLen, vertexCount, vtxdFlags);
        _flgmbmSetIndexes(bm, indxBuf, indexesLen);
    }else{
        _flgmbmSetVtxd(bm, vertexData, vertexDataLen, vertexCount, vtxdFlags);
        _flgmbmSetIndexes(bm, indexes, indexesLen);
    }

    _flgmbmSetTransform(bm, NULL);
    _flgmbmSetTransformBuf(bm, NULL);
    _flgmbmSetType(bm, flgmbmTYPE_NIL);
    _flgmbmSetColorEnabled(bm, false);
    bm->mat = (flgmbmMat)flgmbmMatInit();
    flgmbmSetVboID(bm, 0);
    flgmbmSetIboID(bm, 0);
    flgmbmSetVaoID(bm, 0);

    #define _fltmpFlagIsSet(_bm, targetFlag) ( (_bm)->vtxdFlags & (targetFlag) )
    
    if(_fltmpFlagIsSet(bm, flgmbmVTXD_NORM)){
        flgmComputeVertexNormal(  bm->vtxd, 0, 
            (GLfloat*)bm->vtxd + flgmbmGetIndex(bm->vtxdFlags, flgmbmVTXD_NORM, bm->vtxCount), 
            0, bm->indexes, bm->indexesLen  );
    }

    //Generate GL buffers
    flgmbmSetVboID( bm, 
        flglGenBuffer(GL_ARRAY_BUFFER, bm->vtxdLen * sizeof(*bm->vtxd), bm->vtxd, vboUsage)  );

    flgmbmSetIboID(bm, 
        flglGenBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->indexesLen * sizeof(*bm->indexes),
            bm->indexes, iboUsage
        ));

    //Generate GL vertex Array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    flgmbmSetVaoID(bm, vao);

    //Configure GL buffers
    //--------------------
    glBindVertexArray(bm->vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, bm->vboID);
    //position attribute
    if(_fltmpFlagIsSet(bm, flgmbmVTXD_POS)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXPOS, 3, GL_FLOAT, false,  0, 
            (GLvoid*)(flgmbmGetIndex(bm->vtxdFlags, flgmbmVTXD_POS, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXPOS);
    }
    //normal attribute
    if(_fltmpFlagIsSet(bm, flgmbmVTXD_NORM)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXNORM, 3, GL_FLOAT, false, 0, 
            (GLvoid*)(flgmbmGetIndex(bm->vtxdFlags, flgmbmVTXD_NORM, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXNORM);
    }
    //texture coordinate
    if(_fltmpFlagIsSet(bm, flgmbmVTXD_TEXCOORD)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXTEXCOORD, 2, GL_FLOAT, false,  0, 
            (GLvoid*)(flgmbmGetIndex(bm->vtxdFlags, flgmbmVTXD_TEXCOORD, bm->vtxCount)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXTEXCOORD);
    }

    //bind ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->iboID);

    //unbind GL objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if(!saveData){
        _flgmbmSetVtxd(bm, NULL, 0, 0, bm->vtxdFlags);
        _flgmbmSetIndexes(bm, NULL, bm->indexesLen);//@note: storing the index buffer
            //length despise setting it to NULL is important since it will be use later in
            //drawing the mesh(in call to glDrawElements)
    }
    return bm;

#undef _fltmpFlagIsSet
}

void flgmbmFree(flgmBasicMesh* bm){
    if(!bm) return;

    if(bm->vtxd){
        flmemFree((void*)bm->vtxd);
        _flgmbmSetVtxd(bm, NULL, 0, 0, 0);
    }
    if(bm->indexes){
        flmemFree((void*)bm->indexes);
        _flgmbmSetIndexes(bm, NULL, 0);
    }

    if(bm->vboID){
        glDeleteBuffers(1, &bm->vboID);
        flgmbmSetVboID(bm, 0);
    }
    if(bm->iboID){
        glDeleteBuffers(1, &bm->iboID);
        flgmbmSetIboID(bm, 0);
    }
    if(bm->vaoID){
        glDeleteVertexArrays(1, &bm->vaoID);
        flgmbmSetVaoID(bm, 0);
    }

    if(bm->_transformBuf){
        flmemFree((void*)bm->_transformBuf);
        _flgmbmSetTransformBuf(bm, NULL);
    }
}

void flgmbmSetTransform(flgmBasicMesh* bm, float16* transform, bool saveCopy){
    if(saveCopy){
        if(!bm->_transformBuf) _flgmbmSetTransformBuf(bm, flmemMalloc(sizeof(float16)));
        *(float16*)bm->_transformBuf = *transform;
    }
    _flgmbmSetTransform(bm, saveCopy?bm->_transformBuf : transform);
}

void flgmbmDraw(flgmBasicMesh* bm, flglShaderProgram shaderProgram){
    if(bm->mat.diffTexID){
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(shaderProgram.ulMatDiff, 0);
        glBindTexture(GL_TEXTURE_2D, bm->mat.diffTexID);
    }
    if(bm->mat.specTexID){
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(shaderProgram.ulMatSpec, 1);
        glBindTexture(GL_TEXTURE_2D, bm->mat.specTexID);
    }

    if(bm->mat.shine >= 0 && shaderProgram.ulMatShine >= 0){
        glUniform1f(shaderProgram.ulMatShine, bm->mat.shine);
    }

    if(bm->transform_ && shaderProgram.ulModel >= 0){
        glUniformMatrix4fv(shaderProgram.ulModel, 1, false, bm->transform_->v);
    }
    if(bm->colorEnabled && shaderProgram.ulClr >= 0){
        glUniform3f(shaderProgram.ulClr, bm->color.x, bm->color.y, bm->color.z);
    }
    
    glBindVertexArray(bm->vaoID);
    glDrawElements(GL_TRIANGLES, bm->indexesLen, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
}



flgmBasicMesh* flgmbmNewRectangle(GLuint w, GLuint h, uint8_t vtxdFlags){
    if(!flgmbmVtxdIsSupported(vtxdFlags)) return NULL;

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
    if(vtxdFlags & flgmbmVTXD_POS){
        memcpy(shVtxd, shVtxs, sizeof(shVtxs));
    }
        //normal data
    //Vertex normals will be automatically computed

        //texture coordinate
    if(vtxdFlags & flgmbmVTXD_TEXCOORD){
        //Setup texture coordinate for rectangle using openGL texture coordinate system
        GLfloat shTexCoords[_fltShVtxCount*2/*floats per texture coord*/] = {
            1, 1, //top right corner vertex
            0, 1, //top left corner vertex
            0, 0, //bottom left corner vertex
            1, 0  //bottom right corner vertex
        };
        memcpy(shVtxd+flgmbmGetIndex(vtxdFlags, flgmbmVTXD_TEXCOORD, _fltShVtxCount),
            shTexCoords, sizeof(shTexCoords));
    }
    //--

    return flgmbmNew(shVtxd, flgmbmGetVtxdLen(vtxdFlags, _fltShVtxCount), _fltShVtxCount,
    vtxdFlags, shIndexes, sizeof(shIndexes)/sizeof(*shIndexes), false);

    #undef _fltShVtxCount
}

flgmBasicMesh* flgmbmNewBox(GLuint w, GLuint h, GLuint b){
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

    uint8_t vtxdFlags = flgmbmVTXD_POS|flgmbmVTXD_NORM;
    return flgmbmNew(shVtxd, flgmbmGetVtxdLen(vtxdFlags, _fltShVtxCount), _fltShVtxCount,
    vtxdFlags, shIndexes, sizeof(shIndexes)/sizeof(*shIndexes), false);

    #undef _fltShVtxCount
}

void* flgmComputeVertexNormal(
    const GLfloat* vtxs, GLuint vtxsStride, GLfloat* normDest, GLuint normDestStride, 
    const GLuint* indexes, GLuint indexesLen  ){
    //check if data is tightly packed
    if(!vtxsStride) vtxsStride = 3;//floats
    if(!normDestStride) normDestStride = 3;//floats

    //Clear $normDest buffer before computing normals
    for(GLuint i = 0; i<indexesLen; i++){
        * ((Vector3*)(normDest+indexes[i]*normDestStride) ) = Vector3Zero();
    }

    //Loop through each triangle(3 indexes that form a face)
    for(GLuint i = 0; i<indexesLen; i += 3){
        //Get handle to the 3 vertexes of the triangle
        const Vector3* p0 = (const Vector3*)(vtxs+indexes[i+0]*vtxsStride);
        const Vector3* p1 = (const Vector3*)(vtxs+indexes[i+1]*vtxsStride);
        const Vector3* p2 = (const Vector3*)(vtxs+indexes[i+2]*vtxsStride);
        //Compute the triangle normal using p0, p1 and p2
        Vector3 tnorm = Vector3CrossProduct(Vector3Subtract(*p1, *p0), Vector3Subtract(*p2, *p0));
        //Update the corresponding normals of the vertexes in $normDest
        *( (Vector3*)(normDest+indexes[i+0]*normDestStride) ) = Vector3Add(
                *(Vector3*)(normDest+indexes[i+0]*normDestStride), tnorm  );
        *( (Vector3*)(normDest+indexes[i+1]*normDestStride) ) = Vector3Add(
                *(Vector3*)(normDest+indexes[i+1]*normDestStride), tnorm  );
        *( (Vector3*)(normDest+indexes[i+2]*normDestStride) ) = Vector3Add(
                *(Vector3*)(normDest+indexes[i+2]*normDestStride), tnorm  );
    }

    //Normalize computed normals in $normDest buffer
    for(GLuint i = 0; i<indexesLen; i++){
        *( (Vector3*)(normDest+indexes[i]*normDestStride) ) = Vector3Normalize(
                *(Vector3*)(normDest+indexes[i]*normDestStride)  );
    }
}