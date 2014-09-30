#ifndef __MEMCLASS_H
#define __MEMCLASS_H

#include <stddef.h>

class MEMORY
	{
  private:
    #ifdef _MSC_VER
    //padding required when compiling in MICROSOFT since
    //Borland used to put in an extra byte to represent an
    //empty class.
    char  unusedMicrosoftPadding;
    #endif

	public:
		void * operator new(size_t size);
		void * operator new(size_t size, HANDLE hFile);
		void operator delete(void * p);
	};

#endif
