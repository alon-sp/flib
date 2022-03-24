
#include"../fl.h"

#include"flEntity.h"

bool flentSetName(flEntity* entP, const char* namestr){
    if(!namestr) namestr = "";

    char* destBuf = flmemRealloc( (char*)entP->name, ( strlen(namestr)+1 )*sizeof(char) );
    if(!destBuf){
        flerrHandle("\nMEMf flentSetName");
        return false;
    }

    strcpy(destBuf, namestr);
    _flentSetName(entP, destBuf);
    return true;
}