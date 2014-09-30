///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// textFact.cpp
//
// Text Builder
// TEXTFACT class functions.
//
// Robert Lundahl/ Edgar C. Jerez
//
// Start Date 05/07/92
// End   Date 06/../92
///////////////////////////////////////////////////////////////

#include "tf_incld.h"


///////////////////////////////////////////////////////////////
// TEXTFACT::TEXTFACT: Class Constructor
//
///////////////////////////////////////////////////////////////

TEXTFACT::TEXTFACT ( OBJECTID id )
{
  LONG	zero = 0L;

	hActive = GetActiveWindow();
	objectId = id;
	if (AObjGetDataCount(objectId) == 0)
  	{
		AObjSetDataCount(objectId, DATACOUNT);
    }

	data0 = (OBJECTID*)AObjLockData(objectId, 0L);
	if (data0 == NULL)
  	{
		AObjSetData(objectId, 0L, &zero, 4L);
		data0 = (OBJECTID*)AObjLockData(objectId, 0L);
    }

	strclass1 = new STRING(objectId, 1L);

	data2 = (TOKEN*)AObjLockData(objectId, 2L);
	if (data2 == NULL)
		{
		AObjResizeData(objectId, 2L, sizeof(TOKEN));
		data2 = (TOKEN*)AObjLockData(objectId, 2L);
		data2->type = STOP;
		}

	strclass3 = new STRING(objectId, 3L);

	data4 = (DATAFOUR*)AObjLockData(objectId, 4L);
	if (data4 == NULL)
		{
		AObjResizeData(objectId, 4L, sizeof(DATAFOUR));
		data4 = (DATAFOUR*)AObjLockData(objectId, 4L);
		data4->autoUpdate 	= FALSE;
		data4->resultId 	= 0L;
 		data4->version		= VERSION_OBJECT;
		}
	if(data4->version==101)
  	{
		data4->version = VERSION_OBJECT;
    }
}				// TEXTFACT::TEXTFACT...
		


///////////////////////////////////////////////////////////////
// TEXTFACT::DoAssign:
//
///////////////////////////////////////////////////////////////

LONG TEXTFACT::DoAssign (OBJECTID theObject,  pAObjMessage systemPtr )
{
	LONG			dataPtrCnt;
	LONG 			ptrIndex;
	TYPEID		srcType, dstType;
	OBJECTID 	srcObject, dstObject;

	srcObject = systemPtr->message3;
	dstObject = theObject;
	srcType = AObjGetType(srcObject);
	dstType = AObjGetType(dstObject);
	if (!srcObject || !dstObject)
	{
		MessageBox(NULL, "Assign Failed; Null Object Value", NULL, MB_OK | MB_ICONEXCLAMATION);
		return (A_ERROR);
	}
	if ( srcType != OTYPE_TEXTMAKER || dstType!= OTYPE_TEXTMAKER )
	{
		MessageBox(NULL, "Assign Failed; Invalid Object Type", NULL, MB_OK | MB_ICONEXCLAMATION);
		return (A_ERROR);
	}
	dataPtrCnt = AObjGetDataCount(srcObject);
	for (ptrIndex = 0; ptrIndex < dataPtrCnt; ptrIndex++)
		CopyDataPtr(dstObject,srcObject,ptrIndex);
	return (A_OK);
}				// DoAssign...


///////////////////////////////////////////////////////////////
// TEXTFACT::ExportObject:
//
///////////////////////////////////////////////////////////////

