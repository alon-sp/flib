#include"_flfil.h"

bool _flfiRunTests(){
    const char* srcfPath = "../../src/file/flfi.c";
    const char* srcf = flfiRead(srcfPath, NULL);
    if(!srcf){
        //Attempt reading the file again, but this time around send a destination pointer
        //to retrieve error log
        flLog* errlog = NULL;
        srcf = flfiRead(srcfPath, &errlog);
        if(fllogStr(errlog)){
            printf("\n%s", fllogStr(errlog));
            fllogFree(errlog);
            errlog = NULL;
        }else return false;
    }

    flfiFree(srcf);
    srcf = NULL;

    return true;
}