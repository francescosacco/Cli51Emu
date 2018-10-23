#include "Mcs51.hpp"

#include <iostream>
#include "McsTypes.hpp"

using namespace std ;

Mcs51::Mcs51()
{
    mcsRet_t ret ;
    
    cout << "\tMcs51 Construtor" << endl ;

    core = new( Core ) ;
    flash = new( Flash ) ;
    ram = new( Ram ) ;
}

Mcs51::~Mcs51()
{
    delete( ram ) ;
    delete( flash ) ;
    delete( core ) ;
    

    cout << "\tMcs51 Destrutor" << endl ;
}