LONG TEXTFACT::ExportObject ( pAObjMessage /*systemPtr*/ )
{
	char*		p;
	char*		empty = "";
  char*		express;
	UINT 		i;
	LRESULT	ret;
	long		length;

	ret = AUpsfExportField("VERSION", 0L, AUPSF_NATV_UINT, AUPSF_UNIV_INTEGER, &data4->version, sizeof(UINT));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Text Builder: Error On Export 'VERSION'", NULL, MB_OK | MB_ICONEXCLAMATION);
	for (i = 0; data0[i]; i++)
  {
		ret = AUpsfExportField("OBJLIST", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, data0+i, sizeof(OBJECTID));
		if (ret != AUPSF_NOERROR)
			MessageBox(NULL, "Text Builder: Error On Export 'OBJLIST'", NULL, MB_OK | MB_ICONEXCLAMATION);
	}
	for (i = 0; data0[i]; i++)
	{
		p = (char*)strclass1->Get(i);
		if (p)
			length = strlen(p);
		else
    {
      p = empty;
			length = 0;
		}
		ret = AUpsfExportField("OBJALIAS", i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, p, length);
		if (ret != AUPSF_NOERROR)
			MessageBox(NULL, "Text Builder: Error On Export 'OBJALIAS'", NULL, MB_OK | MB_ICONEXCLAMATION);
	}

    // export auto update and result object
	ret = AUpsfExportField("AUTOUPDT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &data4->autoUpdate, sizeof(BOOL));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Text Builder: Error On Export 'AUTOUPDT'", NULL, MB_OK | MB_ICONEXCLAMATION);

	ret = AUpsfExportField("RESULTOBJ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data4->resultId, sizeof(OBJECTID));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Text Builder: Error On Export 'RESULTOBJ'", NULL, MB_OK | MB_ICONEXCLAMATION);

	// Build Expression
	express = (char*)new BYTE[maxEdit*2];
	if (express == NULL)
		return (A_ERROR);

	express[0] = 0;
	MakeExport(express);
	ret = AUpsfExportField("EXPRESSI", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, express, lstrlen(express));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Text Builder: Error On Export 'EXPRESSI'", NULL, MB_OK | MB_ICONEXCLAMATION);

	express[0] = 0;
	MakeExportMac(express);
	ret = AUpsfExportField("EXPRESSN", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, express, lstrlen(express));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Text Builder: Error On Export 'EXPRESSN'", NULL, MB_OK | MB_ICONEXCLAMATION);

	delete express;
	return (A_OK);
}				// TEXTFACT::ExportObject...


///////////////////////////////////////////////////////////////
// TEXTFACT::ImportObject:
//
///////////////////////////////////////////////////////////////

