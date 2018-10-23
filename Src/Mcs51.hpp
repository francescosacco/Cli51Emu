#ifndef _MCS51_
#define _MCS51_

#include "Core.hpp"
#include "Flash.hpp"
#include "Ram.hpp"

class Mcs51
{
    private :
        Core  * core  ;
        Flash * flash ;
        Ram   * ram ;
    
    public :
        Mcs51() ;
        ~Mcs51() ;

} ;

#endif // _MCS51_
