#if(!defined(__WinList_h))
	#define __WinList_h

#include "memclass.h"

class WINDOWLIST:public MEMORY
{
	private:
		void far* head;
		void far* tail;

	public:
		WINDOWLIST();

		BOOL 	Add(
			HWND hwnd,
			void far*);

		BOOL 	Remove(
			HWND hwnd);

		void far* GetObject(
			HWND hwnd);

		HWND 	GetWindow(
			void far* theObject);

		void far*	 GetNextObject(
			void far* theObject);

		HWND 	GetNextWindow(
			HWND hwnd);

		BOOL	IsListEmpty();
};

#endif
