#include "almsys.h"

#include "bitree.h"
#include "intdebug.h"
#include "stckarry.h"

BITREE::BITREE()
  {
	root=NULL;
	firstSorted = NULL;
	}

BITREE::~BITREE()
	{
	if(firstSorted)
		PurgeTree();
	}

BOOL BITREE::Add(
	LPBITREEMEMBER newNode)
	{
	LPBITREEMEMBER * parent;
	BOOL rval;

	parent = GetInsertPosition(newNode);

	if(parent)
		{
		*parent = newNode;
		rval = TRUE;
		}
	else
		{
		delete newNode;
		rval = FALSE;
		}

	return rval;
	}

LPBITREEMEMBER * BITREE::GetInsertPosition(LPBITREEMEMBER child)
	{
	LPBITREEMEMBER * newPosition;

	newPosition = &root;

	while(*newPosition)
		{
		if(child->Compare(*newPosition) <=0)
			newPosition = &((*newPosition)->left);
		else
			newPosition = &((*newPosition)->right);
 		
 		I_ASSERT(newPosition);
 		}

  return newPosition;
  }

/////////////////////////////////////////////////////////////
// FUNCTION:			BITREE::FindNode
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPBITREEMEMBER BITREE::FindNode(
	long criteria)
	{
	return FindMatch(criteria);
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			BITREE::FindMatch
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPBITREEMEMBER BITREE::FindMatch(
	long criteria)
	{
	int l_result;
	LPBITREEMEMBER node=root;
  
	while(node)
		{
		l_result = node->MatchCriteria(criteria);
	
		if(l_result > 0) //see if criteria is greater than node
			node = node->left;
		else if(l_result < 0)	//see if criteria is less than node
			node = node->right;
		else
    	{
      break;
      }
		}

	return node;
	} 

																			 																			 


BOOL BITREE::SortInOrder()
	{
  InitSort();
	InOrder();

	return (firstSorted != NULL)?TRUE:FALSE;
  }


BOOL BITREE::SortPreOrder()
	{
  InitSort();
	PreOrder();

	return (firstSorted != NULL)?TRUE:FALSE;
	}
		

//Used a stack to eliminate recursion.
void BITREE::InOrder()
	{
  STACKARRAY stackPtr;
  LPBITREEMEMBER node;
  //seed the stack with the root
  //initialize node to point to the first stack element
  node = root;
  stackPtr.Push(node);
  
  //node will always be the top of the stack
  while(node)
  	{
		if(!node->TestVisitedFlag() && node->left)
    	{
      //we're on the stack and have a left node so set our visted flag.
      node->SetVisitedFlag();
      node = node->left;
      stackPtr.Push(node);
      }
		else
    	{
     	//pop ourselves off the stack and report value
      stackPtr.Pop();
      
				
			*nextSorted = node;
			nextSorted = &node->sortPtr;
			*nextSorted = NULL;
      
      //the only place we want to clear the visited flag is when we report 
      //the value
			node->ClearVisitedFlag();
      
      if(node->right)
      	{
      	node = node->right;
				stackPtr.Push(node);
				}
			else
				{
				//set node to parent
        node = (LPBITREEMEMBER)stackPtr.Top();
				}        	
      }
    }
  }


////////////////////////////////////////////////////////////////////
void BITREE::PreOrder()
	{
  LPBITREEMEMBER node;
  STACKARRAY stackPtr;
  
  stackPtr.Push(root);
  
  while(!stackPtr.IsEmpty())
  	{
    node = (LPBITREEMEMBER)stackPtr.Pop();
    I_ASSERT(node);
    
		if(node)
			{
			*nextSorted = node;
			nextSorted = &node->sortPtr;
			*nextSorted = NULL;

			//push right node first so we'll vist after left node
			if(node->right)
				stackPtr.Push(node->right);
			
			if(node->left)
				stackPtr.Push(node->left);

			}
    }
  }


LPBITREEMEMBER BITREE::GetNextSortedMember(
	LPBITREEMEMBER preNode)
	{
	if(preNode == NULL)
		return firstSorted;
	else
		return preNode->sortPtr;
  }

void BITREE::PurgeTree()
	{
	LPBITREEMEMBER member;
	LPBITREEMEMBER prevMember;


	//clean the tree by going through the list and deleting the previous member
	member = NULL;
  prevMember = NULL;
	while(NULL != (member = GetNextSortedMember(member)))
		{
 		if(prevMember)
			delete prevMember;

    prevMember = member;
		}

	if(prevMember)
		delete prevMember;

	firstSorted = NULL;
	root = NULL;
	}

//////////////////////////////////////////////////////////////////
void BITREE::InitSort()
	{
	firstSorted = NULL;
  nextSorted = &firstSorted;
	}    
