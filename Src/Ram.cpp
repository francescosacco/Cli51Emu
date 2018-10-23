#include "Ram.hpp"

#include <iostream>
#include <string.h> // To use memset().
#include "McsTypes.hpp"

using namespace std ;

/**********
 *
 *      x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Fx |  B  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ex | ACC |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Dx | PSW |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Cx |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Bx |  P3 |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  IP |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ax |  P2 |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  IE |  -  |  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 9x |  P1 |  -  |  -  |  -  |  -  |  -  |  -  |  -  | SCON| SBUF|  -  |  -  |  -  |  -  |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 8x |  P0 |  SP | DPL | DPH |  -  |  -  |  -  | PCON| TCON| TMOD| TL0 | TL1 | TH0 | TH1 |  -  |  -  |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 7x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  \
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+   \
 * 6x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |    |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+    |
 * 5x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |    +-> General Purpose Registers
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+    |
 * 4x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |    |
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+   /
 * 3x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |  /
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 2x |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     | Bit Addressable Registers
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 1x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 2 ~ Bank 3
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * 0x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 0 ~ Bank 1
 *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *
 **********/

bool Ram::isSFR( uint8_t addr )
{
    uint16_t i , numBlocks ;
    sfrBlock_t * pSfrBlock ;
    bool ret = false ;
    
    pSfrBlock = ( sfrBlock_t * ) sfr ;
    
    numBlocks  = sizeof( sfr_t      ) ;
    numBlocks /= sizeof( sfrBlock_t ) ;
    
    for( i = 0 ; i < numBlocks ; i++ )
    {
        if( ( pSfrBlock + i )->addr == addr )
        {
            ret = true ;
            break ;
        }
    }

    return( ret ) ;
}

bool Ram::isGeneral( uint8_t addr )
{
    bool ret = false ;
    
    if( ( addr & 0x80 ) == 0x00 )
    {
        ret = true ;
    }

    return( ret ) ;
}

void Ram::setSFR( uint8_t addr , uint8_t data )
{
    uint16_t i , numBlocks ;
    sfrBlock_t * pSfrBlock ;
    
    pSfrBlock = ( sfrBlock_t * ) sfr ;
    
    numBlocks  = sizeof( sfr_t      ) ;
    numBlocks /= sizeof( sfrBlock_t ) ;
    
    for( i = 0 ; i < numBlocks ; i++ )
    {
        if( ( pSfrBlock + i )->addr == addr )
        {
            ( pSfrBlock + i )->data = data ;
            break ;
        }
    }
}

uint8_t Ram::getSFR( uint8_t addr )
{
    uint16_t i , numBlocks ;
    sfrBlock_t * pSfrBlock ;
    uint8_t ret = 0xFF ;
    
    pSfrBlock = ( sfrBlock_t * ) sfr ;
    
    numBlocks  = sizeof( sfr_t      ) ;
    numBlocks /= sizeof( sfrBlock_t ) ;
    
    for( i = 0 ; i < numBlocks ; i++ )
    {
        if( ( pSfrBlock + i )->addr == addr )
        {
            ret = ( pSfrBlock + i )->data ;
            break ;
        }
    }
    
    return( ret ) ;
}

Ram::Ram()
{
    cout << "\t\tRam Construtor" << endl ;
    
    memoryGeneral = new( uint8_t[ 0x80 ] ) ;
    sfr = new( sfr_t ) ;
    
    ( void ) memset( ( void * ) sfr , 0x00 , sizeof( sfr_t ) ) ;

    sfr->p0.addr   = SFR_ADDR_P0   ;
    sfr->sp.addr   = SFR_ADDR_SP   ;
    sfr->dpl.addr  = SFR_ADDR_DPL  ;
    sfr->dph.addr  = SFR_ADDR_DPH  ;
    sfr->pcon.addr = SFR_ADDR_PCON ;
    sfr->tcon.addr = SFR_ADDR_TCON ;
    sfr->tmod.addr = SFR_ADDR_TMOD ;
    sfr->tl0.addr  = SFR_ADDR_TL0  ;
    sfr->tl1.addr  = SFR_ADDR_TL1  ;
    sfr->th0.addr  = SFR_ADDR_TH0  ;
    sfr->th1.addr  = SFR_ADDR_TH1  ;
    sfr->p1.addr   = SFR_ADDR_P1   ;
    sfr->scon.addr = SFR_ADDR_SCON ;
    sfr->sbuf.addr = SFR_ADDR_SBUF ;
    sfr->p2.addr   = SFR_ADDR_P2   ;
    sfr->ie.addr   = SFR_ADDR_IE   ;
    sfr->p3.addr   = SFR_ADDR_P3   ;
    sfr->ip.addr   = SFR_ADDR_IP   ;
    sfr->psw.addr  = SFR_ADDR_PSW  ;
    sfr->acc.addr  = SFR_ADDR_ACC  ;
    sfr->b.addr    = SFR_ADDR_B    ;
}

