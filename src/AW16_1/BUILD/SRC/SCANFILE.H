/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:				SCANFILE.H
//
// AUTHOR:			Scott McCarty
//
// DESCRIPTION:	Contains class declarations for scanfile module.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef SCANFILE_H
#define SCANFILE_H

////////////////////////////////////////////////////////////////
//
// ENUM:		ScanMode
//
// DESCR:		Defines the different types of scan mode.  Scan modes
//					determine how file references with a file are determined.
//
////////////////////////////////////////////////////////////////

enum ScanMode
	{
	cppScan,
	rcScan
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		FileQueue
//
// DESCR:		Maintains a queue of files to look at during creation
//					of dependencies.
//
////////////////////////////////////////////////////////////////

class FileQueue : private CPtrList
	{
	public:
									FileQueue(void) : CPtrList()	{}
								 ~FileQueue(void)               {}

		BOOL					Empty(void)										{return IsEmpty();}
		void					Push(void* p)									{AddHead(p);}
		void*					Pop(void)											{return RemoveTail();}
		BOOL					InQueue(void* p)							{return (Find(p) != NULL);}
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		FileDeps
//
// DESCR:		Maintains a list of FileNode* dependencies for a given file.
//
////////////////////////////////////////////////////////////////

class FileDeps : public CPtrList
	{
	public:
									FileDeps(void) : CPtrList()	{}
								 ~FileDeps(void)              {}

		void					Add(void* p)								{AddTail(p);}
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		FileNode
//
// DESCR:		Contains all information about a file, including what files
//					it references.
//
////////////////////////////////////////////////////////////////

class FileNode
	{
	private:

		enum
			{
			scanned			= 0x1,											// have called Scan()
			wasSearched	= 0x2,											// have searched for path
			validPath		= 0x4												// found path after searching
			};

		int								m_status;							// one of enums
		ScanMode					m_ScanMode;
		int								m_nType;
		CString						m_Name;
		CString						m_Path;
		BOOL							m_fTerminal;					// a "system" file (e.g. <stdio.h>)
		CPtrList					m_Files;							// list of files in this file

		BOOL							IsNode(long, char*, int&, CString&);
		void							FindPath(void);

	public:
											FileNode(const char*, int, ScanMode);
										 ~FileNode(void);

		int								Type(void)						{return m_nType;}
		CString&					Name(void)						{return m_Name;}
		CString&					Path(void)						{return m_Path;}
		ScanMode					Mode(void)						{return m_ScanMode;}
		BOOL							ValidPath(void);
		BOOL							Terminal(void);

		void							AddToQueue(FileDeps&, FileQueue&);
		void							Scan(void);

		// for m_nType:
		enum types
			{
			csource,															// main C/C++ source file
			rcsource,															// main RC source file
			stdinclude,														// C/C++/RC std lib directive
			locinclude,														// C/C++/RC local directive
			rcterminal														// RC terminal file reference
			};
	};


#endif	// !SCANFILE_H