LONG TEXTFACT::ImportObject ( pAObjMessage /*systemPtr*/ )
{
  char		charbuf[256];
	UINT		i = 0;
	long		count;
	long		ret;
	long		natvSize;
	LONG		result = A_OK;

	count = AUpsfFieldCount("OBJLIST");
	if (count > 0)
  {
		AObjUnlockData(objectId, 0L);
		AObjResizeData(objectId, 0L, (count+1)*sizeof(OBJECTID));
		data0 = (OBJECTID*)AObjLockData(objectId, 0L);
		for (i = 0; i < count; i++)
		{
			ret = AUpsfImportField("OBJLIST", i, AUPSF_NATV_OBJECTREF, data0+i, sizeof(OBJECTID));
			if (ret != AUPSF_NOERROR)
			{
				AUpsfLogComment("Error On Import 'OBJLIST'; List Terminated");
				data0[i] = NULL;
				result = A_ERROR;
				break;
			}

			ret = AUpsfImportField("OBJALIAS", i, AUPSF_NATV_STRING, charbuf, 255);
			if (ret != AUPSF_NOERROR)
			{
				AUpsfLogComment("Error On Import 'OBJALIAS'; List Terminated");
				strclass1->Append((BYTE*)"");
				data0[i] = NULL;
				result = A_ERROR;
				break;
			}
			strclass1->Append((BYTE*)charbuf);
		}
 		data0[i] = NULL;
	}

	ret = AUpsfFieldInfo("EXPRESSI", 0L, AUPSF_NATV_STRING, NULL, &natvSize);
	if (ret != AUPSF_NOERROR)
	{
  	// Try importing from the Mac....
		ret = AUpsfFieldInfo("EXPRESSN", 0L, AUPSF_NATV_STRING, NULL, &natvSize);
		if (ret != AUPSF_NOERROR)
		{
			AUpsfLogComment("Error On Import 'EXPRESSN'; Setting to zero");
			natvSize = 0L;
			result = A_ERROR;
		}

		if (natvSize > 0)
		{
			char*	expression;
	
			expression = new char[UINT(natvSize+1)];
			ret = AUpsfImportField("EXPRESSN", 0L, AUPSF_NATV_STRING, expression, natvSize+1);
			if (ret != AUPSF_NOERROR)
      {
				AUpsfLogComment("Error On Import 'EXPRESSN'; Setting to NULL");
				result = A_ERROR;
			}
	    else
				Parse((BYTE*)expression, NULL, TRUE);
	
	   	delete expression;
		}
	}
	else
	{
		if (natvSize > 0)
		{
			char*	expression;
	
			expression = new char[UINT(natvSize+1)];
			ret = AUpsfImportField("EXPRESSI", 0L, AUPSF_NATV_STRING, expression, natvSize+1);
			if (ret != AUPSF_NOERROR)
      {
				AUpsfLogComment("Error On Import 'EXPRESSI'; Setting to NULL");
				result = A_ERROR;
			}
	    else
				Parse((BYTE*)expression, NULL, TRUE);

	   	delete expression;
		}
  }

	ret = AUpsfImportField("AUTOUPDT", 0L, AUPSF_NATV_BOOL, &data4->autoUpdate, sizeof(BOOL));
	if (ret != AUPSF_NOERROR)
	{
		AUpsfLogComment("Error On Import AUTOUPDT; Setting to Off");
		data4->autoUpdate = 0;
		result = A_ERROR;
	}

	ret = AUpsfImportField("RESULTOBJ", 0L, AUPSF_NATV_OBJECTREF, &data4->resultId, sizeof(OBJECTID));
	if (ret != AUPSF_NOERROR)
	{
		AUpsfLogComment("Error On Import RESULTOBJ; Set to (None)");
    data4->resultId = 0L;
		result = A_ERROR;
	}
	return (result);
}				// TEXTFACT::ImportObject...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeExport:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeExport ( char* express )
{
  int		len;

	if (data2 == NULL)
		return;
	for ( int i = 0; data2[i].type != STOP; i++)
	{
		switch ( data2[i].type )
		{
		case LITERAL:
			MakeLiteral((BYTE*)express, data2[i].multi);
			break;

		case CHARACTER:
			MakeCharacter((BYTE*)express, data2[i].multi);
			break;

		case OBJECT:
			MakeObject((BYTE*)express, data2[i].multi);
			break;
		}
	}

	if (i > 0)
    {
		len = lstrlen(express);
   	express[len-1] = 0;
    }
}				// TEXTFACT::MakeExport


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeExportMac:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeExportMac ( char* express )
{
  int		len;

	if (data2 == NULL)
		return;
	for ( int i = 0; data2[i].type != STOP; i++)
	{
		switch ( data2[i].type )
		{
		case LITERAL:
			MakeLiteral((BYTE*)express, data2[i].multi);
			break;

		case CHARACTER:
			MakeCharacterMac((BYTE*)express, data2[i].multi);
			break;

		case OBJECT:
			MakeObject((BYTE*)express, data2[i].multi);
			break;
		}
	}				// for ( int i = 0; data2[i].type != STOP; i++)...
	if (i > 0)
  {
		len = lstrlen(express);
   	express[len-1] = 0;
  }
}				// TEXTFACT::MakeExportMac...


///////////////////////////////////////////////////////////////
// TEXTFACT::MakeCharacterMac:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::MakeCharacterMac ( BYTE* express, LONG multi )
{
	char 	space[30];
	wsprintf(space, "\"%c\"+", (char)multi);
	lstrcat((LPSTR)express, (LPCSTR)space);
}				// TEXTFACT::MakeCharacterMac...


///////////////////////////////////////////////////////////////
// TEXTFACT::Validate:
//
///////////////////////////////////////////////////////////////

