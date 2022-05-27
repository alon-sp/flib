#include"flgm.h"

static bool flgmBmeshVtxdIsSupported(uint8_t flags){
    switch(flags){
        case flgmVTXD_POS:
        case flgmVTXD_POS|flgmVTXD_NORM:
        case flgmVTXD_POS|flgmVTXD_TEXCOORD:
        case flgmVTXD_POS|flgmVTXD_CLR:
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD:
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_CLR:
        case flgmVTXD_POS|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
            return true;
        default:
            return false;
    }
}

static uint flgmBmeshGetStride(flgmBmesh* bm){
    switch(bm->vtxdFlags){
        case flgmVTXD_POS:
            return 3;
        case flgmVTXD_POS|flgmVTXD_NORM:
        case flgmVTXD_POS|flgmVTXD_CLR:
            return 3+3;
        case flgmVTXD_POS|flgmVTXD_TEXCOORD:
            return 3+2;
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD:
            return 3+3+2;
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_CLR:
            return 3+3+3;
        case flgmVTXD_POS|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
            return 3+2+3;
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
            return 3+3+2+3;
    }
    return 0;
}

static uint flgmBmeshGetIndex(flgmBmesh* bm, uint8_t dflag){
    switch(bm->vtxdFlags){
        case flgmVTXD_POS:
            break;
        case flgmVTXD_POS|flgmVTXD_NORM:
        case flgmVTXD_POS|flgmVTXD_CLR:
        case flgmVTXD_POS|flgmVTXD_TEXCOORD:
            if(dflag != flgmVTXD_POS) return 3;
            break;
        
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD:
        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_CLR:
            if(dflag == flgmVTXD_NORM) return 3;
            if(dflag != flgmVTXD_POS) return 3+3;
            break;

        case flgmVTXD_POS|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
            if(dflag == flgmVTXD_TEXCOORD) return 3;
            if(dflag == flgmVTXD_CLR) return 3+2;
            break;

        case flgmVTXD_POS|flgmVTXD_NORM|flgmVTXD_TEXCOORD|flgmVTXD_CLR:
            if(dflag == flgmVTXD_NORM) return 3;
            if(dflag == flgmVTXD_TEXCOORD) return 3+3;
            if(dflag == flgmVTXD_CLR) return 3+3+2;
            break;
    }

    return 0;
}

flgmBmesh* flgmBmeshNew(float* vertexData, const uint vertexDataLen, 
    const uint* indexes, const uint indexesLen, uint8_t vtxdFlags, bool saveData){
    
    if( !(vertexData && indexes) ) return NULL;
    if( !flgmBmeshVtxdIsSupported(vtxdFlags) ) return NULL;

    flgmBmesh* bm = flmemMalloc(sizeof(flgmBmesh));

    if(saveData){
        float* vtxdBuf = flmemMalloc(vertexDataLen*sizeof(float));
        memcpy(vtxdBuf, vertexData, vertexDataLen*sizeof(float));

        uint* indxBuf = flmemMalloc(indexesLen*sizeof(uint));
        memcpy(indxBuf, indexes, indexesLen*sizeof(uint));

        _flgmBmeshSetVtxd(bm, vtxdBuf, vertexDataLen, vtxdFlags);
        _flgmBmeshSetIndexes(bm, indxBuf, indexesLen);
    }else{
        _flgmBmeshSetVtxd(bm, vertexData, vertexDataLen, vtxdFlags);
        _flgmBmeshSetIndexes(bm, indexes, indexesLen);
    }

    _flgmBmeshSetTransform(bm, NULL);
    _flgmBmeshSetTransformBuf(bm, NULL);
    bm->mat = (flgmBmeshMat)flgmBmeshMatInit();
    flgmBmeshSetVboID(bm, 0);
    flgmBmeshSetIboID(bm, 0);
    flgmBmeshSetVaoID(bm, 0);

    #define _fltmpFlagIsSet(_bm, dflag) ( (_bm)->vtxdFlags & (dflag) )
    
    if(_fltmpFlagIsSet(bm, flgmVTXD_NORM)){
        flgmComputeVertexNormal(  bm->vtxd, flgmBmeshGetStride(bm), 
            (float*)bm->vtxd + flgmBmeshGetIndex(bm, flgmVTXD_NORM), 
            flgmBmeshGetStride(bm), bm->indexes, bm->indexesLen  );
    }

    //Generate GL buffers
    flgmBmeshSetVboID(bm, 
        flglGenBuffer(GL_ARRAY_BUFFER, bm->vtxdLen * sizeof(*bm->vtxd),
            bm->vtxd, saveData? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
        ));

    flgmBmeshSetIboID(bm, 
        flglGenBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->indexesLen * sizeof(*bm->indexes),
            bm->indexes, GL_STATIC_DRAW
        ));

    //Generate GL vertex Array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    flgmBmeshSetVaoID(bm, vao);

    //Configure GL buffers
    //--------------------
    glBindVertexArray(bm->vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, bm->vboID);
    //position attribute
    if(_fltmpFlagIsSet(bm, flgmVTXD_POS)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXPOS, 3, GL_FLOAT, false, 
            flgmBmeshGetStride(bm)*sizeof(*bm->vtxd), 
            (GLvoid*)(flgmBmeshGetIndex(bm, flgmVTXD_POS)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXPOS);
    }
    //normal attribute
    if(_fltmpFlagIsSet(bm, flgmVTXD_NORM)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXNORM, 3, GL_FLOAT, false, 
            flgmBmeshGetStride(bm)*sizeof(*bm->vtxd), 
            (GLvoid*)(flgmBmeshGetIndex(bm, flgmVTXD_NORM)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXNORM);
    }
    //texture coordinate
    if(_fltmpFlagIsSet(bm, flgmVTXD_TEXCOORD)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXTEXCOORD, 2, GL_FLOAT, false, 
            flgmBmeshGetStride(bm)*sizeof(*bm->vtxd), 
            (GLvoid*)(flgmBmeshGetIndex(bm, flgmVTXD_TEXCOORD)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXTEXCOORD);
    }
    //color
    if(_fltmpFlagIsSet(bm, flgmVTXD_CLR)){
        glVertexAttribPointer(  FLGL_ATTRIBLOC_VTXCLR, 3, GL_FLOAT, false, 
            flgmBmeshGetStride(bm)*sizeof(*bm->vtxd), 
            (GLvoid*)(flgmBmeshGetIndex(bm, flgmVTXD_CLR)*sizeof(*bm->vtxd))  );
        glEnableVertexAttribArray(FLGL_ATTRIBLOC_VTXCLR);
    }
    //bind ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bm->iboID);

    //unbind GL objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if(!saveData){
        _flgmBmeshSetVtxd(bm, NULL, 0, bm->vtxdFlags);
        _flgmBmeshSetIndexes(bm, NULL, bm->indexesLen);//@note: storing the index buffer
            //length despise setting it to NULL is important since it will be use later in
            //drawing the mesh(in call to glDrawElements)
    }
    return bm;

