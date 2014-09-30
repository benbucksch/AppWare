#include "include.h"
#include "o_upsf.h"

LPSTR g_szUpsf_usVersion			= "VERSION";
LPSTR g_szUpsf_lTotalSize			= "TOTSIZE";
LPSTR g_szUpsf_uidNext				= "UIDNEXT";
LPSTR g_szUpsf_usNumButtons		= "NUMBUTS";
LPSTR g_szUpsf_bmpNormal			= "BITNORM";
LPSTR g_szUpsf_bmpHighlight		= "BITHIGH";

LPSTR g_szUpsf_button_scope		= "BUTSCOP";
LPSTR g_szUpsf_szName					= "BUTNAME";
LPSTR g_szUpsf_eButtonType		= "BUTTYPE";
LPSTR g_szUpsf_bSet						= "BUTSET";
LPSTR g_szUpsf_uidSignal			= "UIDSIG";
LPSTR g_szUpsf_usNumRegions		= "NUMRGNS";

LPSTR g_szUpsf_region_scope		= "RGNSCOP";
LPSTR g_szUpsf_eRegionType		= "RGNTYPE";
LPSTR g_szUpsf_eCombineMode		=	"CMODE";
LPSTR g_szUpsf_usNumPoints		= "NUMPTS";
LPSTR g_szUpsf_point					= "POINT";

/////////////////////////////////
/// Name: POLY::Export()
/// Purpose: Exports the polybutton object
/// Return: A_OK;
long POLY::Export()
	{
	unsigned short					us;
	LPBUTTON								pButton;
	LPBMP_DATA							bd;

	ExportULong(g_szUpsf_lTotalSize, 	0L, (long)GetSize());
	ExportUShort(g_szUpsf_usVersion,		0L, m_version);
	ExportULong(g_szUpsf_uidNext, 			0L, m_uidNext);
	ExportUShort(g_szUpsf_usNumButtons,	0L, m_usNumButtons);

	bd = (LPBMP_DATA)AObjLockData(m_oiSelf, I_DATA_NORMAL);
	if (bd)
		{
		bd->Export(g_szUpsf_bmpNormal);
		AObjUnlockData(m_oiSelf, I_DATA_NORMAL);
		}

	bd = (LPBMP_DATA)AObjLockData(m_oiSelf, I_DATA_HIGHLIGHT);
	if (bd)
		{
		bd->Export(g_szUpsf_bmpHighlight);
		AObjUnlockData(m_oiSelf, I_DATA_HIGHLIGHT);
		}


	for (pButton = (LPBUTTON)m_aButtons, us=0; us<m_usNumButtons; us++)
		{
  	if(AUpsfCreateScope(g_szUpsf_button_scope, us)==AUPSF_NOERROR)
			{
			pButton->Export();
			AUpsfCloseScope();
			}
		pButton = (LPBUTTON) ( (LPSTR)pButton + pButton->GetSize() );
		}

	return A_OK;
	}

///////////////
/// Name:	BUTTON::Export()
/// Purpose: Exports the button class.  Called by POLY:Export
/// Return: A_OK;
long BUTTON::Export()
	{		 
	USHORT									us;
	LPREGION								pRegion;

	ExportText(g_szUpsf_szName, 0L, m_szButtonName, I_BUTTON_NAME_LENGTH);
	ExportShort(g_szUpsf_eButtonType, 0L, (short)m_buttonType);
	ExportBool(g_szUpsf_bSet, 0L, m_bSet);
	ExportLong(g_szUpsf_uidSignal, 0L, m_uidSignal);
	ExportUShort(g_szUpsf_usNumRegions, 0L, m_usNumRegions);

	for(pRegion = (LPREGION)m_aRegions, us=0; us<m_usNumRegions; us++)
		{
  	if(AUpsfCreateScope(g_szUpsf_region_scope, us)==AUPSF_NOERROR)
			{
			pRegion->Export();
			AUpsfCloseScope();
			}
		pRegion = (LPREGION)( (LPSTR)pRegion + pRegion->GetSize());
		}

	return A_OK;
	}


///////////////
/// Name:	REGION::Export()
/// Purpose: Exports the region class.  Called by BUTTON:Export
/// Return: A_OK;
long REGION::Export()
	{
	USHORT									us;

	ExportShort(g_szUpsf_eRegionType, 0L, (short)m_type);
	ExportShort(g_szUpsf_eCombineMode, 0L, (short)m_combineMode);
	ExportUShort(g_szUpsf_usNumPoints, 0L, m_usNumPoints);

	for(us=0;	us<m_usNumPoints; us++)
		ExportPoint(g_szUpsf_point, us, &m_aPoints[us]);

	return A_OK;
	}

