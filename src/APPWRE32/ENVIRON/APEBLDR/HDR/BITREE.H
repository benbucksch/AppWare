#ifndef __BITREE_H
#define __BITREE_H

#include "memclass.h"
#include "classdef.h"

#define BT_VISITED		0x0001

class BITREEMEMBER:public MEMORY
	{
	friend BITREE;
	friend UNIQUETREE;

	private:
		LPBITREEMEMBER  left;
		LPBITREEMEMBER  right;
		LPBITREEMEMBER  sortPtr;
    unsigned int		status;

	public:
		BITREEMEMBER():left(NULL), right(NULL), sortPtr(NULL), status(0)
			{
      }
	
		virtual ~BITREEMEMBER()
			{
			}

		virtual int Compare(
			LPBITREEMEMBER node) = 0;

		virtual int MatchCriteria(
			long criteria)
			{
			return -1; //default to no match
			}
      
    void SetVisitedFlag()
    	{
      status |= BT_VISITED;
      }
      
    void ClearVisitedFlag()
    	{
      status &= ~BT_VISITED;
      }
      
    BOOL TestVisitedFlag()
    	{
      return (status & BT_VISITED)?TRUE:FALSE;
      }

	};


class BITREE:public MEMORY
	{
	private:
		LPBITREEMEMBER firstSorted;
		LPBITREEMEMBER * nextSorted;

	void InOrder();

	LPBITREEMEMBER FindMatch( 
		long criteria);

	void InitSort();
  
	protected:
		LPBITREEMEMBER root;

	 	virtual	LPBITREEMEMBER * GetInsertPosition(
			LPBITREEMEMBER child);

	
	public:
		BITREE();

		~BITREE();

		virtual BOOL Add(
			LPBITREEMEMBER newNode);

		BOOL SortInOrder();

		BOOL SortPreOrder();

		void PreOrder();

		LPBITREEMEMBER GetNextSortedMember(
			LPBITREEMEMBER preNode);

		void PurgeTree();

		BOOL EmptyTree()
			{
			return (root == NULL)?TRUE:FALSE;
			}

		LPBITREEMEMBER FindNode(
			long criteria);
		
	};
#endif
