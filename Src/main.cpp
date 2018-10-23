#include <iostream>
#include "Mcs51.hpp"

using namespace std ;

int main( int argc , char * argv[] )
{
    cout << "Cli51Emu - 8051 Emulator - Ver.: 0.0 - Oct/2018" << endl << endl ;
    
    Mcs51 * mcs51 = new( Mcs51 ) ;
    
    
    
    
    
    delete( mcs51 ) ;

    return( 0 ) ;
}