LONG TEXTFACT::Validate ( pAObjMessage sys )
{
    BOOL		changed = FALSE;
    int			i;

	// data 0 & data 1
	for (i = 0; data0[i]; i++)
	{
		changed = AObjValidateID(data0+i, sys);
		if (changed && data0[i] == 0L)
		{
			DataObjectDelete(data0+i);
			strclass1->Delete(i);
        	i--;
		}
	}

	// data 2 & data  3
	for (i = 0; data2[i].type != STOP; i++)
	{
		if (data2[i].type == OBJECT)
		{
			changed = AObjValidateID(&(data2[i].multi), sys);
			if (changed && data2[i].multi == 0L)
			{
				DataTokenDelete(data2+i);
				strclass3->Delete(i);
            	i--;
			}
		}
	}

	// data 4
	changed = AObjValidateID(&data4->resultId, sys);
	return (A_OK);
}				// TEXTFACT::Validate...


///////////////////////////////////////////////////////////////
// TEXTFACT::CheckObjectID:
//
///////////////////////////////////////////////////////////////

VOID TEXTFACT::CheckObjectID ( void )
{
    BOOL		valid;
    int			i;

	// data 0 & data 1
	for (i = 0; data0[i]; i++)
	{
		valid = AObjCheckType(data0[i], 0L);
		if (!valid)
		{
			DataObjectDelete(data0+i);
			strclass1->Delete(i);
			i--;
		}
	}

	// data 2 & data  3
	for (i = 0; data2[i].type != STOP; i++)
	{
		if (data2[i].type == OBJECT)
		{
			valid = AObjCheckType(data2[i].multi, 0L);
			if (!valid)
			{
				DataTokenDelete(data2+i);
				strclass3->Delete(i);
            	i--;
			}
		}
	}

	// data 4
	valid = AObjCheckType(data4->resultId, 0L);
	if (!valid)
    	data4->resultId = 0L;
}				// TEXTFACT::CheckObjectID...


///////////////////////////////////////////////////////////////
// TEXTFACT::DataObjectDelete:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::DataObjectDelete ( OBJECTID* objPtr )
{
	while (*(objPtr+1))
	{
    	*objPtr = *(objPtr+1);
    	objPtr++;
	}
  *objPtr = NULL;
}				// TEXTFACT::DataObjectDelete...


///////////////////////////////////////////////////////////////
// TEXTFACT::DataTokenDelete:
//
///////////////////////////////////////////////////////////////

void TEXTFACT::DataTokenDelete ( TOKEN* tokenPtr )
{
	while ((tokenPtr+1)->type != STOP)
	{
		*tokenPtr = *(tokenPtr+1);
    	tokenPtr++;
	}
	*tokenPtr = *(tokenPtr+1);
}				// TEXTFACT::DataTokenDelete...


///////////////////////////////////////////////////////////////
// TEXTFACT::ObjectHealthy:
//
///////////////////////////////////////////////////////////////

LONG TEXTFACT::ObjectHealthy (
	AObjInfo* /*infoRecordPtr*/ )
{
	if (data4->resultId == 0)
	{
		AObjReportError(objectId, AOBJ_CHECKOBJECT, A_ERROR, "Text Builder object should have a result object specified.", (long)Version());
		return (A_WARNING);
	}
	else
		return (A_OK);
}				// TEXTFACT::ObjectHealthy...


///////////////////////////////////////////////////////////////
// CopyDataPtr: 
//
///////////////////////////////////////////////////////////////

void TEXTFACT::CopyDataPtr ( OBJECTID dstObj, OBJECTID srcObj, long index )
{

	if (index == 1L)
		{
		// Index 1L and 3L are handled by the string class
		strclass1->CopyDataPtr(dstObj, index);
		return;
		}

	if (index == 3L)
		{
		// Index 1L and 3L are handled by the string class
		strclass3->CopyDataPtr(dstObj, index);
		return;
		}

	LONG 		srcSize;
	LPVOID	srcPtr;
	LPVOID 	dstPtr;

	srcPtr  = AObjLockData(srcObj, index);
	srcSize = AObjGetDataSize(srcObj, index);

	AObjUnlockData(dstObj, index);
	AObjResizeData(dstObj, index, srcSize);
	dstPtr  = AObjLockData(dstObj, index);

	if (srcPtr && dstPtr)
		_fmemcpy(dstPtr, srcPtr, (size_t)srcSize);

	AObjUnlockData(srcObj, index);


}				// void CopyDataPtr ( OBJECTID dstObj...

