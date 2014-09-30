/*//////////////////////////////////////////////////////////////
//
//					Copyright 1995, Novell, Inc.  All rights reserved
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
// FILE:					FNAMEOBJ.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Contains class definition for pathname/filename object
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef FNAMEOBJ_H
#define FNAMEOBJ_H

////////////////////////////////////////////////////////////////
//
// CLASS:		SFilePath
//
// DESCR:		Encapsulates a file path.  There are multiple pieces of this
//					path, and all can be manipulated separately.
//
//					Nomenclature:  parts of the following full path are:
//							c:\apps\wordproc\docs\mydocs.wpd
//                                           ^^^ --- extension
//                                    ^^^^^^ ------- name (no trailing '.')
//                                    ^^^^^^^^^^ --- fullname
//              ^^^^^^^^^^^^^^^^^^^^^ -------------- directory (no trailing '\')
//              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ------- pathnoext
//              ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ --- path
//
//					If there are multiple "."'s in the fullname, the rightmost
//					extension is used.  If the filename is set to one that has a
//					trailing '.', then it will be remembered, otherwise, if there
//					is no extension, then no '.' will be there.
//
//					There are no semantics associated with any portion of the
//					path (e.g., the object does not know if this is a valid path,
//					or if the last element is a directory or a file).  Also, the
//					assignment of characters to pieces of the path is done from
//					right to left.  In other words, if there is no directory
//					separator ("/" or "\"), then only the fullname exists.
//
////////////////////////////////////////////////////////////////

class SFilePath
	{
	public:
		static int		MaxLength(void);					// for both path and name

		// allow initialization of memory
		void*					operator new(size_t s, SFilePath* p)	{return p;}

									SFilePath(void);
									SFilePath(const char*);		// same as op=(char*)
								 ~SFilePath(void);

									operator const char*()		const		{return m_pszPath;}
		BOOL					operator =(const char*);	// sets path and name
		BOOL					operator =(const SFilePath&);


		char*					BeginModify(void);				// allows direct modification of path
		void					EndModify(void);					// cleans up after direct modification
		BOOL					MakeFromCurrentDirectory(void);	// uses current fullname
		BOOL					AddToEnd(const char*);		// extends path by adding filename
		BOOL					RemoveFromEnd(void);			// removes filename

		int						Length(void)							const;		// length of op char*() value

		// The "Get...()" methods return the number of chars copied, or the
		// required number if the "char*" param is NULL.
		// The "Set...()" methods return TRUE if the set operation worked
		int						GetPath(char*, int)				const;
		BOOL					GetPath(SFilePath& s)			const	{return (s = (*this));}
		int						GetPathNoExt(char*, int)	const;
		BOOL					GetPathNoExt(SFilePath&)	const;
		BOOL					SetPath(const char* p)		{return (*this) = p;}

		BOOL					HasDirectory(void)				const;
		int						GetDirectory(char*, int)	const;
		BOOL					GetDirectory(SFilePath&)	const;
		BOOL					SetDirectory(const char*);

		// Always has a fullname
		int						GetFullName(char*, int)		const;
		BOOL					GetFullName(SFilePath&)		const;
		BOOL					SetFullName(const char*);

		// Always has a name
		int						GetName(char*, int)				const;
		BOOL					GetName(SFilePath&)				const;
		BOOL					SetName(const char*);

		BOOL					HasExtension(void)				const;
		int						GetExtension(char*, int)	const;
		BOOL					GetExtension(SFilePath&)	const;
		BOOL					SetExtension(const char*);

	private:
		void					Normalize(void);				// converts to canonical form
		BOOL					MakeRoom(int);					// allocs chars to fit desired size
		void					SetupNameExt(void);			// sets up fullname,ext pointers
		void					CleanName(void);				// removes trailing dirseps, etc.

		char*					m_pszPath;							// file path goes here (always valid)
		char*					m_pszFullName;					// if not null, points to fullname
		char*					m_pszExtension;					// if not null, points to extension
		BOOL					m_fNameHasPeriod;				// TRUE => show '.' if no extension
		int						m_nPathSize;						// size of mem for pszPath
		char					m_cDummy;
	};

#endif	// !FNAMEOBJ_H