Ram::~Ram()
{
    cout << "\t\tRam Destrutor" << endl ;
    
    delete sfr ;
    delete[] memoryGeneral ;
}

void Ram::write( uint8_t addr , uint8_t data )
{
    bool addrSFR ;
    
    addrSFR = isSFR( addr ) ;
    
    if( addrSFR )
    {
        setSFR( addr , data ) ;
    }
    else
    {
        memoryGeneral[ addr ] = data ;
    }
}

uint8_t Ram::read( uint8_t addr )
{
    bool addrSFR , addrGeneral ;
    uint8_t ret = 0xFF ;
    
    addrSFR = isSFR( addr ) ;
    addrGeneral = isGeneral( addr ) ;
    
    if( addrSFR )
    {
        ret = getSFR( addr ) ;
    }
    else if( addrGeneral )
    {
        ret = memoryGeneral[ addr ] ;
    }

    return( ret ) ;
}

/**********
 *
 *        7     6     5     4     3     2     1     0
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * F0h | F7h | F6h | F5h | F4h | F3h | F2h | F1h | F0h | B
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * E0h | E7h | E6h | E5h | E4h | E3h | E2h | E1h | E0h | ACC
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * D0h | D7h | D6h | D5h | D4h | D3h | D2h | D1h | D0h | PSW
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * B8h | BFh | BEh | BDh | BCh | BBh | BAh | B9h | B8h | IP
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * B0h | B7h | B6h | B5h | B4h | B3h | B2h | B1h | B0h | P3
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * A8h | AFh | AEh | ADh | ACh | ABh | AAh | A9h | A8h | IE
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * A0h | A7h | A6h | A5h | A4h | A3h | A2h | A1h | A0h | P2
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 98h | 9Fh | 9Eh | 9Dh | 9Ch | 9Bh | 9Ah | 99h | 98h | SCON
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 90h | 97h | 96h | 95h | 94h | 93h | 92h | 91h | 90h | P1
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 88h | 8Fh | 8Eh | 8Dh | 8Ch | 8Bh | 8Ah | 89h | 88h | TCON
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 80h | 87h | 86h | 85h | 84h | 83h | 82h | 81h | 80h | P0
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 *
 *        7     6     5     4     3     2     1     0
 * 30h |     |     |     |     |     |     |     |     |
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 2Fh | 7Fh | 7Eh | 7Dh | 7Ch | 7Bh | 7Ah | 79h | 78h |
 * 2Eh | 77h | 76h | 75h | 74h | 73h | 72h | 71h | 70h |
 * 2Dh | 6Fh | 6Eh | 6Dh | 6Ch | 6Bh | 6Ah | 69h | 68h |
 * 2Ch | 67h | 66h | 65h | 64h | 63h | 62h | 61h | 60h |
 * 2Bh | 5Fh | 5Eh | 5Dh | 5Ch | 5Bh | 5Ah | 59h | 58h |
 * 2Ah | 57h | 56h | 55h | 54h | 53h | 52h | 51h | 50h |
 * 29h | 4Fh | 4Eh | 4Dh | 4Ch | 4Bh | 4Ah | 49h | 48h |
 * 28h | 47h | 46h | 45h | 44h | 43h | 42h | 41h | 40h |
 * 27h | 3Fh | 3Eh | 3Dh | 3Ch | 3Bh | 3Ah | 39h | 38h |
 * 26h | 37h | 36h | 35h | 34h | 33h | 32h | 31h | 30h |
 * 25h | 2Fh | 2Eh | 2Dh | 2Ch | 2Bh | 2Ah | 29h | 28h |
 * 24h | 27h | 26h | 25h | 24h | 23h | 22h | 21h | 20h |
 * 23h | 1Fh | 1Eh | 1Dh | 1Ch | 1Bh | 1Ah | 19h | 18h |
 * 22h | 17h | 16h | 15h | 14h | 13h | 12h | 11h | 10h |
 * 21h | 0Fh | 0Eh | 0Dh | 0Ch | 0Bh | 0Ah | 09h | 08h |
 * 20h | 07h | 06h | 05h | 04h | 03h | 02h | 01h | 00h |
 *     +-----+-----+-----+-----+-----+-----+-----+-----+
 * 1Fh |     |     |     |     |     |     |     |     |
 *
 **********/

