#ifndef FLUTILHEADERH_INCLUDED
#define FLUTILHEADERH_INCLUDED

#define flutiGetVoidPtrProp(voidPtr, propName, voidPtrType) (\
    ( (voidPtrType*)(voidPtr) )->propName \
)

#define _flutiSetConstValue(constVar, constValue, constType) (\
    *( (constType*)&(constVar) ) = constValue \
)

#endif