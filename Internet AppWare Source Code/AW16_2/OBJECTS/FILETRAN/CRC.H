// ************************* START OF CRC.H *************************
//
//
//
// This header file contains the definitions for the two CRC classes,
// Crc16 and Crc32.
//

#ifndef _CRC_DOT_H
#define _CRC_DOT_H
//*
//* Class Crc16 is an example of a more sophisticted integrity
//* checking class. It has a new private data item table_16, and
//* a second private function build_table_16(). It also has its own
//* implementation of CalculatBlock(). It implements a 16 bit version
//* of a table driven cyclic redundancy check calculation.  And a new
//* function CalculateCharacter().
//*
#define CRC16_POLYNOMIAL 0x1021

class ChkCrc16
{
  public:
    ChkCrc16( void );
    ~ChkCrc16( void );
    unsigned long CalcCRC16( BYTE* data, unsigned int count,unsigned long crc = 0 );
    //unsigned long CalculateCharacter( unsigned char c,
    //                                  unsigned long crc = 0 );

  protected:
    void BuildTable16( unsigned int table[] );

    HGLOBAL hTable_16 ;
    static unsigned int far*table_16;
    static int users_16;
};


//*
//* Class Crc32 is an example of a more sophisticted integrity
//* checking class. It has a new private data item table_32, and
//* a second private function build_table_32(). It also has its own
//* implementation of CalculatBlock(). It implements a 32 bit version
//* of a table driven cyclic redundancy check calculation.  And a new
//* function CalculateCharacter().
//*
#define CRC32_POLYNOMIAL     0xEDB88320L

class ChkCrc32 
{
  public:
    ChkCrc32( void );
    ~ChkCrc32( void );
    unsigned long CalcCRC32( BYTE *data, unsigned int count,unsigned long crc = 0 );
    //unsigned long CalculateCharacter( unsigned char c,
    //                                  unsigned long crc = 0 );

  protected:
    void BuildTable32( unsigned long table[] );

    HGLOBAL hTable_32 ;
    static unsigned long far *table_32;
    static int users_32;
};
#endif // #ifndef _CRC_DOT_H

// *************************** END OF CRC.H ***************************

