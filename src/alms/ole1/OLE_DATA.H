/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					OLE_DATA.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains the datablock definitions and class wrappers.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OLE_DATA_H
#define OLE_DATA_H

////////////////////////////////////////////////////////////////
//
// Datablock definitions:
//
//	Datablock 0 contains the configuration information of the object
//	Datablock 1 contains the (server-generated) OLE data.
//
//	OLEDATA_BLOCKCOUNT:		# of datas on the OLE object
//	OLEDATA_CFGBLOCK:			contains OLE Object configuration data.
//	OLEDATA_OLEBLOCK:			contains OLE object data written with OleWriteToStream
//
////////////////////////////////////////////////////////////////

#define OLEDATA_CFGBLOCK				0												// first block
#define OLEDATA_OLEBLOCK				1												// last block
#define OLEDATA_BLOCKCOUNT			(OLEDATA_OLEBLOCK + 1)

////////////////////////////////////////////////////////////////
//
// CLASS:		OLEDATA0
//
// DESC:		Maps the raw datablock 0 into a more friendly class interface.
//					This is done with a class rather than a dumb struct for
//					hygienic reasons.  String fields within the data are stored
//					as offsets into the datablock.
//
////////////////////////////////////////////////////////////////

const int					OLEOBJ_VERSION		= 1;					// current version of data

class OLECFGDATA
	{
	private:
		short					m_Version;											// version of object
		OLEOPTIONS		m_fOptions;											// options
		AOBJECTID			m_oMenuObject;									// menu to modify
		WORD					m_pszObjectType;								// descriptive name of object type
		WORD					m_pszLinkFile;									// if linked, then file to link
		WORD					m_pszLinkItem;									// if linked, then item to link
		WORD					m_nMenuInsertPos;								// insert before this position in menu
		char					m_cData[1];											// actual text for above strings

		VOID					Init(LPSTR, WORD&, LPSTR&);
		LPCSTR				GetString(WORD o)								{return ((o > 0) ? (((LPCSTR) this) + o) : NULL);}

									OLECFGDATA(VOID)								{}			// disable default
								 ~OLECFGDATA(VOID)								{}			// called from Destroy()

	public:
		static WORD		SizeNeeded(LPSTR, LPSTR, LPSTR);

		VOID*					operator new(size_t s)					{return NULL;}		// disable default
		VOID*					operator new(size_t, SROLEObject*);
		VOID					operator delete(VOID* p)				{}								// don't free

									OLECFGDATA(SROLEObject*)				{}


		VOID					Init(OLEOPTIONS, LPSTR, LPSTR, LPSTR);	// for construction
		VOID					Destroy(SROLEObject*);

		short					Version(VOID)										{return m_Version;}

		VOID					SetMenuObject(AOBJECTID o)				{m_oMenuObject = o;}
		VOID					SetMenuInsertPos(WORD n)				{m_nMenuInsertPos = n;}

		LPCSTR				ObjectType(VOID)								{return GetString(m_pszObjectType);}
		LPCSTR				LinkFile(VOID)									{return GetString(m_pszLinkFile);}
		LPCSTR				LinkItem(VOID)									{return GetString(m_pszLinkItem);}
		WORD					MenuPosition(VOID)							{return m_nMenuInsertPos;}
		AOBJECTID			MenuObject(VOID)								{return m_oMenuObject;}
		OLEOPTIONS		Options(VOID)										{return m_fOptions;}
	};


#endif	// !OLE_DATA_H
