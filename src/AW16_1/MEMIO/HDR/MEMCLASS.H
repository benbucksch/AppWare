#ifndef __MEMCLASS_H
#define __MEMCLASS_H

#include <stddef.h>

class MEMORY
	{
	public:
		void far* operator new(size_t size);
		void far* operator new(size_t size, int hFile);
		void operator delete(void far* p);
	};

#endif
