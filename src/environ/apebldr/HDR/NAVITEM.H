///////////////////////////////////////////////////////////////////////////
//	This class is designed to be inherited, using multiple inheritance, by
//	all objects that are shown in the navigator window.  The ShowItem
//  pure virtual function allows the navigator to show the item without
//	nowing the type of item.  Isn't OOP great!.
///////////////////////////////////////////////////////////////////////////
#if(!defined(__navitem_h))
#define __navitem_h

class	NAVIGATORITEM
	{
	public:
	virtual void ShowItem(void) = 0;
	};

#endif