#undef _fltmpFlagIsSet
}

void flgmBmeshFree(flgmBmesh* bm){
    if(!bm) return;

    if(bm->vtxd){
        flmemFree((void*)bm->vtxd);
        _flgmBmeshSetVtxd(bm, NULL, 0, 0);
    }
    if(bm->indexes){
        flmemFree((void*)bm->indexes);
        _flgmBmeshSetIndexes(bm, NULL, 0);
    }

    if(bm->vboID){
        glDeleteBuffers(1, &bm->vboID);
        flgmBmeshSetVboID(bm, 0);
    }
    if(bm->iboID){
        glDeleteBuffers(1, &bm->iboID);
        flgmBmeshSetIboID(bm, 0);
    }
    if(bm->vaoID){
        glDeleteVertexArrays(1, &bm->vaoID);
        flgmBmeshSetVaoID(bm, 0);
    }

    if(bm->_transformBuf){
        flmemFree((void*)bm->_transformBuf);
        _flgmBmeshSetTransformBuf(bm, NULL);
    }
}

void flgmBmeshSetTransform(flgmBmesh* bm, float16* transform, bool saveCopy){
    if(saveCopy){
        if(!bm->_transformBuf) _flgmBmeshSetTransformBuf(bm, flmemMalloc(sizeof(float16)));
        *(float16*)bm->_transformBuf = *transform;
    }
    _flgmBmeshSetTransform(bm, saveCopy?bm->_transformBuf : transform);
}

void flgmBmeshDraw(flgmBmesh* bm, flglShaderProgram shaderProgram){
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
    
    glBindVertexArray(bm->vaoID);
    glDrawElements(GL_TRIANGLES, bm->indexesLen, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
}

void* flgmComputeVertexNormal(
    const float* vtxs, uint vtxsStride, float* normDest, uint normDestStride, 
    const uint* indexes, uint indexesLen  ){
    //check if data is tightly packed
    if(!vtxsStride) vtxsStride = 3;//floats
    if(!normDestStride) normDestStride = 3;//floats

    //Clear $normDest buffer before computing normals
    for(uint i = 0; i<indexesLen; i++){
        * ((Vector3*)(normDest+indexes[i]*normDestStride) ) = Vector3Zero();
    }

    //Loop through each triangle(3 indexes that form a face)
    for(uint i = 0; i<indexesLen; i += 3){
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
    for(uint i = 0; i<indexesLen; i++){
        *( (Vector3*)(normDest+indexes[i]*normDestStride) ) = Vector3Normalize(
                *(Vector3*)(normDest+indexes[i]*normDestStride)  );
    }
}