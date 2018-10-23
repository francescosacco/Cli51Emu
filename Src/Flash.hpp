#ifndef _FLASH_
#define _FLASH_

#include <stdint.h>
#include "McsTypes.hpp"

class Flash
{
    private :
        uint8_t * program ;
        uint16_t size ;
    
    public :
        Flash() ;
        ~Flash() ;
        
        mcsRet_t load( uint8_t * data , uint16_t sizeIn ) ;
        uint8_t read( uint16_t addr ) ;

} ;

#endif // _FLASH_