///////////////////////////////
/// Name: POLY::Import  static member funtion				
/// Purpose: Sizes the datas for the polybutton, and imports it.
/// Parameters: oiPoly--the objectid of the polybutton to import
/// Return: A_OK or A_ERROR
long POLY::Import( 
	AOBJECTID								oiPoly)
	{
	LPPOLY									poly;
	unsigned long						ulPolySize;
	long										lBmpSize;
	LPBMP_DATA							pBmpData;
	LPBUTTON								pButton;
	USHORT									us;
				
	if ( ImportULong(g_szUpsf_lTotalSize, 0L, &ulPolySize) != AUPSF_NOERROR)
		{
	  AUpsfLogComment("Unknown polybutton size. Unable to import");
		return A_ERROR;
		}

	SetPolyDataSize(oiPoly,	ulPolySize);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return A_ERROR);

	ImportUShort(g_szUpsf_usVersion,		0L, &poly->m_version);
	ImportULong(g_szUpsf_uidNext, 			0L, &poly->m_uidNext);
	ImportUShort(g_szUpsf_usNumButtons,	0L, &poly->m_usNumButtons);

	if(AUpsfFieldInfo(g_szUpsf_bmpNormal, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR && lBmpSize)
		{
		AObjResizeData(poly->m_oiSelf, I_DATA_NORMAL, lBmpSize + sizeof(BITMAPFILEHEADER));
		pBmpData = (LPBMP_DATA)AObjLockData(poly->m_oiSelf, I_DATA_NORMAL);
		if (pBmpData)
			{
			pBmpData->Import(g_szUpsf_bmpNormal, lBmpSize);
			AObjUnlockData(poly->m_oiSelf, I_DATA_NORMAL);
			}
		}
	else 
		AObjResizeData(poly->m_oiSelf, I_DATA_NORMAL, 0L);  //no normal bitmap

	if(AUpsfFieldInfo(g_szUpsf_bmpHighlight, 0L, AUPSF_NATV_BITMAPINFO, NULL, &lBmpSize) == AUPSF_NOERROR && lBmpSize)
		{
		AObjResizeData(poly->m_oiSelf, I_DATA_HIGHLIGHT, lBmpSize + sizeof(BITMAPFILEHEADER));
		pBmpData = (LPBMP_DATA)AObjLockData(poly->m_oiSelf, I_DATA_HIGHLIGHT);
		if (pBmpData)
			{
			pBmpData->Import(g_szUpsf_bmpNormal, lBmpSize);
			AObjUnlockData(poly->m_oiSelf, I_DATA_HIGHLIGHT);
			}
		}
	else 
		AObjResizeData(poly->m_oiSelf, I_DATA_HIGHLIGHT, 0L);  //no normal bitmap

	//now, do the buttons
	for (pButton = (LPBUTTON)poly->m_aButtons, us=0; us<poly->m_usNumButtons; us++)
		{
  	if(AUpsfOpenScope(g_szUpsf_button_scope, us)==AUPSF_NOERROR)
			{
			pButton->Import();
			AUpsfCloseScope();
			}
		pButton = (LPBUTTON) ( (LPSTR)pButton + pButton->GetSize() );
		}

	return A_OK;
	}

/////////////////

///////////////////
/// Name BUTTON::Import()
/// Purpose: Import the button class part of the polybutton object.  Called by POLY::Import
/// Return: A_OK;
long BUTTON::Import()
	{
	USHORT									us;
	LPREGION								pRegion;

  AUpsfImportField(g_szUpsf_szName, 0L, AUPSF_NATV_STRING, m_szButtonName, I_BUTTON_NAME_LENGTH);
	ImportShort(g_szUpsf_eButtonType, 0L, (short *)&m_buttonType);
	ImportBool(g_szUpsf_bSet, 0L, &m_bSet);
	ImportLong(g_szUpsf_uidSignal, 0L, &m_uidSignal);
	ImportUShort(g_szUpsf_usNumRegions, 0L, &m_usNumRegions);

	for(pRegion = (LPREGION)m_aRegions, us=0; us<m_usNumRegions; us++)
		{
  	if(AUpsfOpenScope(g_szUpsf_region_scope, us)==AUPSF_NOERROR)
			{
			pRegion->Import();
			AUpsfCloseScope();
			}
		pRegion = (LPREGION)( (LPSTR)pRegion + pRegion->GetSize());
		}

	return A_OK;
	}


///////////////////
/// Name REGION::Import()
/// Purpose: Import the region class part of the button class.  Called by BUTTON::Import
/// Return: A_OK;
long REGION::Import()
	{
	USHORT									us;
																								 
	ImportShort(g_szUpsf_eRegionType, 0L, (short *)&m_type);
	ImportShort(g_szUpsf_eCombineMode, 0L, (short *)&m_combineMode);
	ImportUShort(g_szUpsf_usNumPoints, 0L, &m_usNumPoints);

	for(us=0;	us<m_usNumPoints; us++)
		ImportPoint(g_szUpsf_point, us, &m_aPoints[us]);

	return A_OK;
	}
