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

    tmp = ramObj->read( SFR_ADDR_PSW ) ;
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

bool Core::parity( uint8_t in )
{
    uint8_t parity = in ;
    bool ret ;

    parity ^= ( ( parity >> 4 ) | ( parity << 4 ) ) ;
    parity ^=   ( parity >> 2 ) ;
    parity ^=   ( parity >> 1 ) ;

    ret = ( parity & 1 ) ? ( true ) : ( false ) ;
    return( ret ) ;
}

bool Core::isBitSet( uint8_t data , uint8_t bit )
{
    uint8_t mask ;
    bool ret ;
    
    mask = 0x01 ;
    mask <<= bit ;
    
    ret = ( data & mask ) ? ( true ) : ( false ) ;
    
    return( ret ) ;
}

bool Core::isBitSet( uint16_t data , uint8_t bit )
{
    uint16_t mask ;
    bool ret ;
    
    mask = 0x0001 ;
    mask <<= bit ;
    
    ret = ( data & mask ) ? ( true ) : ( false ) ;
    
    return( ret ) ;
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
    /**********
     *
     *  - -- --- ---------------------------------------------- --- -- -
     *              DATA BUS
     *  - -- --- ----+ +--------+ +---------/ \---------/ \---- --- -- - 
     *               | |        | |        /   \       /   \
     *             +-+ +-+    +-+ +-+     +-+ +-+     +-+ +-+
     *              \   /      \   /        | |         | |
     *            +--\ /--+  +--\ /--+      | |         | |
     *            | TMP 1 |  | TMP 2 |      | |       +-+ +-+
     *            +--+ +--+  +--+ +--+      | |        \   /
     *               | |        | |         | |    +----\ /----+
     *            +--+ +--+  +--+ +--+      | |    | SFR | RAM |
     *             \       \/       /       | |    +-----+-----+
     *              \     ALU      /        | |    | ACC |
     *               \            /         | |    |  B  |
     *                +---+  +---+          | |    | PSW |
     *                    |  |              | |    +-----+
     *                    |  +--------------+ |
     *                    +-------------------+
     *
     **********/
    
    uint16_t addr16 , tmp16 ;
    uint8_t addr8 , tmp1 , tmp2 , opcode ;
    registerOffset_t reg ;
    bool tmpBit , unsignedBit ;
    
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
         * Flags: None.
         *
         **********/
        pc++ ;
        break ;
    // LJMP addr16
    case 0x02 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *    MSB | A15 | A14 | A13 | A12 | A11 | A10 |  A9 |  A8 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *    LSB |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr16   = ( uint16_t ) flashObj->read( pc + 1 ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) flashObj->read( pc + 2 ) ;
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
         * Flags: None.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmp1 = ( tmp1 >> 1 ) | ( tmp1 << 7 ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
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
         * Flags: Parity.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmp1++ ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // INC addr8
    case 0x05 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr8 = flashObj->read( pc + 1 ) ;
        tmp1 = ramObj->read( addr8 ) ;
        tmp1++ ;
        ramObj->write( addr8 , tmp1 ) ;
        
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
         * Flags: None.
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;

        // Increment the variable pointed from the register.
        tmp1 = ramObj->read( addr8 ) ;
        tmp1++ ;
        ramObj->write( addr8 , tmp1 ) ;
    
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
         * Flags: None.
         *
         **********/
    
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp1 = readBankedRegister( reg , ramObj ) ;
        // Increment the value.
        tmp1++ ;
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp1 ) ;

        pc++ ;
        break ;
    // JBC bitAddr8 , offset8
    case 0x10 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr8  = flashObj->read( pc + 1 ) ; // Read bit address.
        addr16 = ( uint16_t ) flashObj->read( pc + 2 ) ; // Read offset.
        
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
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *    MSB | A15 | A14 | A13 | A12 | A11 | A10 |  A9 |  A8 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *    LSB |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr16   = ( uint16_t ) flashObj->read( pc + 1 ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) flashObj->read( pc + 2 ) ;
        
        pc += 3 ;

        addr8 =ramObj->read( SFR_ADDR_SP ) ;
        ramObj->write( ++addr8 , ( uint8_t )   pc                                                                                                       ) ;
        ramObj->write( ++addr8 , ( uint8_t ) ( pc >> 8 ) ) ;
        ramObj->write( SFR_ADDR_SP , addr8 ) ;

        pc = addr16 ;    
        break ;
    // RRC A
    case 0x13 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         *
         *              7     6     5     4     3     2     1     0
         *           +-----+-----+-----+-----+-----+-----+-----+-----+     +-----+
         * ACC +-->--+  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 +-->--+  C  +-->--+
         *     |     +-----+-----+-----+-----+-----+-----+-----+-----+     +-----+     |
         *     |                                                                       |
         *     +-----<-----<-----<-----<-----<-----<-----<-----<-----<-----<-----<-----+
         *
         * Flags: Parity
         *        Carry
         *
         **********/
        tmp1   = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        
        tmp2 = tmp1 ;
        tmp1 = ( tmp1 >> 1 ) | ( ( tmpBit ) ? ( 0x80 ) : ( 0x00 ) ) ;
        
        tmpBit = isBitSet( tmp2 , 0 ) ; // Check bit 0.
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // DEC A
    case 0x14 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: Parity
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmp1-- ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // DEC direct
    case 0x15 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr8 = flashObj->read( pc + 1 ) ;
        tmp1 = ramObj->read( addr8 ) ;
        tmp1-- ;
        ramObj->write( addr8 , tmp1 ) ;
        
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
         * Flags: None.
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp1 = ramObj->read( addr8 ) ;
        tmp1-- ;
        ramObj->write( addr8 , tmp1 ) ;
    
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
         * Flags: None.
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp1 = readBankedRegister( reg , ramObj ) ;
        // Decrement the value.
        tmp1-- ;
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp1 ) ;
    
        pc++ ;
        break ;
    // JB bitAddr,offset
    case 0x20 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  0  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
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
    // RETI
    case 0x32 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  x  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/
         *                             |
         *                             +--------------------------> RET (0) or RETI (1)
         *
         * Flags: None.
         *
         **********/
        addr8 = ramObj->read( SFR_ADDR_SP ) ;

        addr16   = ( uint16_t ) ramObj->read( addr8-- ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) ramObj->read( addr8-- ) ;

        ramObj->write( SFR_ADDR_SP , addr8 ) ;
        pc = addr16 ;    
        break ;
    // RL A
    case 0x23 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  0  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         *
         *              7     6     5     4     3     2     1     0
         *           +-----+-----+-----+-----+-----+-----+-----+-----+
         * ACC +--<--+  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 +--<--+
         *     |     +-----+-----+-----+-----+-----+-----+-----+-----+     |
         *     |                                                           |
         *     +----->----->----->----->----->----->----->----->----->-----+
         *
         * Flags: None.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmp1 = ( tmp1 << 1 ) | ( tmp1 >> 7 ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        pc++ ;
        break ;
    // JNB bitAddr8 , offset8
    case 0x30 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr8  = flashObj->read( pc + 1 ) ;
        addr16 = ( uint16_t ) flashObj->read( pc + 2 ) ;
        
        pc += 3 ;

        tmpBit = ramObj->readBit( addr8 ) ;
        if( tmpBit == false )
        {
            pc += addr16 ;
        }
        break ;
    // RLC A
    case 0x33 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         *
         *              7     6     5     4     3     2     1     0
         *           +-----+-----+-----+-----+-----+-----+-----+-----+     +-----+
         * ACC +--<--+  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 +--<--+  C  +--<--+
         *     |     +-----+-----+-----+-----+-----+-----+-----+-----+     +-----+     |
         *     |                                                                       |
         *     +----->----->----->----->----->----->----->----->----->----->----->-----+
         *
         * Flags: Parity
         *        Carry
         *
         **********/
        tmp1   = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        
        tmp2 = tmp1 ;
        tmp1 = ( tmp1 << 1 ) | ( ( tmpBit ) ? ( 0x01 ) : ( 0x00 ) ) ;
        
        tmpBit = isBitSet( tmp2 , 7 ) ; // Check bit 0.
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // ADD A , #data
    case 0x24 :
    // ADDC A , #data
    case 0x34 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  x  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/
         *                             |
         *                             +---------------------------> Opcode 24h = ADD
         *                                                                  34h = ADDC
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: Carry.
         *        Auxiliary Carry.
         *        Overflow.
         *        Parity.
         *
         **********/
        tmp2 = flashObj->read( pc + 1 ) ;
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        unsignedBit = isBitSet( tmp1 , 7 ) || isBitSet( tmp1 , 7 ) ;

        // Is ADDC opcode?
        tmpBit = isBitSet( opcode , 4 ) ;
        if( tmpBit )
        {
            // Yes, opcode ADDC.

            // We start calculation with Carry Flag.
            tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
            tmp16 = ( tmpBit ) ? ( 0x0001 ) : ( 0x0000 ) ;
        }
        else
        {
            // No, opcode ADD.
            tmp16 = 0x0000 ;
        }

        tmp16 += ( tmp1 & 0x01 ) + ( tmp2 & 0x01 ) ;
        tmp16 += ( tmp1 & 0x02 ) + ( tmp2 & 0x02 ) ;
        tmp16 += ( tmp1 & 0x04 ) + ( tmp2 & 0x04 ) ;
        tmp16 += ( tmp1 & 0x08 ) + ( tmp2 & 0x08 ) ;
        
        // Check AC flag.
        tmpBit = isBitSet( tmp16 , 4 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_AC , tmpBit ) ;
        
        tmp16 += ( tmp1 & 0x10 ) + ( tmp2 & 0x10 ) ;
        tmp16 += ( tmp1 & 0x20 ) + ( tmp2 & 0x20 ) ;
        tmp16 += ( tmp1 & 0x40 ) + ( tmp2 & 0x40 ) ;

        // Check OV flag.
        if( unsignedBit )
        {
            ramObj->writeBit( SFR_ADDR_PSW_OV , false ) ;
        }
        else
        {
            tmpBit = isBitSet( tmp16 , 7 ) ;
            ramObj->writeBit( SFR_ADDR_PSW_OV , tmpBit ) ;
        }

        tmp16 += ( tmp1 & 0x80 ) + ( tmp2 & 0x80 ) ;
        
        // Check Carry Flag.
        tmpBit = isBitSet( tmp16 , 8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        ramObj->write( SFR_ADDR_ACC , ( uint8_t ) tmp16 ) ;

        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc += 2 ;
        break ;
    // ADD A , addr8
    case 0x25 :
    // ADDC A , addr8
    case 0x35 :
        break ;
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  x  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/
         *                             |
         *                             +---------------------------> Opcode 25h = ADD
         *                                                                  35h = ADDC
         *
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: Carry.
         *        Auxiliary Carry.
         *        Overflow.
         *        Parity.
         *
         **********/
        addr8 = ( uint16_t ) flashObj->read( pc + 1 ) ;
        tmp2  = ramObj->read( addr8 ) ;
        tmp1  = ramObj->read( SFR_ADDR_ACC ) ;

        unsignedBit = isBitSet( tmp1 , 7 ) || isBitSet( tmp1 , 7 ) ;

        // Is ADDC opcode?
        tmpBit = isBitSet( opcode , 4 ) ;
        if( tmpBit )
        {
            // Yes, opcode ADDC.

            // We start calculation with Carry Flag.
            tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
            tmp16 = ( tmpBit ) ? ( 0x0001 ) : ( 0x0000 ) ;
        }
        else
        {
            // No, opcode ADD.
            tmp16 = 0x0000 ;
        }

        tmp16 += ( tmp1 & 0x01 ) + ( tmp2 & 0x01 ) ;
        tmp16 += ( tmp1 & 0x02 ) + ( tmp2 & 0x02 ) ;
        tmp16 += ( tmp1 & 0x04 ) + ( tmp2 & 0x04 ) ;
        tmp16 += ( tmp1 & 0x08 ) + ( tmp2 & 0x08 ) ;
        
        // Check AC Flag.
        tmpBit = isBitSet( tmp16 , 4 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_AC , tmpBit ) ;
        
        tmp16 += ( tmp1 & 0x10 ) + ( tmp2 & 0x10 ) ;
        tmp16 += ( tmp1 & 0x20 ) + ( tmp2 & 0x20 ) ;
        tmp16 += ( tmp1 & 0x40 ) + ( tmp2 & 0x40 ) ;

        // Check OV Flag.
        if( unsignedBit )
        {
            ramObj->writeBit( SFR_ADDR_PSW_OV , false ) ;
        }
        else
        {
            tmpBit = isBitSet( tmp16 , 7 ) ;
            ramObj->writeBit( SFR_ADDR_PSW_OV , tmpBit ) ;
        }

        tmp16 += ( tmp1 & 0x80 ) + ( tmp2 & 0x80 ) ;
        
        // Check Carry Flag.
        tmpBit = isBitSet( tmp16 , 8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        ramObj->write( SFR_ADDR_ACC , ( uint8_t ) tmp16 ) ;
                                
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc += 2 ;
        break ;
    // ADD A , @Rx
    case 0x26 :
    case 0x27 :
    // ADDC A , @Rx
    case 0x36 :
    case 0x37 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  x  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/                   \_ _/
         *                             |                       |
         *                             |                       +--> Register
         *                             +--------------------------> Opcode 26h/27h = ADD
         *                                                                 36h/37h = ADDC
         *
         * Flags: Carry.
         *        Auxiliary Carry.
         *        Overflow.
         *        Parity.
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp2 = ramObj->read( addr8 ) ;
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;

        unsignedBit = isBitSet( tmp1 , 7 ) || isBitSet( tmp1 , 7 ) ;

        // Is ADDC opcode?
        tmpBit = isBitSet( opcode , 4 ) ;
        if( tmpBit )
        {
            // Yes, opcode ADDC.

            // We start calculation with Carry Flag.
            tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
            tmp16 = ( tmpBit ) ? ( 0x0001 ) : ( 0x0000 ) ;
        }
        else
        {
            // No, opcode ADD.
            tmp16 = 0x0000 ;
        }

        tmp16 += ( tmp1 & 0x01 ) + ( tmp2 & 0x01 ) ;
        tmp16 += ( tmp1 & 0x02 ) + ( tmp2 & 0x02 ) ;
        tmp16 += ( tmp1 & 0x04 ) + ( tmp2 & 0x04 ) ;
        tmp16 += ( tmp1 & 0x08 ) + ( tmp2 & 0x08 ) ;
        
        // Check AC Flag.
        tmpBit = isBitSet( tmp16 , 4 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_AC , tmpBit ) ;
        
        tmp16 += ( tmp1 & 0x10 ) + ( tmp2 & 0x10 ) ;
        tmp16 += ( tmp1 & 0x20 ) + ( tmp2 & 0x20 ) ;
        tmp16 += ( tmp1 & 0x40 ) + ( tmp2 & 0x40 ) ;

        // Check OV Flag.
        if( unsignedBit )
        {
            ramObj->writeBit( SFR_ADDR_PSW_OV , false ) ;
        }
        else
        {
            tmpBit = isBitSet( tmp16 , 7 ) ;
            ramObj->writeBit( SFR_ADDR_PSW_OV , tmpBit ) ;
        }

        tmp16 += ( tmp1 & 0x80 ) + ( tmp2 & 0x80 ) ;
        
        // Check Carry Flag.
        tmpBit = isBitSet( tmp16 , 8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        ramObj->write( SFR_ADDR_ACC , ( uint8_t ) tmp16 ) ;
                                
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // ADD A , Rx
    case 0x28 :
    case 0x29 :
    case 0x2A :
    case 0x2B :
    case 0x2C :
    case 0x2D :
    case 0x2E :
    case 0x2F :
    // ADDC A , Rx
    case 0x38 :
    case 0x39 :
    case 0x3A :
    case 0x3B :
    case 0x3C :
    case 0x3D :
    case 0x3E :
    case 0x3F :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  0  |  1  |  x  |  1  |  x  |  x  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/       \_______ _______/
         *                             |                 |
         *                             |                 +--------> Register
         *                             +--------------------------> Opcode 28h ~ 2Fh = ADD
         *                                                                 38h ~ 3Fh = ADDC
         * Flags: Carry.
         *        Auxiliary Carry.
         *        Overflow.
         *        Parity.
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp2 = readBankedRegister( reg , ramObj ) ;
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;

        unsignedBit = isBitSet( tmp1 , 7 ) || isBitSet( tmp1 , 7 ) ;

        // Is ADDC opcode?
        tmpBit = isBitSet( opcode , 4 ) ;
        if( tmpBit )
        {
            // Yes, opcode ADDC.

            // We start calculation with Carry Flag.
            tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
            tmp16 = ( tmpBit ) ? ( 0x0001 ) : ( 0x0000 ) ;
        }
        else
        {
            // No, opcode ADD.
            tmp16 = 0x0000 ;
        }

        tmp16 += ( tmp1 & 0x01 ) + ( tmp2 & 0x01 ) ;
        tmp16 += ( tmp1 & 0x02 ) + ( tmp2 & 0x02 ) ;
        tmp16 += ( tmp1 & 0x04 ) + ( tmp2 & 0x04 ) ;
        tmp16 += ( tmp1 & 0x08 ) + ( tmp2 & 0x08 ) ;
        
        // Check AC Flag.
        tmpBit = isBitSet( tmp16 , 4 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_AC , tmpBit ) ;
        
        tmp16 += ( tmp1 & 0x10 ) + ( tmp2 & 0x10 ) ;
        tmp16 += ( tmp1 & 0x20 ) + ( tmp2 & 0x20 ) ;
        tmp16 += ( tmp1 & 0x40 ) + ( tmp2 & 0x40 ) ;

        // Check OV Flag.
        if( unsignedBit )
        {
            ramObj->writeBit( SFR_ADDR_PSW_OV , false ) ;
        }
        else
        {
            tmpBit = isBitSet( tmp16 , 7 ) ;
            ramObj->writeBit( SFR_ADDR_PSW_OV , tmpBit ) ;
        }

        tmp16 += ( tmp1 & 0x80 ) + ( tmp2 & 0x80 ) ;

        // Check Carry Flag.
        tmpBit = isBitSet( tmp16 , 8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        ramObj->write( SFR_ADDR_ACC , ( uint8_t ) tmp16 ) ;

        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;
        
        pc++ ;
        break ;
    // ORL addr8 , A
    case 0x42 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        tmp2 = tmp2 | tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 2 ;
        break ;
    // ORL addr8 , #data
    case 0x43 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        tmp1 = flashObj->read( pc + 2 ) ;
        // Execute OR logic.
        tmp2 = tmp2 | tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 3 ;
        break ;
    // ORL A , #data
    case 0x44 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: Parity.
         *
         **********/
        // Read data.
        tmp2 = flashObj->read( pc + 1 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        tmp1 = tmp1 | tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc += 2 ;
        break ;
    // ORL A , addr8
    case 0x45 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: Parity.
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute OR logic.
        tmp1 = tmp1 | tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

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
         * Flags: Parity.
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;
        // Increment the variable pointed from the register.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute OR operation.
        tmp1 = tmp1 | tmp2 ;

        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

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
         * Flags: Parity.
         *
         **********/
        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x07 ) ;
        // Read data from the register.
        tmp2 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute OR logic.
        tmp1 = tmp1 | tmp2 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        /**********
         *
         * Parity Flag.
         * Set/Cleared by hardware each instruction cycle
         * to indicate an odd/even number of 1 bits in the accumulator.
         *
         **********/
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmpBit = parity( tmp1 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_P , tmpBit ) ;

        pc++ ;
        break ;
    // JC offset8
    case 0x40 :
    // JNC offset8
    case 0x50 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  x  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/
         *                             |
         *                             +--------------------------> 40h - JC
         *                                                          50h - JNC
         *
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         * Flags: None.
         *
         **********/
        addr16 = ( uint16_t ) flashObj->read( pc + 1 ) ;
        
        pc += 2 ;

        // Test if opcode JC or JNC respectively.
        tmpBit = ( isBitSet( opcode , 4 ) == false ) ? ramObj->readBit( SFR_ADDR_PSW_C ) : !ramObj->readBit( SFR_ADDR_PSW_C ) ;
        if( tmpBit )
        {
            pc += addr16 ;
        }
        break ;
    // ANL addr8 , A
    case 0x52 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  1  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute AND logic.
        tmp2 = tmp2 & tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 2 ;
        break ;
    // ANL addr8 , #data
    case 0x53 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        tmp1 = flashObj->read( pc + 2 ) ;
        // Execute AND logic.
        tmp2 = tmp2 & tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 3 ;
        break ;
    // ANL A , #data
    case 0x54 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read data.
        tmp2 = flashObj->read( pc + 1 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute AND logic.
        tmp1 = tmp1 & tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        pc += 2 ;
        break ;
    // ANL A , addr8
    case 0x55 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  0  |  1  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute AND logic.
        tmp1 = tmp1 & tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        pc += 2 ;
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
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute AND operation.
        tmp1 = tmp1 & tmp2 ;

        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        pc++ ;
        break ;
    // ANL A , Rx
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
        tmp2 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute AND logic.
        tmp1 = tmp1 & tmp2 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        pc++ ;
        break ;
    // XRL addr8 , A
    case 0x62 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute XOR logic.
        tmp2 = tmp2 ^ tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 2 ;
        break ;
    // XRL addr8 , #data
    case 0x63 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        tmp1 = flashObj->read( pc + 2 ) ;
        // Execute XOR logic.
        tmp2 = tmp2 ^ tmp1 ;
        // Write data at the address.
        ramObj->write( addr8 , tmp2 ) ;
    
        pc += 3 ;
        break ;
    // XRL A , #data
    case 0x64 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read data.
        tmp2 = flashObj->read( pc + 1 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute XOR logic.
        tmp1 = tmp1 ^ tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        pc += 2 ;
        break ;
    // XRL A , addr8
    case 0x65 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  0  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read address.
        addr8 = flashObj->read( pc + 1 ) ;
        // Read data from this address.
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Execute XOR logic.
        tmp1 = tmp1 ^ tmp2 ;
        // Write data at the address.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
        pc += 2 ;
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
        tmp2 = ramObj->read( addr8 ) ;
        // Read ACC.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute XOR operation.
        tmp1 = tmp1 ^ tmp2 ;

        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
    
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
        tmp2 = readBankedRegister( reg , ramObj ) ;

        // Read ACC value.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Execute XOR logic.
        tmp1 = tmp1 ^ tmp2 ;
        
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        pc++ ;
        break ;
    // JZ offset
    case 0x60 :
    // JNZ offset
    case 0x70 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  x  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                           \_ _/
         *                             |
         *                             +--------------------------> 60h - JZ
         *                                                          70h - JNZ
         *
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr16 = ( uint16_t ) flashObj->read( pc + 1 ) ;
        
        pc += 2 ;

        // Read ACC value.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        // Test if opcode JZ or JNZ respectively.
        tmpBit = ( isBitSet( opcode , 4 ) == false ) ? ( tmp1 == 0x00 ) : ( tmp1 != 0x00 ) ;
        if( tmp1 )
        {
            pc += addr16 ;
        }
        break ;
    // ORL C , bitAddr8
    case 0x72 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  1  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        addr8 = flashObj->read( pc + 1 ) ;

        tmpBit = tmpBit || ramObj->readBit( addr8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        pc += 2 ;
        break ;
    // JMP A + DPTR
    case 0x73 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/

        // Load DPTR.
        addr16   = ( uint16_t ) ramObj->read( SFR_ADDR_DPH ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) ramObj->read( SFR_ADDR_DPL ) ;
        
        // Add ACC.
        addr16 += ( uint16_t ) ramObj->read( SFR_ADDR_ACC ) ;

        pc = addr16 ;
        break ;
    // MOV A , #data
    case 0x74 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  1  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        tmp1 = flashObj->read( pc + 1 ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

        pc += 2 ;
        break ;
    // MOV addr8 , #data
    case 0x75 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  0  |  1  |  1  |  1  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr8 = flashObj->read( pc + 1 ) ;
        tmp1  = flashObj->read( pc + 2 ) ;
        ramObj->write( addr8 , tmp1 ) ;

        pc += 3 ;
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
        tmp2 = flashObj->read( pc + 1 ) ;

        ramObj->write( addr8 , tmp2 ) ;
    
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
        tmp2 = flashObj->read( pc + 1 ) ;
        
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp2 ) ;

        pc += 2 ;
        break ;
    // SJMP offset
    case 0x80 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  O7 |  O6 |  O5 |  O4 |  O3 |  O2 |  O1 |  O0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        addr16 = ( uint16_t ) flashObj->read( pc + 1 ) ;
        
        pc += 2 ;
        pc += addr16 ;

        break ;
    // ANL C , bitAddr8
    case 0x82 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  0  |  1  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        addr8 = flashObj->read( pc + 1 ) ;

        tmpBit = tmpBit && ramObj->readBit( addr8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        pc += 2 ;
        break ;
    // MOVC A,@A+PC
    case 0x83 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Load PC.
        addr16  = pc ;
        // Add ACC.
        addr16 += ( uint16_t ) ramObj->read( SFR_ADDR_ACC ) ;

        tmp1 = flashObj->read( addr16 ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        pc++ ;
        break ;
    // DIV AB
    case 0x84 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  1  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/

        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        tmp2 = ramObj->read( SFR_ADDR_B   ) ;
        
        // Is B iqual zero?
        if( tmp2 == 0x00 )
        {
            // Yes, so it's an error.
            ramObj->write( SFR_ADDR_ACC , 0x00 ) ;
            ramObj->write( SFR_ADDR_B   , 0x00 ) ;
            
            ramObj->writeBit( SFR_ADDR_PSW_OV , true ) ;
        }
        else
        {
            // No, so it's possible to divide.
            tmp16 = ( uint16_t ) ( tmp1 / tmp2 ) ;
            ramObj->write( SFR_ADDR_ACC , ( uint8_t ) tmp16 ) ;

            tmp16 = ( uint16_t ) ( tmp1 % tmp2 ) ;
            ramObj->write( SFR_ADDR_B   , ( uint8_t ) tmp16 ) ;
            
            ramObj->writeBit( SFR_ADDR_PSW_OV , false ) ;
        }

        ramObj->writeBit( SFR_ADDR_PSW_C , false ) ;

        pc++ ;
        break ;
    // MOV addr8 , addr8
    case 0x85 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  1  |  0  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Read data from source address.
        addr8 = flashObj->read( pc + 1 ) ;
        tmp1 = ramObj->read( addr8 ) ;

        // Write data at destiny address.
        addr8 = flashObj->read( pc + 2 ) ;
        ramObj->write( addr8 , tmp1 ) ;

        pc += 3 ;
        break ;
    // MOV addr8 , @Rx
    case 0x86 :
    case 0x87 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  0  |  0  |  1  |  1  |  x  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *                                                   \_ _/
         *                                                     |
         *                                                     +--> Register
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/

        // Check with register will be used.
        reg = ( registerOffset_t ) ( opcode & 0x01 ) ;
        // Read address from the register.
        addr8 = readBankedRegister( reg , ramObj ) ;

        // Increment the variable pointed from the register.
        addr8 = ramObj->read( addr8 ) ;
        tmp1  = ramObj->read( addr8 ) ;
        
        addr8 = flashObj->read( pc + 1 ) ;
        ramObj->write( addr8 , tmp1 ) ;
    
        pc += 2 ;
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
    // MOVC A,@A+DPTR
    case 0x93 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  0  |  1  |  0  |  0  |  1  |  1  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        // Load DPTR.
        addr16   = ( uint16_t ) ramObj->read( SFR_ADDR_DPH ) ;
        addr16 <<= 8 ;
        addr16  |= ( uint16_t ) ramObj->read( SFR_ADDR_DPL ) ;
        
        // Add ACC.
        addr16 += ( uint16_t ) ramObj->read( SFR_ADDR_ACC ) ;

        tmp1 = flashObj->read( addr16 ) ;
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;
        
        pc++ ;
        break ;
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
    // ORL C , /bitAddr8
    case 0xA0 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  1  |  0  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        addr8 = flashObj->read( pc + 1 ) ;

        tmpBit = tmpBit || !ramObj->readBit( addr8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        pc += 2 ;
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
    // ANL C , /bitAddr8
    case 0xB0 :
        /**********
         *
         *           7     6     5     4     3     2     1     0
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         * Opcode |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *        |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
         *        +-----+-----+-----+-----+-----+-----+-----+-----+
         *
         **********/
        tmpBit = ramObj->readBit( SFR_ADDR_PSW_C ) ;
        addr8 = flashObj->read( pc + 1 ) ;

        tmpBit = tmpBit && !ramObj->readBit( addr8 ) ;
        ramObj->writeBit( SFR_ADDR_PSW_C , tmpBit ) ;

        pc += 2 ;
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
        tmp2 = readBankedRegister( reg , ramObj ) ;
        // Read ACC value.
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;

        // Save Register value in the ACC.
        ramObj->write( SFR_ADDR_ACC , tmp2 ) ;

        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp1 ) ;

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
         * Flags: None.
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
        tmp1 = readBankedRegister( reg , ramObj ) ;
        // Save ACC value at SFR.
        ramObj->write( SFR_ADDR_ACC , tmp1 ) ;

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
         * Flags: None.
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
        tmp1 = ramObj->read( SFR_ADDR_ACC ) ;
        
        // Write value back at the Register.
        writeBankedRegister( reg , ramObj , tmp1 ) ;

        pc++ ;
        break ;
    }
}
