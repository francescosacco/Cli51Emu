#include "Flash.hpp"

#include <iostream>
#include <string.h> // To use memcpy().
#include "McsTypes.hpp"

using namespace std ;

Flash::Flash()
{
    cout << "\t\tFlash Construtor" << endl ;

    program = NULL ;
    size = 0 ;
}

Flash::~Flash()
{
    cout << "\t\tFlash Destrutor" << endl ;
    
    if( program )
    {
        delete[] program ;
        program = NULL ;
        size = 0 ;
    }
}

mcsRet_t Flash::load( uint8_t * data , uint16_t sizeIn )
{
    mcsRet_t ret = mcsError ;
    
    if( program )
    {
        delete[] program ;
        program = NULL ;
    }

    size = sizeIn ;

    if( sizeIn )
    {
        program = new uint8_t[ size ] ;
        if( program )
        {
            ( void ) memcpy( ( void * ) program , ( const void * ) data , ( size_t ) sizeIn ) ;
            ret = mcsSuccess ;
        }
        else
        {
            ret = mcsOutOfMemory ;
        }
    }
    
    return( ret ) ;
}

uint8_t Flash::read( uint16_t addr )
{
    uint8_t ret = 0xFF ;
    
    if( ( program ) && ( addr < size ) )
    {
        ret = program[ addr ] ;
    }
    
    return( ret ) ;
}
