#ifndef __STCKARRY_H__
#define __STCKARRY_H__

#include "memclass.h"

#define STACK_EMPTY	-1

class STACKARRAY:public MEMORY
	{
  private:
    void * * dataPtr;
  	int		 stackTop;
    unsigned int stackSize;
    
    BOOL GrowStack();
    
 public:
 		STACKARRAY();
    ~STACKARRAY();
 		BOOL Push(void * data);
 		void * Pop();
    void * Top();
 		
 		BOOL IsEmpty()
 			{
 			return (stackTop == STACK_EMPTY)?TRUE:FALSE;
 			}   
  };


#endif
