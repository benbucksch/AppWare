#include "almsys.h"
#include "unitree.h"
#include "stckarry.h"
#include "intdebug.h"

/////////////////////////////////////////////////////////////
// FUNCTION:			UNIQUETREE::GetInsertPosition
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:				NULL if child == parent else returns the new
//								insert position.
/////////////////////////////////////////////////////////////
LPBITREEMEMBER * UNIQUETREE::GetInsertPosition(
	LPBITREEMEMBER child)
	{
	LPBITREEMEMBER * newPosition;
	int cmpResult;

	newPosition = &root;

	while(*newPosition)
		{
		cmpResult = child->Compare(*newPosition);
		
		if(cmpResult <0)
			newPosition = &((*newPosition)->left);
		else if(cmpResult > 0)
			newPosition = &((*newPosition)->right);
    else
    	{
			//a match was found which means we're not unique, so return NULL
      newPosition = NULL;
      break;
      }
		
		I_ASSERT(newPosition);
		}

  return newPosition;
  }
