#ifndef _RAM_
#define _RAM_

#include <stdint.h>
#include "McsTypes.hpp"

#define SFR_ADDR_P0                              ( ( uint8_t ) 0x80 )
#define SFR_ADDR_SP                              ( ( uint8_t ) 0x81 )
#define SFR_ADDR_DPL                             ( ( uint8_t ) 0x82 )
#define SFR_ADDR_DPH                             ( ( uint8_t ) 0x83 )
#define SFR_ADDR_PCON                            ( ( uint8_t ) 0x87 )
#define SFR_ADDR_TCON                            ( ( uint8_t ) 0x88 )
#define SFR_ADDR_TMOD                            ( ( uint8_t ) 0x89 )
#define SFR_ADDR_TL0                             ( ( uint8_t ) 0x8A )
#define SFR_ADDR_TL1                             ( ( uint8_t ) 0x8B )
#define SFR_ADDR_TH0                             ( ( uint8_t ) 0x8C )
#define SFR_ADDR_TH1                             ( ( uint8_t ) 0x8D )
#define SFR_ADDR_P1                              ( ( uint8_t ) 0x90 )
#define SFR_ADDR_SCON                            ( ( uint8_t ) 0x98 )
#define SFR_ADDR_SBUF                            ( ( uint8_t ) 0x99 )
#define SFR_ADDR_P2                              ( ( uint8_t ) 0xA0 )
#define SFR_ADDR_IE                              ( ( uint8_t ) 0xA8 )
#define SFR_ADDR_P3                              ( ( uint8_t ) 0xB0 )
#define SFR_ADDR_IP                              ( ( uint8_t ) 0xB8 )
#define SFR_ADDR_PSW                             ( ( uint8_t ) 0xD0 )
#define SFR_ADDR_ACC                             ( ( uint8_t ) 0xE0 )
#define SFR_ADDR_B                               ( ( uint8_t ) 0xF0 )

#define SFR_ADDR_PSW_C                           ( ( uint8_t ) 0xD7 )
#define SFR_ADDR_PSW_AC                          ( ( uint8_t ) 0xD6 )
#define SFR_ADDR_PSW_F0                          ( ( uint8_t ) 0xD5 )
#define SFR_ADDR_PSW_RS1                         ( ( uint8_t ) 0xD4 )
#define SFR_ADDR_PSW_RS0                         ( ( uint8_t ) 0xD3 )
#define SFR_ADDR_PSW_OV                          ( ( uint8_t ) 0xD2 )
#define SFR_ADDR_PSW_P                           ( ( uint8_t ) 0xD0 )

typedef struct _SFRBLOCK_T_
{
    uint8_t addr ;
    uint8_t data ;
} sfrBlock_t ;

typedef struct _SFR_T_
{
    sfrBlock_t p0   ;
    sfrBlock_t sp   ;
    sfrBlock_t dpl  ;
    sfrBlock_t dph  ;
    sfrBlock_t pcon ;
    sfrBlock_t tcon ;
    sfrBlock_t tmod ;
    sfrBlock_t tl0  ;
    sfrBlock_t tl1  ;
    sfrBlock_t th0  ;
    sfrBlock_t th1  ;
    sfrBlock_t p1   ;
    sfrBlock_t scon ;
    sfrBlock_t sbuf ;
    sfrBlock_t p2   ;
    sfrBlock_t ie   ;
    sfrBlock_t p3   ;
    sfrBlock_t ip   ;
    sfrBlock_t psw  ;
    sfrBlock_t acc  ;
    sfrBlock_t b    ;
} sfr_t ;

typedef enum _REGISTER_OFFSET_
{
    offsetR0 = 0 ,
    offsetR1 ,
    offsetR2 ,
    offsetR3 ,
    offsetR4 ,
    offsetR5 ,
    offsetR6 ,
    offsetR7
} registerOffset_t ;

typedef enum _BANK_OFFSET_
{
    offsetBank0 = 0 ,
    offsetBank1 ,
    offsetBank2 ,
    offsetBank3
} bankOffset_t ;
 
class Ram
{
    private :
    
        bool isSFR( uint8_t addr ) ;
        bool isGeneral( uint8_t addr ) ;
        
        void setSFR( uint8_t addr , uint8_t data ) ;
        uint8_t getSFR( uint8_t addr ) ;
        
        uint8_t * memoryGeneral ;
        sfr_t * sfr ;
        
    public :
        Ram() ;
        ~Ram() ;
        
        void write( uint8_t addr , uint8_t data ) ;
        uint8_t read( uint8_t addr ) ;

        void writeBit( uint8_t addr , bool data ) ;
        bool readBit( uint8_t addr ) ;
} ;

#endif // _RAM_
