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
// FILE:					SROLSTRM.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Defines the AppWare OLE Object streams for saving and
//								retrieving of OLE objects.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SROLSTRM_H
#define SROLSTRM_H

////////////////////////////////////////////////////////////////
//
// STRUCT:	SROleStreamAux
//
// DESC:		Equivalent to an expanded OLESTREAM structure.  It is used
//					to communicate with the OLE libraries.
//
////////////////////////////////////////////////////////////////

struct SROleStreamAux
	{
	LPOLESTREAMVTBL			m_StreamVtbl;				// This MUST be the first field
	SROleStream*				m_Owner;						// Points back to object that handles
																					//  everything
	};


////////////////////////////////////////////////////////////////
//
// CLASS:		SROleStream
//
// DESC:		Manages I/O of OLE objects to and from AppWare datablocks.
//
////////////////////////////////////////////////////////////////

class SROleStream
	{
	private:
		SROleStreamAux				m_Stream;								// contains vtbls
		SROleClientObj*				m_Object;								// object we're reading/writing
		BOOL									m_Reading;							// TRUE => reading
		VOID _huge*						m_Data;									// points into object data

	public:
													SROleStream(SROleClientObj*, BOOL);		// TRUE => read
												 ~SROleStream(VOID);

		DWORD									Get(VOID*, DWORD);
		DWORD									Put(const VOID*, DWORD);

		LPOLESTREAM						OleStream(VOID)					{return (LPOLESTREAM) &m_Stream;}
		BOOL									Init(DWORD);						// pass 0 if reading
		VOID									Term(VOID);
	};

#endif	// !SROLSTRM_H

