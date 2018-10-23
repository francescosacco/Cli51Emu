#include "Core.hpp"

#include <iostream>
using namespace std ;

uint8_t Core::readBankedRegister( registerOffset_t reg , Ram * ramObj )
{
    uint8_t addr8 , ret , tmp ;
    bankOffset_t bank ;
    
    /**********
     *
     *        7     6     5     4     3     2     1     0
     *     +-----+-----+-----+-----+-----+-----+-----+-----+
     * PSW |  CY |  AC |  F0 | RS1 | RS0 |  OV |  UD |  P  |
     *     +-----+-----+-----+-----+-----+-----+-----+-----+
     *                        \____ ____/
     *                             |
     *                             +----------------------->  Bank  | RS1 | RS0
     *                                                       -------+-----+-----
     *                                                        Bank0 |  0  |  0
     *                                                        Bank1 |  0  |  1
     *                                                        Bank2 |  1  |  0
     *                                                        Bank3 |  1  |  1
     *
     **********/

    tmp = ramObj->read( SFR_ADDR_PSW ) ; ;
    tmp >>= 3 ;
    tmp &= 0x03 ;
    
    bank = ( bankOffset_t ) tmp ;
    
    /**********
     *
     *                            +-----------------------------------------------------------------------> Bank 2
     *                            |                                               +-----------------------> Bank 3
     *      ______________________|______________________   ______________________|______________________
     *     /                                             \ /                                             \
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * 1x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 2 ~ Bank 3
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 0 ~ Bank 1
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *     \______________________ ______________________/ \______________________ ______________________/
     *                            |                                               |
     *                            |                                               +-----------------------> Bank 1
     *                            +-----------------------------------------------------------------------> Bank 0
     *
     **********/
    addr8  = ( uint8_t ) bank ;
    addr8 *= 0x08 ;
    addr8 += ( uint8_t ) reg ;

    ret = ramObj->read( addr8 ) ;
    
    return( ret ) ;
}

void Core::writeBankedRegister( registerOffset_t reg , Ram * ramObj , uint8_t data )
{
    uint8_t addr8 , tmp ;
    bankOffset_t bank ;
    
    /**********
     *
     *        7     6     5     4     3     2     1     0
     *     +-----+-----+-----+-----+-----+-----+-----+-----+
     * PSW |  CY |  AC |  F0 | RS1 | RS0 |  OV |  UD |  P  |
     *     +-----+-----+-----+-----+-----+-----+-----+-----+
     *                        \____ ____/
     *                             |
     *                             +----------------------->  Bank  | RS1 | RS0
     *                                                       -------+-----+-----
     *                                                        Bank0 |  0  |  0
     *                                                        Bank1 |  0  |  1
     *                                                        Bank2 |  1  |  0
     *                                                        Bank3 |  1  |  1
     *
     **********/

    tmp = ramObj->read( SFR_ADDR_PSW ) ; ;
    tmp >>= 3 ;
    tmp &= 0x03 ;
    
    bank = ( bankOffset_t ) tmp ;
    
    /**********
     *
     *                            +-----------------------------------------------------------------------> Bank 2
     *                            |                                               +-----------------------> Bank 3
     *      ______________________|______________________   ______________________|______________________
     *     /                                             \ /                                             \
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * 1x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 2 ~ Bank 3
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 |  R0 |  R1 |  R2 |  R3 |  R4 |  R5 |  R6 |  R7 | Bank 0 ~ Bank 1
     *    +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     *     \______________________ ______________________/ \______________________ ______________________/
     *                            |                                               |
     *                            |                                               +-----------------------> Bank 1
     *                            +-----------------------------------------------------------------------> Bank 0
     *
     **********/
    addr8  = ( uint8_t ) bank ;
    addr8 *= 0x08 ;
    addr8 += ( uint8_t ) reg ;

    ramObj->write( addr8 , data ) ;
}

Core::Core()
{
    cout << "\t\tCore Construtor" << endl ;
    pc = 0x0000 ;
}

Core::~Core()
{
    cout << "\t\tCore Destrutor" << endl ;
}

void Core::reset( void )
{
    pc = 0x0000 ;
}

void Core::interrupt( void )
{
    
}

