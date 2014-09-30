////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"chkCrc16.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "filetrns.h"
#include "crc.h"
//*
//* Global variables for the GFCrc16 class.  This helps manage the
//* table if there are more than one users.
//*
unsigned int *ChkCrc16::table_16 = 0 ;
int ChkCrc16::users_16 = 0;

///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::ChkCrc16( void )
// Describtion: Construction
///////////////////////////////////////////////////////////////////////////
ChkCrc16::ChkCrc16( void )
{
  //size = 16;

  //*
  //* Allocate memory for the CRC lookup table and initialize the
  //* lookup values.
  //*
  if( table_16 == NULL ) {
		hTable_16 = GlobalAlloc (GHND, sizeof(unsigned int)*256) ;
		table_16 = (unsigned int*)GlobalLock (hTable_16) ;
    //table_16 = (unsigned int*)GFMem::AllocateMemory( sizeof( unsigned int ) * 256 );
    BuildTable16( table_16 );
  }
  ++users_16;
}
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::~ChkCrc16( void )
// Describtion: Construction
///////////////////////////////////////////////////////////////////////////
ChkCrc16::~ChkCrc16(void )
{
  //*
  //* Decrement the user flag.  If we were the only user then
  //* remove the table.
  //*
  --users_16;
  if( !users_16 ) {
		if (table_16)
			delete table_16 ;
    //GFMem::DeallocateMemory( table_16 );
    table_16 = NULL;
  }
}
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::CalculateBlock( void )
// Describtion: The calculate function for class GFCrc16 supersedes that for
//							the base class GFChecksum. It uses the look-up table for
//    					speed in calculating the crc value.
///////////////////////////////////////////////////////////////////////////   
unsigned long ChkCrc16::CalcCRC16( 
	BYTE *buffer,
  unsigned int count,
  unsigned long crc )
{
  BYTE *p = buffer;
  //*
  //* Calculate the 16 bit CRC using the table lookup method.
  //*
  while( count-- != 0 )
    crc =(crc << 8) ^ table_16[ (unsigned char)((crc >> 8) ^ *p++) ];
  return( crc & 0xffff );
}
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::CalculateCharacter( void )
// Describtion: The calculate function for class GFCrc16 supersedes that for
//              the base class GFChecksum. It uses the look-up table for
//              speed in calculating the crc value.
///////////////////////////////////////////////////////////////////////////   
/*unsigned long ChkCrc16::CalculateCharacter( 
	unsigned char c,
  unsigned long crc )
{
  return( ((crc << 8) ^ table_16[ (unsigned char)((crc >> 8) ^ c) ]) & 0xffff );
}*/
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::BuildTable16( void )
// Describtion: Here is the mktbl function which constructs the
//              crc look up table:
///////////////////////////////////////////////////////////////////////////
void ChkCrc16::BuildTable16( 
	unsigned int table[] )
{
  int i;
  int j;
  int k;
  int crc;

  //*
  //* Calculate the CRC value for index position n.
  //*
  for( i = 0; i < 256; i++ ) {
    k = i << 8;
    crc = 0;
    for( j = 0; j < 8; j++ ) {
      if( ( crc ^ k ) & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc <<= 1;
      k <<= 1;
    }
    table[ i ] = crc;
  }
}

//////////////////////////////////////////////////////////////*/
//*
//* Global variables for the GFCrc32 class.  This helps manage the
//* table if there are more than one users.
//*
unsigned long *ChkCrc32::table_32 = 0;
int ChkCrc32::users_32 = 0;
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc16::ChkCrc16( void )
// Describtion: Construction
///////////////////////////////////////////////////////////////////////////
ChkCrc32::ChkCrc32( void )
{
  //size = 32;

  //*
  //* Allocate memory for the CRC lookup table and initialize the
  //* lookup values.
  //*
  if( table_32 == NULL ) {
		hTable_32 = GlobalAlloc (GHND, sizeof(unsigned long)*256) ;
		table_32 = (unsigned long*)GlobalLock (hTable_32) ;

    //table_32 = (unsigned long *)GFMem::AllocateMemory( sizeof( unsigned long ) * 256 );
    BuildTable32( table_32 );
  }
  ++users_32;
}
ChkCrc32::~ChkCrc32( void )
{
  //*
  //* Decrement the user flag.  If we were the only user then
  //* remove the table.
  //*
  --users_32;
  if( !users_32 ) {
		if (table_32)
			delete table_32 ;
    //GFMem::DeallocateMemory( table_32 );
    table_32 = NULL;
  }
}
//* DESCRIPTION
//*   The calculate function for class GFCrc32 supersedes that for
//*   the base class GFChecksum. It uses the look-up table for
//*   speed in calculating the crc value.
unsigned long ChkCrc32::CalcCRC32( 
	BYTE *buffer,
  unsigned int count,
  unsigned long crc )
{
  BYTE *p = buffer;
  unsigned long temp1;
  unsigned long temp2;

  //*
  //* Calculate the 32 bit CRC using the table lookup method.
  //*
  while ( count-- != 0 ) {
    temp1 = ( crc >> 8 ) & 0x00FFFFFFL;
    temp2 = table_32[ ( (int) crc ^ *p++ ) & 0xff ];
    crc = temp1 ^ temp2;
  }
  return( crc );
}
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc32::~ChkCrc32( void )
// Describtion: The calculate function for class GFCrc16 supersedes that for
//              the base class GFChecksum. It uses the look-up table for
//              speed in calculating the crc value.
///////////////////////////////////////////////////////////////////////////
/*unsigned long ChkCrc32::CalculateCharacter( 
	unsigned char c,
  unsigned long crc )
{
  unsigned long temp1;
  unsigned long temp2;

  temp1 = (crc >> 8) & 0x00FFFFFFL;
  temp2 = table_32[ ( (int) crc ^ c ) & 0xff ];
  return( temp1 ^ temp2  );
}	*/
///////////////////////////////////////////////////////////////////////////
// Function:    ChkCrc32::~ChkCrc32( void )
// Describtion: Here is the mktbl function which constructs the
//              crc look up table:
///////////////////////////////////////////////////////////////////////////
void ChkCrc32::BuildTable32( unsigned long table[] )
{
  int i;
  int j;
  unsigned long crc;

  for( i = 0; i <= 255; i++ ) {
    crc = i;
    for( j = 8; j > 0; j-- ) {
      if( crc & 1 )
        crc = ( crc >> 1 ) ^ CRC32_POLYNOMIAL;
      else
        crc >>= 1;
    }
    table[ i ] = crc;
  }
}
