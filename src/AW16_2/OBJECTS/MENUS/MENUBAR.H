#ifndef _MENUBAR_H_
#define _MENUBAR_H_

class MENUBAR 		// Menu Builder Class
{
	private:
	HWND			  hwnd;
	OBJECTID	  objid;
	HMENU		   	hBar;
	BARobj* 		barData0;   	 	// data 0
	OBJECTID*		barData1;   		// data 1
	ITMobj*			itmData0;    		// data 0

	BYTE			  tabItemText[maxEdit+1];

	public:

	BUILD ( VOID )
	  {
		hBar = 0L;
	  }

	~BUILD ( VOID );

	void Build ( HWND hwnd, OBJECTID objid );
	void EachMenu ( HMENU hmenu, OBJECTID objid, BOOL disable=FALSE);
	void APopMenu ( OBJECTID menuObject );
	void EachItem ( HMENU hmenu, OBJECTID objid);
	BOOL AppendItem ( HMENU hMenu );
	BOOL AppendSeparator ( HMENU hMenu );
	void ValidateIDs(OBJECTID objid);
	void Design_ClearAllHandles(OBJECTID objid);
	HMENU BuildHandle ( OBJECTID objId );

  };

#endif /*_MENUBAR_H_*/
