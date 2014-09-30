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
// FILE:					SROLEAPP.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Defines an application class.  This application object
//								manages documents opened within the application (currently,
//								only one document will ever be opened).
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLEAPP_H
#define SROLEAPP_H

////////////////////////////////////////////////////////////////
//
// CLASS:	SROleClientApp
//
// DESC:	Manages application-level OLE routines.
//
////////////////////////////////////////////////////////////////

// Maximum size of application name
const int							SA_MAXAPP					= 64;

class SROleClientApp
	{
	private:
		char							m_szAppName[SA_MAXAPP];	// human-readable app name
		WORD							m_cfNative;							// clipformat of Native
		WORD							m_cfOwnerLink;					// clipformat of OwnerLink
		WORD							m_cfObjectLink;					// clipformat of ObjectLink

		SROleClientDoc*		m_Document;							// should actually be an array of docs

	public:
											SROleClientApp(LPSTR);
										 ~SROleClientApp(VOID);

		BOOL							Init(VOID);							// FALSE => failure
		VOID							Cleanup(VOID);

		BOOL							AddDoc(SROleClientDoc*);
		VOID							RemoveDoc(SROleClientDoc*);
		SROleClientDoc*		GetDoc(VOID)						{return m_Document;}

		char*							AppName(VOID)						{return m_szAppName;}
		WORD							NativeCF(VOID)					{return m_cfNative;}
		WORD							OwnerLinkCF(VOID)				{return m_cfOwnerLink;}
		WORD							ObjectLinkCF(VOID)			{return m_cfObjectLink;}

		BOOL							OLEObjOnClipboard(VOID);
	};

#endif	// !SROLEAPP_H

