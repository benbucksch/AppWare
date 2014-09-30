#define STRICT
#define ASTRICT

#include <string.h>
#include "commObj.h"

BOOL ALMAPI CommIsPortOpen(OBJECTID oiConn)
{
	COMMUNICATION *comm ;
  BOOL isOpen = FALSE ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		isOpen = comm->LinkOpen() ;
	return (isOpen) ;
}
int  ALMAPI CommRxCount (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
	int RXcount = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
  	RXcount = comm->RxCount() ;
	return (RXcount) ;
}
int  ALMAPI CommTxCount (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
  int TXcount = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if(comm)
		TXcount = comm->TxCount() ;
	return (TXcount) ;
}
int ALMAPI CommFlushQueue (OBJECTID oiConn, int fnQueue)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if(comm)
		rVal = comm->FlushQueue (fnQueue) ;
	return (rVal) ;
}
int  ALMAPI CommGetStopBits (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
	int StopBits = -1;

	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
  	StopBits = comm->GetStopBits() ;
	return (StopBits) ;
}
int  ALMAPI CommGetParity (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
	int Parity = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		Parity = comm->GetParity() ;
  return (Parity) ;
}
int  ALMAPI CommGetHandShake(OBJECTID oiConn) 
{
	COMMUNICATION *comm ;
	int HandShake ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		HandShake = comm->GetHandShake() ;

	return (HandShake) ;
}
int  ALMAPI CommSetBaudRate (OBJECTID oiConn, int Baudrate)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->SetBaudRate(Baudrate) ;
  return (rVal) ;
}

int  ALMAPI CommSetStopBits (OBJECTID oiConn, int StopBits)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->SetStopBits(StopBits) ;
  return (rVal) ;
}
int  ALMAPI CommSetParity (OBJECTID oiConn, int Parity)
{
	COMMUNICATION *comm ;
  int rVal ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->SetParity(Parity) ;
  return (rVal) ;
}
int  ALMAPI CommSetHandShake(OBJECTID oiConn, int handshake)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->SetHandShake(handshake) ;
  return (rVal) ;
}
int	ALMAPI CommSetXONXOFF(OBJECTID oiConn, int XonXoff)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->SetXonXoff(XonXoff) ;
  return (rVal) ;
}
int ALMAPI CommGetPauseStatus(OBJECTID oiConn)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->GetPauseStatus() ;
  return (rVal) ;
}
int	 ALMAPI	CommGetBuffer( OBJECTID oiConn, BYTE *buffer, int char_count )
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->GetBuffer(buffer, char_count) ;
  return (rVal) ;
}
int  ALMAPI CommGetString (OBJECTID oiConn, BYTE* string, int n)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->GetString (string, n) ;
  return (rVal) ;
}
int  ALMAPI CommGetChar(OBJECTID oiConn )
{
	COMMUNICATION *comm ;
  int c = -1;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if(comm)
		c = comm->GetChar() ;
	return (c) ;
}
int  ALMAPI CommWaitChar(OBJECTID oiConn, long mesc)
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->WaitChar(mesc) ;
  return (rVal) ;
}
int  ALMAPI CommPutBuffer(OBJECTID oiConn, const BYTE* buf, int n )
{
	COMMUNICATION *comm ;
  int rVal ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->PutBuffer(buf, n) ;
  return (rVal) ;
}
int  ALMAPI CommPutChar(OBJECTID oiConn, BYTE b )
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->PutChar(b) ;
  return (rVal) ;
}
int  ALMAPI CommPutString(OBJECTID oiConn, const BYTE *string )
{
	COMMUNICATION *comm ;
  int rVal = -1 ;
	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		rVal = comm->PutString(string) ;
  return (rVal) ;
}
LPSTR ALMAPI CommGetMem (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
	LPSTR lpMem = NULL ;

	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		lpMem = comm->lpCommMem ;
  return (lpMem) ;
}
long ALMAPI CommGetMemLen (OBJECTID oiConn)
{
	COMMUNICATION *comm ;
	long len = 0 ;

	comm = (COMMUNICATION*) AObjGetRuntimeInfo(oiConn) ;
	if (comm)
		len = _fstrlen (comm->lpCommMem) ;
  return (len) ;
}
