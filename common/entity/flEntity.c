
#include"../fl.h"

#include"flEntity.h"

bool flentSetName(flEntity* ent, const char* namestr){
    if(!namestr) namestr = "";

    char* destBuf = flmemRealloc( (char*)ent->name, ( strlen(namestr)+1 )*sizeof(char) );
    if(!destBuf){
        flerrHandle("\nMEMf flentSetName");
        return false;
    }

    strcpy(destBuf, namestr);
    _flentSetName(ent, destBuf);
    return true;
}