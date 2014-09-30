#if(!defined(__WinList_h))
	#define __WinList_h

#include "memclass.h"

class WINDOWLIST:public MEMORY
{
	private:
		void * head;
		void * tail;

	public:
		WINDOWLIST();

		BOOL 	Add(
			HWND hwnd,
			void *);

		BOOL 	Remove(
			HWND hwnd);

		void * GetObject(
			HWND hwnd);

		HWND 	GetWindow(
			void * theObject);

		void *	 GetNextObject(
			void * theObject);

		HWND 	GetNextWindowInList(
			HWND hwnd);

		BOOL	IsListEmpty();

		void * GetTailObject(
			HWND * windowPtr);

		BOOL RemoveByPtr(
			void *  ptr);

		BOOL RemoveTail();

		void PurgeList();
	};

#endif
