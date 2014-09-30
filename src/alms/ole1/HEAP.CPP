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
// FILE:					HEAP.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Heap manager.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <a_alm.h>

#include "intdebug.h"
#include "heapint.h"

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

// Max allocation is 16 bytes shy of 64k so arithmetic is not LONG (is
// this actually necessary?)
#define MAXALLOC			(65536L - 16 - sizeof(HeapPage))

// Increment to grow Heap::m_pPageAry by
#define ARYGROW				10

// Global heap for MemAlloc(), MemFree().
static Heap*					g_pGlobalHeap;

//////////////// Debug stuff

#define HEAPDEBUG								// turn this off for no consistency checks

#ifdef HEAPDEBUG
#define CHECK					I_ASSERT(Check())
#else
#define CHECK
#endif

//****************************************************************
//							Entry Implementation
//****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:	Entry::Init
//
// DESCR:		Initializes the heap entry by setting up the offset
//					to the HeapPage.
//
////////////////////////////////////////////////////////////////

VOID	Entry::Init(
	HeapPage*		pPage
	)
	{
	I_ASSERT((char*) this > (char*) pPage);			// page is before all entries
	I_ASSERT(HIWORD(this) == HIWORD(pPage));		// must be in same segment

	m_pPage = (WORD) (((char*) this) - ((char*) pPage));
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Entry::Page
//
// DESCR:		Returns the page that this heap entry is on.
//
////////////////////////////////////////////////////////////////

HeapPage*		Entry::Page(
	VOID
	)
	{
	return ((HeapPage*) (((char*) this) - m_pPage));
	}

//****************************************************************
//							VarEntry Implementation
//****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:	VarEntry::Init
//
// DESCR:		Initializes the entry by setting the page offset and the
//					size of the block.  The entry is marked as used.
//
////////////////////////////////////////////////////////////////

VOID		VarEntry::Init(
	WORD				wSize,
	HeapPage*		pPage
	)
	{
	I_ASSERT(!(wSize & 1));											// can't be odd
	I_ASSERT((char*) this > (char*) pPage);			// page is before all entries
	I_ASSERT(HIWORD(this) == HIWORD(pPage));		// must be in same segment

	this->Entry::Init(pPage);
	m_Size = wSize;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	VarEntry::Next
//
// DESCR:		Returns the next VarEntry* in the list, NULL if end of list.
//
////////////////////////////////////////////////////////////////

VarEntry*		VarEntry::Next(
	VOID
	)
	{
	I_ASSERT(IsFree());							// entry MUST be free for it to be in the free
																	// list

	// Reusse m_pPage to hold a pointer to the next in free list
	return (m_pPage != 0) ?
					((VarEntry*) (((char*) this) + m_pPage)) :
					NULL;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	VarEntry::SetNext
//
// DESCR:		Sets the next pointer to the given value.  The offset to the
//					next entry is kept.  The next entry must be after this one.
//
////////////////////////////////////////////////////////////////

VOID		VarEntry::SetNext(
	VarEntry*		pNext
	)
	{
	I_ASSERT(IsFree());
	I_ASSERT(pNext == NULL || pNext > this);	// next must be after this one

	if (pNext)
		m_pPage = (WORD) (((char*) pNext) - ((char*) this));
	else
		m_pPage = 0;
	}

//****************************************************************
//							HeapPage Implementation
//****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::operator new
//
// DESCR:		Allocates a block of memory of the given size.  An ugly hack
//					here is to record the handle of the allocated block in the
//					object by casting the (void) pointer to HeapPage*.
//
////////////////////////////////////////////////////////////////

VOID*		HeapPage::operator new(
	size_t		s,
	WORD			fStatus,
	LONG			lPageSize
	)
	{
	VOID*			pMem		= NULL;
	HANDLE		hMem;
	UINT			fOpt;

	I_UNUSED(s);

	fOpt = (fStatus & HP_TASKOWNED) ? GMEM_MOVEABLE : (GMEM_MOVEABLE | GMEM_SHARE);
	hMem = GlobalAlloc(fOpt, lPageSize);

	if (hMem && ((pMem = GlobalLock(hMem)) != NULL))
		{
		// This is NOT GOOD, but, hey, it's a hack
		((HeapPage*) pMem)->m_hMem   = hMem;
		((HeapPage*) pMem)->m_fFlags = OwnsMemory;
		}

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::operator new
//
// DESCR:		"Allocates" memory for the HeapPage by returning the pointer
//					passed in.  This operator should be called if the HeapPage's
//					memory is being allocated from existing memory (such as the
//					Heap's memory block).
//
////////////////////////////////////////////////////////////////

VOID*		HeapPage::operator new(
	size_t		s,
	WORD			fStatus,							// not needed except for consistency
	VOID*			pMem
	)
	{
	I_UNUSED(s);
	I_UNUSED(fStatus);

	if (pMem != NULL)
		{
		// This is NOT GOOD, but, hey, it's a hack
		((HeapPage*) pMem)->m_hMem   = NULL;
		((HeapPage*) pMem)->m_fFlags = 0x0;			// doesn't own memory
		}

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::operator delete
//
// DESCR:		Frees the memory that the HeapPage allocated.  The ugly part
//					is to cast the pointer to a HeapPage to get both the owned
//					status and the memory handle if the memory is owned (and must
//					therefore be deleted).
//
////////////////////////////////////////////////////////////////

VOID		HeapPage::operator delete(
	VOID*			pPage
	)
	{
	HANDLE		hMem;

	if (pPage)
		{
		// This is NOT GOOD, but it masks the ugliness
		if (((HeapPage*) pPage)->m_fFlags & OwnsMemory)
			{
			hMem = ((HeapPage*) pPage)->m_hMem;
			GlobalUnlock(hMem);
			GlobalFree(hMem);
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::constructor
//
// DESCR:		Initailizes a variable sized HeapPage.  The freelist must
//					be set up to point to the rest of memory as one large
//					free block.
//
////////////////////////////////////////////////////////////////

HeapPage::HeapPage(
	WORD			fStatus,
	LONG			lPageSize
	) :
		// DON'T do m_fFlags here since the operator new has initialized it.
		m_lPageSize(lPageSize),
		m_wFixedSize(0),
		m_wFirstFree(0)
	{
	VarEntry*		pVar;

	m_fFlags |= fStatus;

	pVar = (VarEntry*) FirstEntry();
	pVar->SetSize((WORD) (lPageSize - sizeof(HeapPage)));
	pVar->MarkFree();
	pVar->SetNext((VarEntry*) NULL);

	SetFirstFree(pVar);

	CHECK;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::constructor
//
// DESCR:		Initializes a fixed size HeapPage.
//
////////////////////////////////////////////////////////////////

HeapPage::HeapPage(
	WORD			fStatus,
	WORD			wFixed,
	LONG			lPageSize
	) :
		// DON'T do m_fFlags here since the operator new has initialized it.
		m_lPageSize(lPageSize),
		m_wFirstFree(0)
	{
	m_wFixedSize  = ((wFixed + 1) & ~1) + sizeof(FixedEntry);
	m_fFlags     |= fStatus;

	CHECK;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::destructor
//
// DESCR:		Destroys a HeapPage object.  Since the object doesn't point
//					to any other memory, we don't do much here.
//
////////////////////////////////////////////////////////////////

HeapPage::~HeapPage(
	VOID
	)
	{
	CHECK;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::FirstEntry
//
// DESCR:		Returns the first entry in the HeapPage.  The first entry
//					occupies memory starting immediately after the end of
//					the HeapPage object.
//
////////////////////////////////////////////////////////////////

Entry*	HeapPage::FirstEntry(
	VOID
	)
	{
	return (Entry*) (((char*) this) + sizeof(HeapPage));
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::FirstFree
//
// DESCR:		Returns the first entry in the free list maintained
//					by the page.  If there are no elements in the free list,
//					NULL is returned.
//
////////////////////////////////////////////////////////////////

Entry*	HeapPage::FirstFree(
	VOID
	)
	{
	Entry*		pFree;

	if (m_wFirstFree == 0)
		pFree = NULL;
	else
		pFree = (Entry*) (((char*) this) + m_wFirstFree);

	return pFree;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::SetFirstFree
//
// DESCR:		Sets the first free element to the given one.  The first
//					free is recorded as an offset in bytes from the 'this'
//					pointer.
//
////////////////////////////////////////////////////////////////

VOID	HeapPage::SetFirstFree(
	Entry*		pEntry
	)
	{
	if (pEntry == NULL)
		m_wFirstFree = 0;
	else
		m_wFirstFree = (WORD) (((char*) pEntry) - ((char*) this));
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::NextFixed
//
// DESCR:		Returns the next FixedEntry.  NULL is returned at end of page.
//
////////////////////////////////////////////////////////////////

FixedEntry*		HeapPage::NextFixed(
	FixedEntry*		pCur
	)
	{
	if ((char*) pCur >= ((char*) this) + (WORD) (m_lPageSize - m_wFixedSize))
		pCur = NULL;
	else
		pCur = (FixedEntry*) (((char*) pCur) + m_wFixedSize);

	return pCur;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::NextVar
//
// DESCR:		Returns the next VarEntry.  NULL is returned at end of page.
//
////////////////////////////////////////////////////////////////

VarEntry*		HeapPage::NextVar(
	VarEntry*		pCur
	)
	{
	if ((char*) pCur >= ((char*) this) + (WORD) (m_lPageSize - pCur->Size()))
		pCur = NULL;
	else
		pCur = (VarEntry*) (((char*) pCur) + pCur->Size());

	return pCur;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::Coalesce
//
// DESCR:		Merges adjacent free blocks into one free block.  TRUE
//					is returned if any merges occurred.  The algorithm is
//					to start at a free block and look at the next block (free or not)
//					after it.  If it is free, then the size and the next pointers
//					are adjusted, and the process is repeated until no more can
//					be added to that free block.  Then the next free block is
//					processed, etc.
//
////////////////////////////////////////////////////////////////

BOOL	HeapPage::Coalesce(
	VarEntry*		pFree,							// = NULL
	BOOL				fOnceOnly						// = FALSE
	)
	{
	BOOL				fMerged		= FALSE;
	VarEntry*		pNext;

	CHECK;

	if (pFree == NULL)
		pFree = (VarEntry*) FirstFree();

	for (; pFree != NULL; pFree = pFree->Next())
		{
		// Continue adding free blocks immediately following pFree
		for (;;)
			{
			pNext = NextVar(pFree);

			if (pNext && pNext->IsFree())				// merge
				{
				pFree->SetSize(pFree->Size() + pNext->Size());
				pFree->SetNext(pNext->Next());
				fMerged = TRUE;
				}
			else
				break;
			}

		if (fOnceOnly)
			break;
		}

	CHECK;

	return fMerged;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::AllocFixed
//
// DESCR:		Searches the FixedEntries for a free one and returns it.
//					THIS SHOULD CHANGE TO USE A FREE LIST!!  This could be done
//					by maintaining a free list, where an odd value would indicate
//					the entry was part of the free list.
//
////////////////////////////////////////////////////////////////

FixedEntry*		HeapPage::AllocFixed(
	VOID
	)
	{
	FixedEntry*		pFixed;

	CHECK;

	I_ASSERT(FixedSize());

	pFixed = (FixedEntry*) FirstEntry();

	while (!pFixed->IsFree())
		{
		pFixed = NextFixed(pFixed);
		}

	if (pFixed)
		pFixed->MarkUsed(this);

	CHECK;

	return pFixed;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::AllocVar
//
// DESCR:		Searches the free list for a block large enough to
//					satisfy the request.  There is an option of using either
//					first fit or best fit.  Any extra space in the free block
//					actually selected will be split into another free block
//					(if it is at least twice the size of a VarEntry).
//
////////////////////////////////////////////////////////////////

VarEntry*		HeapPage::AllocVar(
	WORD				wSize
	)
	{
	BOOL				fBestFit;
	VarEntry*		pVar;
	VarEntry*		pLast;
	VarEntry*		pBestFit;
	VarEntry*		pBestLast;
	VarEntry*		pSplit;
	WORD				wBestFit;

	CHECK;

	I_ASSERT(!FixedSize());

	pLast = NULL;
	pVar  = (VarEntry*) FirstFree();

	fBestFit = (m_fFlags & HP_USEBESTFIT) ? TRUE : FALSE;
	wBestFit = 0;
	pBestFit = NULL;

	for (;pVar != NULL;)
		{
		if (pVar->Size() >= wSize)									// candidate
			{
			if (!fBestFit || pVar->Size() == wSize)		// found it
				break;

			else if (pBestFit == NULL || wBestFit > (pVar->Size() - wSize))
				{
				wBestFit  = pVar->Size() - wSize;
				pBestFit  = pVar;
				pBestLast = pLast;
				}
			}

		pLast = pVar;
		pVar  = pVar->Next();
		}

	if (pBestFit)
		{
		pVar  = pBestFit;
		pLast = pBestLast;
		}

	if (pVar)														// split it if necessary
		{
		if (pVar->Size() > wSize + 2 * sizeof(VarEntry))	// can't split if too small
			{
			pSplit = pVar->Split(wSize);
			pSplit->SetSize(pVar->Size() - wSize);
			pSplit->MarkFree();
			pSplit->SetNext(pVar->Next());
			}
		else
			{
			pSplit = pVar->Next();
			wSize  = pVar->Size();
			}

		if (pLast)
			pLast->SetNext(pSplit);
		else
			SetFirstFree(pSplit);

		pVar->Init(wSize, this);				// automatically marks as used
		}

	CHECK;

	return pVar;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::FreeFixed
//
// DESCR:		Frees an entry allocated by AllocFixed().  Currently this
//					just marks the entry as free.
//
////////////////////////////////////////////////////////////////

BOOL		HeapPage::FreeFixed(
	FixedEntry*		pEntry
	)
	{
	CHECK;

	I_ASSERT(pEntry->Page() == this);

	pEntry->MarkFree();

	CHECK;

	return TRUE;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::FreeVar
//
// DESCR:		Frees an entry allocated by AllocVar().  This will coalesce
//					immediately unless the HP_DELAYEDCOALESCE option has been
//					set.
//
////////////////////////////////////////////////////////////////

BOOL		HeapPage::FreeVar(
	VarEntry*		pEntry
	)
	{
	BOOL				fFreed	= FALSE;
	VarEntry*		pFree;
	VarEntry*		pLast;

	CHECK;

	I_ASSERT(pEntry->Page() == this);

	// If delayed coalescing, just search free list to link in this
	// new entry.  If coalescing, then coalesce if possible during
	// the search

	pFree = (VarEntry*) FirstFree();

	if (pFree == NULL)								// make pEntry first free.
		{
		SetFirstFree(pEntry);
		pEntry->MarkFree();
		pEntry->SetNext((VarEntry*) NULL);
		fFreed = TRUE;
		}

	else
		{
		pLast = NULL;

		while (pFree != NULL && pFree < pEntry)
			{
			pLast = pFree;
			pFree = pFree->Next();
			}

		pEntry->MarkFree();
		pEntry->SetNext(pFree);

		if (pLast)										// middle of list
			pLast->SetNext(pEntry);
		else													// head of list
			SetFirstFree(pEntry);

		if (!(m_fFlags & HP_DELAYEDCOALESCE))
			// Force a one-element only coalesce
			Coalesce(pLast, TRUE);

		fFreed = TRUE;
		}

	CHECK;

	return fFreed;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::Alloc
//
// DESCR:		Allocates a (sub)block of memory of the given size.
//
////////////////////////////////////////////////////////////////

VOID*		HeapPage::Alloc(
	WORD		wSize
	)
	{
	VOID*		pMem		= NULL;
	Entry*	pBlock;

	CHECK;

	if (wSize < MAXALLOC)
		{
		if (wSize & 0x1)
			wSize++;

		if (FixedSize())
			{
			I_ASSERT(wSize + sizeof(FixedEntry) != m_wFixedSize);

			if ((pBlock = AllocFixed()) != NULL)
				pMem = (VOID*) (((char*) pBlock) + sizeof(FixedEntry));
			}

		else
			{
			wSize += sizeof(VarEntry);

			if (((pBlock = AllocVar(wSize)) == NULL) &&
					(m_fFlags & HP_DELAYEDCOALESCE) &&
					Coalesce())
				pBlock = AllocVar(wSize);

			if (pBlock)
				pMem = (VOID*) (((char*) pBlock) + sizeof(VarEntry));
			}
		}

	CHECK;

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::Free
//
// DESCR:		Frees memory allocated by Alloc.
//
////////////////////////////////////////////////////////////////

BOOL		HeapPage::Free(
	VOID*		pMem
	)
	{
	BOOL					fFreed		= FALSE;
	VarEntry*			pVar;
	FixedEntry*		pFixed;

	CHECK;

	if (pMem != NULL)
		{
		if (FixedSize())
			{
			pFixed = (FixedEntry*) (((char*) pMem) - sizeof(FixedEntry));
			fFreed = FreeFixed(pFixed);
			}
		else
			{
			pVar   = (VarEntry*) (((char*) pMem) - sizeof(VarEntry));
			fFreed = FreeVar(pVar);
			}
		}

	CHECK;

	return fFreed;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	HeapPage::Check
//
// DESCR:		Validates the consistency of the heap page.
//
////////////////////////////////////////////////////////////////

BOOL		HeapPage::Check(
	VOID
	)
	{
	BOOL				fBad								= FALSE;
	BOOL				fFirstFound					= FALSE;
	WORD				nNonFreeListCount		= 0;
	WORD				nFreeListCount			= 0;
	VarEntry*		pFirstFree					= NULL;
	VarEntry*		pEntry;
	VarEntry*		pLast;

	// Types of validation checks possible:
	//  - all used entries have 'this' as their page
	//  - all entries in free list are marked as free
	//  - all free entries are in the free list
	//  - free list is motonically increasing
	//  - free list terminates
	//  - recorded first free is really the first free

	if (!FixedSize())
		{
		for (pEntry = (VarEntry*) FirstEntry(); pEntry != NULL;)
			{
			if (pEntry->IsFree())
				{
				if (!fFirstFound)
					{
					fFirstFound = TRUE;
					pFirstFree = pEntry;
					}
				nNonFreeListCount++;
				}
			else
				{
				if (this != pEntry->Page())
					fBad = TRUE;							// pages don't match
				}
				
			pEntry = NextVar(pEntry);
			}

		if (pFirstFree != FirstFree())
			fBad = TRUE;									// first real free != recorded first free

		for (pLast = NULL, pEntry = (VarEntry*) FirstFree();
				 pEntry != NULL;
				 pEntry = pEntry->Next())
			{
			if (!pEntry->IsFree())
				fBad = TRUE;								// item in freelist not marked as free

			if ((char*) pLast > (char*) pEntry)
				fBad = TRUE;								// not monotonically increasing; also
																		//  catches non-terminating lists

			nFreeListCount++;
			}

		if (nNonFreeListCount != nFreeListCount)
			fBad = TRUE;									// not all free are in freelist
		}

	return !fBad;
	}

//****************************************************************
//							Heap Implementation
//****************************************************************

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::operator new
//
// DESCR:		Allocates space for a new heap object.  The pagesize
//					really refers to the pagesize of HeapPages, but we
//					also use it for the Heap (which contains a HeapPage
//					also).
//
////////////////////////////////////////////////////////////////

VOID*		Heap::operator new(
	size_t			s,										// ignored
	WORD				fStatus,
	LONG				lPageSize
	)
	{
	VOID*				pMem		= NULL;
	HANDLE			hMem;
	UINT				fOpt;

	I_UNUSED(s);

	fOpt = (fStatus & HP_TASKOWNED) ? GMEM_MOVEABLE : (GMEM_MOVEABLE | GMEM_SHARE);

	if (((hMem = GlobalAlloc(fOpt, lPageSize)) != NULL) &&
			((pMem = GlobalLock(hMem)) != NULL))
		{
		((Heap*) pMem)->m_hMem = hMem;
		}

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::operator delete
//
// DESCR:		Frees up the memory allocated by operator new.  Unfortunately
//					this requires doing some (perhaps) frowned upon things to
//					get the memory handle.
//
////////////////////////////////////////////////////////////////

VOID		Heap::operator delete(
	VOID*			pMem
	)
	{
	HANDLE		hMem;

	if (pMem != NULL)
		{
		hMem = ((Heap*) pMem)->m_hMem;
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::constructor
//
// DESCR:		Initializes a new Heap object.  The only tricky part is to
//					create a HeapPage, and then use that HeapPage to hold the
//					array of pointers to HeapPages.
//
////////////////////////////////////////////////////////////////

Heap::Heap(
	WORD		fFlags,
	LONG		lPageSize
	) :
		m_Signature(HEAPSIGNATURE),
		m_fFlags(fFlags),
		m_lPageSize(lPageSize),
		m_pMainPage(NULL),
		m_pPageAry(NULL),
		m_nPageArySize(0),
		m_nPageAryCount(0),
		m_wFixed(0)
	{
	VOID*			pMem;

	// We need enough memory for 'this' and our main page
	I_ASSERT(lPageSize > sizeof(Heap) + sizeof(HeapPage));
	I_ASSERT(!(fFlags & HP_CONSTANTSIZE));

	pMem = (VOID*) (((char*) this) + sizeof(Heap));

	m_pMainPage = new (fFlags, pMem) HeapPage(fFlags, lPageSize - sizeof(Heap));

	// This shouldn't fail since we are passing in the memory
	I_ASSERT(m_pMainPage != NULL);

	AddPage(m_pMainPage);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::destructor
//
// DESCR:		Deletes all HeapPages allocated.
//
////////////////////////////////////////////////////////////////

Heap::~Heap(
	VOID
	)
	{
	int			nPage;

	for (nPage = 0; nPage < m_nPageAryCount; nPage++)
		delete m_pPageAry[nPage];
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::AddPage
//
// DESCR:		Adds a page to the HeapPage array.  This is somewhat tricky
//					since we are using the HeapPages to hold onto the HeapPage array.
//					There are some subtle things going on here:  if we need to grow
//					the array and we can't find any new memory in the existing
//					HeapPages, we can't grow the array.  In this case, we allocate
//					from the HeapPage that we wish to add.  This is not the most
//					desirable if we are trying to allocate a very large block of
//					memory and that size + the size of the HeapPage array won't
//					fit in the pagesize.
//
////////////////////////////////////////////////////////////////

BOOL	Heap::AddPage(
	HeapPage*		pPage
	)
	{
	BOOL				fAdded	= FALSE;
	VOID*				pNewAry;
	WORD				nNewSize;
	int					nPage;
	WORD				cNewSize;

	if (m_nPageAryCount >= m_nPageArySize)		// we need to grow array
		{
		nNewSize = ((m_nPageArySize / ARYGROW) + 1) * ARYGROW;
		cNewSize = nNewSize * sizeof(HeapPage*);

		if (m_pPageAry == NULL)									// we are in startup mode:  use main page
			{
			I_ASSERT(m_pMainPage != NULL);
			pNewAry = m_pMainPage->Alloc(cNewSize);
			}
		else																		// normal condition
			{
			for (nPage = 0; nPage < m_nPageAryCount; nPage++)
				{
				if ((pNewAry = m_pPageAry[nPage]->Alloc(cNewSize)) != NULL)
					break;
				}

			//?? Why don't we always use the new page if we need to alloc stuff?
			if (pNewAry == NULL)									// no room in any page, use new one
				pNewAry = pPage->Alloc(cNewSize);

			CopyMemory(pNewAry, m_pPageAry, m_nPageArySize * sizeof(HeapPage*));
			Free(m_pPageAry);											//!! This better not free pages
			}

		m_pPageAry     = (HeapPage**) pNewAry;
		m_nPageArySize = nNewSize;
		}

	if (m_nPageAryCount < m_nPageArySize)
		{
		m_pPageAry[m_nPageAryCount] = pPage;
		m_nPageAryCount++;

		fAdded = TRUE;
		}

	return fAdded;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::FindPage
//
// DESCR:		Returns the HeapPage that the memory is on.  This is done
//					by backing off either sizeof(FixedEntry) or sizeof(VarEntry)
//					and requesting the Page().
//
////////////////////////////////////////////////////////////////

HeapPage*		Heap::FindPage(
	VOID*				pMem
	)
	{
	HeapPage*		pPage		= NULL;
	Entry*			pEntry;

	CHECK;

	if (pMem != NULL)
		{
		if (FixedSize())
			pEntry = (Entry*) (((char*) pMem) - sizeof(FixedEntry));
		else
			pEntry = (Entry*) (((char*) pMem) - sizeof(VarEntry));

		pPage = pEntry->Page();

#ifdef ADBG_DEBUG
		BOOL		fOk		= FALSE;

		// Perform a check on the page to make sure we have a good page
		for (int nPage = 0; nPage < m_nPageAryCount; nPage++)
			{
			if (m_pPageAry[nPage] == pPage)
				{
				fOk = TRUE;
				break;
				}
			}

		if (!fOk)
			pPage = NULL;

		ADBG_ASSERT_E(fOk, , "Invalid HeapPage");
#endif
		}

	return pPage;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::Alloc
//
// DESCR:		Allocates memory from a HeapPage and returns in.  There is
//					a possibility of abnormal failure here if the caller is
//					requesting a large block of memory and we need to grow the
//					HeapPage array and the memory allocated for the grown array
//					comes from the new HeapPage.
//
////////////////////////////////////////////////////////////////

VOID*		Heap::Alloc(
	WORD		wSize,
	WORD		fFlags
	)
	{
	VOID*				pMem;
	HeapPage*		pPage;
	int					nPage;

	for (nPage = 0; nPage < m_nPageAryCount; nPage++)
		{
		if ((pMem = m_pPageAry[nPage]->Alloc(wSize)) != NULL)
			break;
		}

	if (pMem == NULL)						// try allocating a new page
		{
		if (FixedSize())
			pPage = new (m_fFlags, m_lPageSize) HeapPage(m_fFlags, m_wFixed,
																										m_lPageSize);
		else
			pPage = new (m_fFlags, m_lPageSize) HeapPage(m_fFlags, m_lPageSize);

		if (pPage)
			{
			if (AddPage(pPage))
				{
				pMem = pPage->Alloc(wSize);
				I_ASSERT(pMem != NULL);			// very unusual condition, but possible
				}
			else
				delete pPage;
			}
		}

	if (pMem && (fFlags & HP_ZEROINIT))
		FillMemory(pMem, 0, wSize);

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::Free
//
// DESCR:		Frees memory allocated by Alloc().
//
////////////////////////////////////////////////////////////////

BOOL		Heap::Free(
	VOID*				pMem
	)
	{
	BOOL				fFreed	= FALSE;
	HeapPage*		pPage;

	if (pMem != NULL)
		{
		pPage = FindPage(pMem);

		if (pPage)
			fFreed = pPage->Free(pMem);
		}

	return fFreed;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::OwnsMem
//
// DESCR:		Returns TRUE if the heap has allocated the given memory.
//
////////////////////////////////////////////////////////////////

BOOL		Heap::OwnsMem(
	VOID*		pMem
	)
	{
	BOOL			fOwns		= FALSE;

	if (pMem)
		fOwns = (FindPage(pMem) != NULL);

	return fOwns;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	Heap::Check
//
// DESCR:		Returns TRUE if the heap has no inconsistencies.
//
////////////////////////////////////////////////////////////////

BOOL		Heap::Check(
	VOID
	)
	{
	BOOL			fOk		= TRUE;

	for (int nPage = 0; nPage < m_nPageAryCount; nPage++)
		{
		if (!m_pPageAry[nPage]->Check())
			{
			fOk = FALSE;
			break;
			}
		}

	return fOk;
	}

//****************************************************************
//							Simple Interface Implementation
//****************************************************************

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapCreate
//
// DESCR:			Creates a new heap.
//
////////////////////////////////////////////////////////////////

LONG		HeapCreate(
	LONG		lPageSize,
	WORD		fFlags
	)
	{
	return (LONG) (new (fFlags, lPageSize) Heap(fFlags, lPageSize));
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapDestroy
//
// DESCR:			Destroys a heap created with HeapCreate
//
////////////////////////////////////////////////////////////////

BOOL		HeapDestroy(
	LONG		hHeap
	)
	{
	Heap*		pHeap		= (Heap*) hHeap;
	BOOL		fKilled;

	if (pHeap->IsAHeap())
		{
		delete (Heap*) hHeap;
		fKilled = TRUE;
		}
	else
		fKilled = FALSE;

	return fKilled;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapCheck
//
// DESCR:			Checks a heap for internal consistency.
//
////////////////////////////////////////////////////////////////

BOOL		HeapCheck(
	LONG		hHeap
	)
	{
	Heap*		pHeap		= (Heap*) hHeap;
	BOOL		fOk			= FALSE;

	if (pHeap->IsAHeap())
		fOk = pHeap->Check();

	return fOk;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapAlloc
//
// DESCR:			Allocates memory from a heap.
//
////////////////////////////////////////////////////////////////

VOID*		HeapAlloc(
	LONG		hHeap,
	WORD		wBytes,
	WORD		fFlags
	)
	{
	Heap*		pHeap		= (Heap*) hHeap;
	VOID*		pMem		= NULL;

	if (pHeap->IsAHeap())
		{
		pMem = pHeap->Alloc(wBytes, fFlags);
		}

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapFree
//
// DESCR:			Frees memory allocated by HeapAlloc
//
////////////////////////////////////////////////////////////////

BOOL		HeapFree(
	LONG		hHeap,
	VOID*		pMem
	)
	{
	Heap*		pHeap		= (Heap*) hHeap;
	BOOL		fFreed;

	if (pHeap->IsAHeap())
		fFreed = pHeap->Free(pMem);
	else
		fFreed = FALSE;

	return fFreed;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	HeapOwns
//
// DESCR:			Returns TRUE if the heap owns the memory
//
////////////////////////////////////////////////////////////////

BOOL		HeapOwns(
	LONG		hHeap,
	VOID*		pMem
	)
	{
	Heap*		pHeap		= (Heap*) hHeap;
	BOOL		fOwns;

	if (pHeap->IsAHeap())
		fOwns = pHeap->OwnsMem(pMem);
	else
		fOwns = FALSE;

	return fOwns;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	MemAlloc
//
// DESCR:			Allocates memory from the global heap
//
////////////////////////////////////////////////////////////////

VOID*		MemAlloc(
	WORD		wBytes
	)
	{
	VOID*		pMem;

	if (g_pGlobalHeap == NULL)
		g_pGlobalHeap = new (0, MAXALLOC) Heap(0, MAXALLOC);

	if (g_pGlobalHeap)
		pMem = g_pGlobalHeap->Alloc(wBytes, HP_ZEROINIT);
	else
		pMem = NULL;

	return pMem;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	MemFree
//
// DESCR:			Frees memory from the global heap
//
////////////////////////////////////////////////////////////////

BOOL		MemFree(
	VOID*		pMem
	)
	{
	BOOL		fFreed;

	if (g_pGlobalHeap)
		fFreed = g_pGlobalHeap->Free(pMem);
	else
		fFreed = FALSE;

	return fFreed;
	}

//****************************************************************
//							Test Code
//****************************************************************

#ifdef HEAPDEBUG

////////////////////////////////////////////////////////////////
//
// FUNCTION:	TestLogComment
//
// DESCR:			Logs a comment to the file
//
////////////////////////////////////////////////////////////////

VOID _cdecl		TestLogComment(
	LPSTR		pszFmt,
	...
	)
	{
	static BOOL		logit					= FALSE;
	va_list				va;
	char					szMsg[256];
	FILE*					f;

	if (logit)
  	{
		f = fopen("test.log", "a+");

		if (f)
			{
			va_start(va, pszFmt);

			wvsprintf(szMsg, pszFmt, va);

			fprintf(f, szMsg);

			fclose(f);
			}
		}

	va_end(va);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	TestFailure
//
// DESCR:			Outputs a message indicating that the test has failed.
//
////////////////////////////////////////////////////////////////

VOID	TestFailure(
	LPSTR			pszMsg
	)
	{
	MessageBox(NULL, pszMsg, "TestHeap Failure", MB_OK);
	TestLogComment(pszMsg);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	TestNormalUsage
//
// DESCR:			Performs "normal" usage tests on the heap
//
////////////////////////////////////////////////////////////////

VOID	TestNormalUsage(
	Heap*		pTestHeap
	)
	{
	VOID**	pPtrs;
	VOID*		pMem;
	LONG		lTime;
	int			nRemembered;
	int			nCur;
	WORD		wSize;

	// Grow a list of remembered pointers from 1 to n, and for each
	// number, allocate and free a large number of blocks.  For example,
	// for n == 1, allocate one block and free it.  For n == 2, allocate
	// two blocks, then free one, then allocate another, free the second,
	// etc.

	for (nRemembered = 1; nRemembered < 1000; nRemembered++)
		{
		TestLogComment("Starting pass #%d\n", nRemembered);

		pPtrs = (VOID**) pTestHeap->Alloc(nRemembered * sizeof(VOID*),
																			HP_ZEROINIT);

		if (pPtrs)
			{
			nCur = 0;

			TestLogComment("Starting alloc/free loop of 64k\n");

			// Allocate 64K blocks (freeing some along the way)
			for (lTime = 0; lTime < 65536L; lTime++)
				{
				// Allocate a randomly sized block
				wSize = rand() % 31000 + 1;

				TestLogComment("\tAllocating block #%ld of %d bytes:  ", lTime, wSize);

				pMem = pTestHeap->Alloc(wSize, 0);

				if (pMem != NULL)
					{
					TestLogComment("Successful:  ptr = %lx\n", pMem);

					if (pPtrs[nCur])
						{
						TestLogComment("\tFreeing ptr %lx:  ", pPtrs[nCur]);
						if (!pTestHeap->Free(pPtrs[nCur]))
							{
							TestLogComment("Failure\n");
							TestFailure("Free of pPtrs[nCur] failed");
							}
						else
							TestLogComment("Success\n");
						}

					pPtrs[nCur++] = pMem;
					nCur %= nRemembered;
					}
				else
					{
					TestLogComment("Failure\n");
					TestFailure("Unable to allocate memory");
					}
				}

			// now free all
			TestLogComment("\n\tNow freeing all remaining\n");
			for (lTime = nRemembered; lTime > 0; lTime--)
				{
				TestLogComment("\tFreeing %lx:  ", pPtrs[nCur]);
				if (!pTestHeap->Free(pPtrs[nCur++]))
					{
					TestLogComment("Failure\n");
					TestFailure("Free of pPtrs[nCur] failed (2)");
					}
				else
					TestLogComment("Success\n");
				nCur %= nRemembered;
				}
			}
		else
			TestFailure("Unable to allocate mem for remembered ptrs");
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	TestHeapManager
//
// DESCR:			Performs some tests on the heap manager.
//
////////////////////////////////////////////////////////////////

VOID	TestHeapManager(
	VOID
	)
	{
	Heap*			pHeap;

	// Assumption is that testing one heap of each type (fixed, var) is
	// sufficient.
	//
	// Types of tests:
	//	Normal usage tests
	//		Allocating, freeing lots of blocks
	//	Abnormal tests
	//		Double freeing
	//		Freeing invalid memory
	//		

	// Normal usage
	pHeap = new (0, 32000) Heap(0, 32000);
	TestNormalUsage(pHeap);
	delete pHeap;

	pHeap = new (0, 32000) Heap(HP_DELAYEDCOALESCE, 32000);
	TestNormalUsage(pHeap);
	delete pHeap;

	pHeap = new (0, 32000) Heap(HP_USEBESTFIT, 32000);
	TestNormalUsage(pHeap);
	delete pHeap;

	pHeap = new (0, 32000) Heap(HP_USEBESTFIT | HP_DELAYEDCOALESCE, 32000);
	TestNormalUsage(pHeap);
	delete pHeap;
	}

#endif

