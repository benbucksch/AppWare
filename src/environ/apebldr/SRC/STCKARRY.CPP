#include <almsys.h>
#include "stckarry.h"
#include "intdebug.h"

#define STACK_INC	1024
#define STACK_MAX	(0xffffffff / sizeof(void far*))

///////////////////////////////////////////////////////////////
STACKARRAY::STACKARRAY():
	dataPtr(NULL),stackTop(STACK_EMPTY), stackSize(0)
	{  
  }
  
///////////////////////////////////////////////////////////////
STACKARRAY::~STACKARRAY()
	{
  if(stackSize > 0)
  	{
    if((HANDLE)dataPtr)
    	{
      GlobalFree((HANDLE)dataPtr);
      }
    else
    	I_ASSERT(FALSE);
    }
  }

/////////////////////////////////////////////////////////////////
BOOL STACKARRAY::GrowStack()
	{
  BOOL rVal=FALSE;
  
 	I_ASSERT(stackSize <= STACK_MAX);
  
  if(stackSize < STACK_MAX)
  	{
	  stackSize += STACK_INC;
  
	  if(stackSize > STACK_MAX)
  		stackSize = STACK_MAX;
    
	  if(dataPtr)
	  	{
	    dataPtr = (void far* far*)GlobalReAlloc((HANDLE)dataPtr, stackSize * sizeof(void far*), 
	    	GMEM_MOVEABLE|GMEM_ZEROINIT); 
	    }
	  else
	  	{
	    dataPtr = (void far* far*)GlobalAlloc(GPTR, stackSize * sizeof(void far*));
	    }
	    
	  if(dataPtr)
    	rVal = TRUE;
    else
    	I_ASSERT(FALSE);
  	}
  return rVal;  
  }


///////////////////////////////////////////////////////////////
BOOL STACKARRAY::Push(void far* data)
	{
  BOOL rVal = TRUE;
  
  if(stackTop == ((int)stackSize-1))
  	rVal = GrowStack();
    
  I_ASSERT(stackTop < ((int)stackSize-1));
  
  if(rVal)
  	{
    stackTop++;
    dataPtr[stackTop] = data;
    }
    
  return rVal;
  }

///////////////////////////////////////////////////////////////
void far* STACKARRAY::Pop()
	{
  void far* rVal = NULL;
  
 	I_ASSERT(stackTop <= ((int)stackSize-1));
  
  if(stackTop > STACK_EMPTY)
  	{
  	rVal = dataPtr[stackTop];
  	stackTop--;
  	}
    
  return rVal;
  }

////////////////////////////////////////////////////////////////
void far* STACKARRAY::Top()
	{
  void far* rVal = NULL;
  
  I_ASSERT(stackTop <= ((int)stackSize-1));
  
  if(stackTop > STACK_EMPTY)
  	rVal = dataPtr[stackTop];
    
  return rVal;
  }
