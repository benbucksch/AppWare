#if(!defined(__WinList_h))
	#define __WinList_h


class WINDOWLIST
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

/*
		HWND 	GetWindow(
			void * theObject);

		void *	 GetNextObject(
			void * theObject);

		HWND 	GetNextWindow(
			HWND hwnd);
*/

		BOOL	IsListEmpty();
};

#endif
