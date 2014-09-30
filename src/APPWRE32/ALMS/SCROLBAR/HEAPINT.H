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
// FILE:					HEAPINT.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Internal class definitions for memory heap.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef HEAPINT_H
#define HEAPINT_H

#include <stdlib.h>					// size_t
#include "heap.h"

// Define the heap signature.  This should be a relatively unique ID that
// would have a low possibility of occurring at random.
#define HEAPSIGNATURE			3141592653UL

class HeapPage;

////////////////////////////////////////////////////////////////
//
// CLASS:		Entry
//
// DESCR:		Defines base-level heap entry.  Heap entries are either
//					fixed in size, or variable in size.  Both have as their
//					first two bytes an offset back to the HeapPage that owns
//					them.
//
////////////////////////////////////////////////////////////////

class Entry
	{
	protected:
		WORD				m_pPage;						// page that this block is in (subtract from 'this')

								Entry(VOID);				// disable new & delete since we are
							 ~Entry(VOID);				//  going to require casting

	public:
		VOID				Init(HeapPage* page);

		HeapPage*		Page(VOID);
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		FixedEntry
//
// DESCR:		Defines a variable sized record.  Free blocks are marked
//					by a size that is odd.  The free list is maintained by the
//					parent's class m_pPage field.
//
////////////////////////////////////////////////////////////////

class FixedEntry : public Entry
	{
	private:
								FixedEntry(VOID);		// disable new & delete since we are
							 ~FixedEntry(VOID);		//  going to require casting to VarEntry

	public:

		BOOL				IsFree(VOID)						{return (m_pPage == 0);}
		VOID				MarkFree(VOID)					{m_pPage = 0;}
		VOID				MarkUsed(HeapPage* p)		{Init(p);}
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		VarEntry
//
// DESCR:		Defines a variable sized record.  Free blocks are marked
//					by a size that is odd.  The free list is maintained by the
//					parent's class m_pPage field.
//
////////////////////////////////////////////////////////////////

class VarEntry : public Entry
	{
	private:
		int				m_Size;						// size of block (including Entry overhead)

								VarEntry(VOID);		// disable new & delete since we are
							 ~VarEntry(VOID);		//  going to require casting to VarEntry

	public:
		VOID				Init(WORD size, HeapPage* page);

		VarEntry*		Split(WORD size)			{return (VarEntry*) (((char*) this) + size);}

		int				Size(VOID)						{return (m_Size & ~1);}
		VOID				SetSize(int s)				{m_Size = (s | (m_Size & 1));}

		BOOL				IsFree(VOID)					{return (m_Size & 1);}
		VOID				MarkFree(VOID)				{m_Size |=  1;}
		VOID				MarkUsed(VOID)				{m_Size &= ~1;}

		VarEntry*		Next(VOID);
		VOID				SetNext(VarEntry*);
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		HeapPage
//
// DESCR:		Heap pages are the entities that contain the heap entries.
//
////////////////////////////////////////////////////////////////

class HeapPage
	{
	private:
		HANDLE			m_hMem;												// handle from GlobalAlloc
		WORD				m_fFlags;											// class enum | passed in
		WORD				m_wFixedSize;									// size of fixed allocation
		LONG				m_lPageSize;									// size of this object
		WORD				m_wFirstFree;									// head of free list

		VOID*				operator new(size_t)					{return NULL;}

		FixedEntry*	NextFixed(FixedEntry*);
		VarEntry*		NextVar(VarEntry*);
		BOOL				Coalesce(VarEntry* = NULL, BOOL = FALSE);

		FixedEntry*	AllocFixed(VOID);
		VarEntry*		AllocVar(WORD wSize);

		BOOL				FreeFixed(FixedEntry*);
		BOOL				FreeVar(VarEntry*);

		Entry*			FirstEntry(VOID);
		Entry*			FirstFree(VOID);
		VOID				SetFirstFree(Entry*);

		enum
			{
			OwnsMemory	= 0x100											// page owns memory it sits on
			};

	public:

		VOID*				operator new(size_t, WORD, LONG);			// does allocate
		VOID*				operator new(size_t, WORD, VOID*);		// doesn't allocate
		VOID				operator delete(VOID*);

								// used for VarEntry heappages
								HeapPage(WORD fStatus, LONG lPageSize);
								// used for FixedEntry heappages
								HeapPage(WORD fStatus, WORD wFixed, LONG lPageSize);
							 ~HeapPage(VOID);

		BOOL				FixedSize(VOID)						{return (m_fFlags & HP_CONSTANTSIZE);}

		VOID*				Alloc(WORD);
		BOOL				Free(VOID*);

		BOOL				Check(VOID);									// check for consistency
	};


////////////////////////////////////////////////////////////////
//
// CLASS:		Heap
//
// DESCR:		The heap manages a set of HeapPages.  It does this by maintaining
//					an array of pointers to heap pages.  The first HeapPage is
//					placed on the same block as the Heap.
//
////////////////////////////////////////////////////////////////

class Heap
	{
	private:
		LONG				m_Signature;						// unique signature for IsAHeap
		HANDLE			m_hMem;									// handle of OS block
		WORD				m_fFlags;								// passed in
		LONG				m_lPageSize;						// passed in
		HeapPage*		m_pMainPage;						// HeapPage that occupies same block as this
		HeapPage**	m_pPageAry;							// array of HeapPage pointers
		int					m_nPageArySize;					// size of above array
		int					m_nPageAryCount;				// # valid in above array
		WORD				m_wFixed;

		VOID*				operator new(size_t)		{return NULL;}
		BOOL				AddPage(HeapPage*);
		HeapPage*		FindPage(VOID*);

	public:
		VOID*				operator new(size_t, WORD, LONG);
		VOID				operator delete(VOID*);

								Heap(WORD fFlags, LONG lPageSize);
								Heap(WORD fFlags, WORD wFixed, LONG lPageSize);
							 ~Heap(VOID);

		BOOL				IsAHeap(VOID)						{return (m_Signature == HEAPSIGNATURE);}

		BOOL				FixedSize(VOID)					{return (m_fFlags & HP_CONSTANTSIZE);}

		VOID*				Alloc(WORD wSize, WORD wFlags);
		BOOL				Free(VOID*);
		BOOL				OwnsMem(VOID*);
		
		BOOL				Check(VOID);									// check for consistency
	};

#endif	// HEAPINT_H
