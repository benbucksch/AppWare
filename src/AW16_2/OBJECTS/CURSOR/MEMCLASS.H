#if(!defined __memclass_h)
	#define __memclass_h
#include <stddef.h>


class MEMORY
	{
	public:
		void far* operator new(size_t size);
		void far* operator new(size_t size, int hFile);
		void operator delete(void far* p);
	};


#endif