void Ram::writeBit( uint8_t addr , bool data )
{
    uint8_t addrByte , addrBit , tmp ;
    
    if( addr < 0x80 )
    {
        addrByte = ( addr / 8 ) + 0x20 ;
        addrBit  = addr % 8 ;
    }
    else if( ( addr >= 0x80 ) && ( addr < 0xC0 ) )
    {
        /**********
         *
         *       7     6     5     4     3     2     1     0
         *    +-----+-----+-----+-----+-----+-----+-----+-----+
         *    |     |     |     |     |     |     |     |     |
         *    +-----+-----+-----+-----+-----+-----+-----+-----+
         *     \_____________ _____________/ \_______ _______/
         *                   |                       |
         *                   |                       +--------> Bit Address  - Mask 07h
         *                   +--------------------------------> Byte Address - Mask F8h
         *
         **********/
        
        addrByte = addr & 0xF8 ;
        addrBit  = addr & 0x07 ;
    }
    else
    {
        // There's no address Cxh, or from x8h to xFh.
        if( ( ( addr & 0xF0 ) == 0xC0 ) || ( ( addr & 0x08 ) == 0x80 ) )
        {
            return ;
        }

        addrByte = addr & 0xF0 ;
        addrBit  = addr & 0x07 ;
    }
 
    tmp = read( addrByte ) ;
    if( data )
    {
        tmp |= ( 1 << addrBit ) ;
    }
    else
    {
        tmp &= ~( 1 << addrBit ) ;
    }

    write( addrByte , tmp ) ;
}

bool Ram::readBit( uint8_t addr )
{
    uint8_t addrByte , addrBit , tmp ;
    bool ret = true ;

    if( addr < 0x80 )
    {
        addrByte = ( addr / 8 ) + 0x20 ;
        addrBit  = addr % 8 ;
    }
    else if( ( addr >= 0x80 ) && ( addr < 0xC0 ) )
    {
        /**********
         *
         *       7     6     5     4     3     2     1     0
         *    +-----+-----+-----+-----+-----+-----+-----+-----+
         *    |     |     |     |     |     |     |     |     |
         *    +-----+-----+-----+-----+-----+-----+-----+-----+
         *     \_____________ _____________/ \_______ _______/
         *                   |                       |
         *                   |                       +--------> Bit Address  - Mask 07h
         *                   +--------------------------------> Byte Address - Mask F8h
         *
         **********/
        
        addrByte = addr & 0xF8 ;
        addrBit  = addr & 0x07 ;
    }
    else
    {
        // There's no address Cxh, or from x8h to xFh.
        if( ( ( addr & 0xF0 ) == 0xC0 ) || ( ( addr & 0x08 ) == 0x80 ) )
        {
            return( ret ) ;
        }

        addrByte = addr & 0xF0 ;
        addrBit  = addr & 0x07 ;
    }

    tmp = read( addrByte ) ;

    ret = ( tmp & ( 1 << addrBit ) ) ? ( true ) : ( false ) ;

    return( ret ) ;
}