void Core::run( Flash * flashObj , Ram * ramObj )
{
    uint16_t addr16 , tmp16 ;
    uint8_t addr8 , acc , tmp8 , opcode ;
    registerOffset_t reg ;
    bool tmpBit ;
    
    opcode = flashObj->read( pc ) ;
    switch( opcode )
    {
    // NOP.
    case 0x00 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        pc++ ;
        break ;
    // LJMP addr16
    case 0x02 :
        /**********
         *
         *           17    16    15    14    13    12    11    10    0F    0E    0D    0C    0B    0A    09    08    07    06    05    04    03    02    01    00
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  1  |  0  | A15 | A14 | A13 | A12 | A11 | A10 |  A9 |  A8 |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr16  = ( uint16_t ) flashObj->read( pc + 1 ) ;
        addr16 <<= 8 ;
        addr16 |= ( uint16_t ) flashObj->read( pc + 2 ) ;
        pc = addr16 ;
        break ;
    // RR A
    case 0x03 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        acc = ( acc >> 1 ) | ( acc << 7 ) ;
        ramObj->write( SFR_ADDR_ACC , acc ) ;
        
        pc++ ;
        break ;
    // INC A
    case 0x04 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        acc++ ;
        ramObj->write( SFR_ADDR_ACC , acc ) ;
        
        pc++ ;
        break ;
    // INC addr8
    case 0x05 :
        /**********
         *
         *           F     E     D     C     B     A     9     8     7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr8 = flashObj->read( pc + 1 ) ;
        tmp8 = ramObj->read( addr8 ) ;
        tmp8++ ;
        ramObj->write( addr8 , tmp8 ) ;
        
        pc += 2 ;
        break ;
    // INC @Rx
    case 0x06 :
    case 0x07 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp8 = ramObj->read( addr8 ) ;
        tmp8++ ;
        ramObj->write( addr8 , tmp8 ) ;
    
        pc++ ;
        break ;
    // INC Rx
    case 0x08 :
    case 0x09 :
    case 0x0A :
    case 0x0B :
    case 0x0C :
    case 0x0D :
    case 0x0E :
    case 0x0F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
    
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;
        // Increment the value.
        tmp8++ ;
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp8 ) ;

        pc++ ;
        break ;
    // JBC bitAddr8 , offset8
    case 0x10 :
        /**********
         *
         *           17    16    15    14    13    12    11    10    0F    0E    0D    0C    0B    0A    09    08    07    06    05    04    03    02    01    00
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  0  |  0  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |  O7 |  O6 |  O5 |  o4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr8  = flashObj->read( pc + 1 ) ;
        addr16 = ( uint16_t ) flashObj->read( pc + 2 ) ;
        
        pc += 3 ;

        tmpBit = ramObj->readBit( addr8 ) ;
        if( tmpBit )
        {
            ramObj->writeBit( addr8 , false ) ;
            pc += addr16 ;
        }
        break ;
    // LCALL addr16
    case 0x12 :
        /**********
         *
         *           17    16    15    14    13    12    11    10    0F    0E    0D    0C    0B    0A    09    08    07    06    05    04    03    02    01    00
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  1  |  0  | A15 | A14 | A13 | A12 | A11 | A10 |  A9 |  A8 |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr16   = ( uint16_t ) flashObj->read( pc + 1 ) ;
        addr16 <<= 3 ;
        addr16  |= ( uint16_t ) flashObj->read( pc + 2 ) ;
        
        pc += 3 ;

        addr8 =ramObj->read( SFR_ADDR_SP ) ;
        ramObj->write( ++addr8 , ( uint8_t ) pc ) ;
        ramObj->write( ++addr8 , ( uint8_t ) ( pc >> 8 ) ) ;
        ramObj->write( SFR_ADDR_SP , addr8 ) ;

        pc = addr16 ;    
        break ;
    case 0x13 :
        break ;
    // DEC A
    case 0x14 :
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        acc-- ;
        ramObj->write( SFR_ADDR_ACC , acc ) ;
        
        pc++ ;
        break ;
    // DEC direct
    case 0x15 :
        addr8 = flashObj->read( pc + 1 ) ;
        tmp8 = ramObj->read( addr8 ) ;
        tmp8-- ;
        ramObj->write( addr8 , tmp8 ) ;
        
        pc += 2 ;
        break ;
    // DEC @Rx
    case 0x16 :
    case 0x17 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp8 = ramObj->read( addr8 ) ;
        tmp8-- ;
        ramObj->write( addr8 , tmp8 ) ;
    
        pc++ ;
        break ;
    // DEC Rx
    case 0x18 :
    case 0x19 :
    case 0x1A :
    case 0x1B :
    case 0x1C :
    case 0x1D :
    case 0x1E :
    case 0x1F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;
        // Decrement the value.
        tmp8-- ;
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp8 ) ;
    
        pc++ ;
        break ;
    // JB bitAddr,offset
    case 0x20 :
        addr8  = flashObj->read( pc + 1 ) ;
        addr16 = ( uint16_t ) flashObj->read( pc + 2 ) ;
        
        pc += 3 ;

        tmpBit = ramObj->readBit( addr8 ) ;
        if( tmpBit )
        {
            pc += addr16 ;
        }
        break ;
    // RET
    case 0x22 :
        addr8 =ramObj->read( SFR_ADDR_SP ) ;
        addr16   = ( uint16_t ) ramObj->read( addr8-- ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) ramObj->read( addr8-- ) ;
        ramObj->write( SFR_ADDR_SP , addr8 ) ;

        pc = addr16 ;    
        break ;
    case 0x23 :
        break ;
    case 0x24 :
        break ;
    case 0x25 :
        break ;
    case 0x26 :
        break ;
    case 0x27 :
        break ;
    case 0x28 :
        break ;
    case 0x29 :
        break ;
    case 0x2A :
        break ;
    case 0x2B :
        break ;
    case 0x2C :
        break ;
    case 0x2D :
        break ;
    case 0x2E :
        break ;
    case 0x2F :
        break ;
    case 0x30 :
        break ;
    case 0x32 :
        break ;
    case 0x33 :
        break ;
    case 0x34 :
        break ;
    case 0x35 :
        break ;
    case 0x36 :
        break ;
    case 0x37 :
        break ;
    case 0x38 :
        break ;
    case 0x39 :
        break ;
    case 0x3A :
        break ;
    case 0x3B :
        break ;
    case 0x3C :
        break ;
    case 0x3D :
        break ;
    case 0x3E :
        break ;
    case 0x3F :
        break ;
    case 0x40 :
        break ;
    // ORL address,A
    case 0x42 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  0  |  1  |  0  |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp8 = ramObj->read( addr8 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        tmp8 = tmp8 | acc ;
        // Write data at the address.
        ramObj->write( addr8 , tmp8 ) ;
    
        pc += 2 ;
        break ;
    // ORL address,#data
    case 0x43 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  0  |  1  |  1  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp8 = ramObj->read( addr8 ) ;
        // Execute OR logic.
        tmp8 = tmp8 | flashObj->read( pc + 2 ) ;
        // Write data at the address.
        ramObj->write( addr8 , tmp8 ) ;
    
        pc += 3 ;
        break ;
    // ORL A,#DATA
    case 0x44 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  0  |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read data.
        tmp8 = flashObj->read( pc + 1 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        acc = acc | tmp8 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , acc ) ;
    
        pc += 2 ;
        break ;
    // ORL A,address
    case 0x45 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  1  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp8 = ramObj->read( addr8 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        acc = acc | tmp8 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , acc ) ;
    
        pc += 2 ;
        break ;
    // ORL A,@Rx
    case 0x46 :
    case 0x47 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp8 = ramObj->read( addr8 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute OR operation.
        acc = acc | tmp8 ;

        ramObj->write( SFR_ADDR_ACC , acc ) ;
    
        pc++ ;
        break ;
    // ORL A,Rx
    case 0x48 :
    case 0x49 :
    case 0x4A :
    case 0x4B :
    case 0x4C :
    case 0x4D :
    case 0x4E :
    case 0x4F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute OR logic.
        acc = acc | tmp8 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , acc ) ;

        pc++ ;
        break ;
    case 0x50 :
        break ;
    case 0x52 :
        break ;
    case 0x53 :
        break ;
    case 0x54 :
        break ;
    case 0x55 :
        break ;
    // ANL A,@Rx
    case 0x56 :
    case 0x57 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  1  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp8 = ramObj->read( addr8 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute AND operation.
        acc = acc & tmp8 ;

        ramObj->write( SFR_ADDR_ACC , acc ) ;
    
        pc++ ;
        break ;
    // ANL A,Rx
    case 0x58 :
    case 0x59 :
    case 0x5A :
    case 0x5B :
    case 0x5C :
    case 0x5D :
    case 0x5E :
    case 0x5F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  1  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute AND logic.
        acc = acc & tmp8 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , acc ) ;

        pc++ ;
        break ;
    case 0x60 :
        break ;
    case 0x62 :
        break ;
    case 0x63 :
        break ;
    case 0x64 :
        break ;
    case 0x65 :
        break ;
    // XRL A,@Rx
    case 0x66 :
    case 0x67 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp8 = ramObj->read( addr8 ) ;
        // Read ACC.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute XOR operation.
        acc = acc ^ tmp8 ;

        ramObj->write( SFR_ADDR_ACC , acc ) ;
    
        pc++ ;
        break ;
    // XRL A,Rx
    case 0x68 :
    case 0x69 :
    case 0x6A :
    case 0x6B :
    case 0x6C :
    case 0x6D :
    case 0x6E :
    case 0x6F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
    
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;

        // Read ACC value.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute XOR logic.
        acc = acc ^ tmp8 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , acc ) ;

        pc++ ;
        break ;
    case 0x70 :
        break ;
    case 0x72 :
        break ;
    case 0x73 :
        break ;
    case 0x74 :
        break ;
    case 0x75 :
        break ;
    // MOV @Rx,#data
    case 0x76 :
    case 0x77 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  1  |  x  |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;

        // Read constant value.
        tmp8 = flashObj->read( pc + 1 ) ;

        ramObj->write( addr8 , tmp8 ) ;
    
        pc += 2 ;
        break ;
    // MOV Rx,#DATA
    case 0x78 :
    case 0x79 :
    case 0x7A :
    case 0x7B :
    case 0x7C :
    case 0x7D :
    case 0x7E :
    case 0x7F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  1  |  1  |  x  |  x  |  x  |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
    
        // Read constant value.
        tmp8 = flashObj->read( pc + 1 ) ;
        
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp8 ) ;

        pc += 2 ;
        break ;
    case 0x80 :
        break ;
    case 0x82 :
        break ;
    case 0x83 :
        break ;
    case 0x84 :
        break ;
    case 0x85 :
        break ;
    case 0x86 :
        break ;
    case 0x87 :
        break ;
    case 0x88 :
        break ;
    case 0x89 :
        break ;
    case 0x8A :
        break ;
    case 0x8B :
        break ;
    case 0x8C :
        break ;
    case 0x8D :
        break ;
    case 0x8E :
        break ;
    case 0x8F :
        break ;
    case 0x90 :
        break ;
    case 0x92 :
        break ;
    case 0x93 :
        break ;
    case 0x94 :
        break ;
    case 0x95 :
        break ;
    case 0x96 :
        break ;
    case 0x97 :
        break ;
    case 0x98 :
        break ;
    case 0x99 :
        break ;
    case 0x9A :
        break ;
    case 0x9B :
        break ;
    case 0x9C :
        break ;
    case 0x9D :
        break ;
    case 0x9E :
        break ;
    case 0x9F :
        break ;
    case 0xA0 :
        break ;
    case 0xA2 :
        break ;
    // INC DPTR
    case 0xA3 :
        tmp16  = ( uint16_t ) ramObj->read( SFR_ADDR_DPH ) ;
        tmp16 <<= 8 ;
        tmp16 |= ( uint16_t ) ramObj->read( SFR_ADDR_DPL ) ;
        tmp16++ ;
        ramObj->write( SFR_ADDR_DPH , ( uint8_t ) ( tmp16 >> 8 ) ) ;
        ramObj->write( SFR_ADDR_DPL , ( uint8_t )   tmp16        ) ;
        
        pc++ ;
        break ;
    case 0xA4 :
        break ;
    // Reserved!
    case 0xA5 :
        pc++ ;
        break ;
    case 0xA6 :
        break ;
    case 0xA7 :
        break ;
    case 0xA8 :
        break ;
    case 0xA9 :
        break ;
    case 0xAA :
        break ;
    case 0xAB :
        break ;
    case 0xAC :
        break ;
    case 0xAD :
        break ;
    case 0xAE :
        break ;
    case 0xAF :
        break ;
    case 0xB0 :
        break ;
    case 0xB2 :
        break ;
    case 0xB3 :
        break ;
    case 0xB4 :
        break ;
    case 0xB5 :
        break ;
    case 0xB6 :
        break ;
    case 0xB7 :
        break ;
    case 0xB8 :
        break ;
    case 0xB9 :
        break ;
    case 0xBA :
        break ;
    case 0xBB :
        break ;
    case 0xBC :
        break ;
    case 0xBD :
        break ;
    case 0xBE :
        break ;
    case 0xBF :
        break ;
    case 0xC0 :
        break ;
    case 0xC2 :
        break ;
    case 0xC3 :
        break ;
    case 0xC4 :
        break ;
    case 0xC5 :
        break ;
    case 0xC6 :
        break ;
    case 0xC7 :
        break ;
    // XCH A,Rx
    case 0xC8 :
    case 0xC9 :
    case 0xCA :
    case 0xCB :
    case 0xCC :
    case 0xCD :
    case 0xCE :
    case 0xCF :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  1  |  0  |  0  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp8 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        acc = ramObj->read( SFR_ADDR_ACC ) ;

        // Save Register value in the ACC.
        ramObj->write( SFR_ADDR_ACC , tmp8 ) ;

        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , acc ) ;

        pc++ ;
        break ;
    case 0xD0 :
        break ;
    case 0xD2 :
        break ;
    case 0xD3 :
        break ;
    case 0xD4 :
        break ;
    case 0xD5 :
        break ;
    case 0xD6 :
        break ;
    case 0xD7 :
        break ;
    case 0xD8 :
        break ;
    case 0xD9 :
        break ;
    case 0xDA :
        break ;
    case 0xDB :
        break ;
    case 0xDC :
        break ;
    case 0xDD :
        break ;
    case 0xDE :
        break ;
    case 0xDF :
        break ;
    case 0xE0 :
        break ;
    // AJMP address
    case 0x01 :
    case 0x21 :
    case 0x41 :
    case 0x61 :
    case 0x81 :
    case 0xA1 :
    case 0xC1 :
    case 0xE1 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode | A10 |  A9 |  A8 |  0  |  0  |  0  |  0  |  1  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *         \_______ _______/                               \______________________ ______________________/
         *                 |                                                              |
         *                 +--------> Address <-------------------------------------------+
         *
         **********/

        addr16 = ( uint16_t ) opcode ;
        addr16 <<= 3 ;
        addr16 |= ( uint16_t ) flashObj->read( pc + 1 ) ;

        pc += 2 ;
        
        pc &= 0xF800 ;
        pc |= addr16 ;

        break ;
    case 0xE2 :
        break ;
    case 0xE3 :
        break ;
    case 0xE4 :
        break ;
    case 0xE5 :
        break ;
    case 0xE6 :
        break ;
    case 0xE7 :
        break ;
    // MOV A,Rx
    case 0xE8 :
    case 0xE9 :
    case 0xEA :
    case 0xEB :
    case 0xEC :
    case 0xED :
    case 0xEE :
    case 0xEF :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  1  |  1  |  0  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        acc = readBankedRegister( reg , ramObj ) ;
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , acc ) ;

        pc++ ;
        break ;
    case 0xF0 :
        break ;
    // ACALL address
    case 0x11 :
    case 0x31 :
    case 0x51 :
    case 0x71 :
    case 0x91 :
    case 0xB1 :
    case 0xD1 :
    case 0xF1 :

        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode | A10 |  A9 |  A8 |  1  |  0  |  0  |  0  |  1  |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
         *         \_______ _______/                               \______________________ ______________________/
         *                 |                                                              |
         *                 +--------> Address <-------------------------------------------+
         *
         **********/

        addr16 = ( uint16_t ) opcode ;
        addr16 <<= 3 ;
        addr16 |= ( uint16_t ) flashObj->read( pc + 1 ) ;

        pc += 2 ;
        
        addr8 =ramObj->read( SFR_ADDR_SP ) ;
        ramObj->write( ++addr8 , ( uint8_t ) pc ) ;
        ramObj->write( ++addr8 , ( uint8_t ) ( pc >> 8 ) ) ;
        ramObj->write( SFR_ADDR_SP , addr8 ) ;
        
        pc &= 0xF800 ;
        pc |= addr16 ;

        break ;
    case 0xF2 :
        break ;
    case 0xF3 :
        break ;
    case 0xF4 :
        break ;
    case 0xF5 :
        break ;
    case 0xF6 :
        break ;
    case 0xF7 :
        break ;
    // MOV Rx,A
    case 0xF8 :
    case 0xF9 :
    case 0xFA :
    case 0xFB :
    case 0xFC :
    case 0xFD :
    case 0xFE :
    case 0xFF :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  1  |  1  |  1  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                       \_______ _______/
         *                                               |
         *                                               +--------> Register
         *
         **********/
    
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        
        // Read ACC value.
        acc = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , acc ) ;

        pc++ ;
        break ;
    }
}
