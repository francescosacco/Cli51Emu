#ifndef _MCSTYPES_
#define _MCSTYPES_

#include <stdint.h>

typedef enum
{
    mcsSuccess = 0 ,
    mcsBadParam    ,
    mcsOutOfMemory ,
    
    mcsError
} mcsRet_t ;

#endif // _MCSTYPES_
