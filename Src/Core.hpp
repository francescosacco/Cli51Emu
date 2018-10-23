#ifndef _CORE_
#define _CORE_

#include "Flash.hpp"
#include "Ram.hpp"

class Core
{
    private :
        uint16_t pc ;
        
        uint8_t readBankedRegister( registerOffset_t reg , Ram * ramObj ) ;
        void writeBankedRegister( registerOffset_t reg , Ram * ramObj , uint8_t data ) ;
        bool parity( uint8_t in ) ;
    
    public :
        Core() ;
        ~Core() ;
        
        void reset( void ) ;
        void interrupt( void ) ;
        void run( Flash * flashObj , Ram * ramObj ) ;

} ;

#endif // _CORE_
