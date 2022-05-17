#include"flfi.h"
#include"fl.h"

const char* flfiRead(const char* filePath, flLog** errlogPD){
    errno = 0;

    FILE* fp = fopen(filePath, "r");

    if(!fp){
        if(errlogPD){
            *errlogPD = fllogNews(3, filePath, ": ", strerror(errno));
        }
        return NULL;
    }

    fseek(fp, 0, SEEK_END);//set the file pointer at the end of the file
    size_t txtfLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* txtfBuf = flmemMalloc(txtfLen*sizeof(char) + 1/*null char*/ + 2/*any other*/);

    //read file
    uint32_t i = 0, tmp;
    while( (tmp = fgetc(fp)) != EOF ){
        *(txtfBuf+i) = (char)tmp;
        i++;
    }
    *(txtfBuf+i) = '\0';

    //check for other errors here
    //if(i != txtfLen)

    //Close the file
    fclose(fp);

    return txtfBuf;
}