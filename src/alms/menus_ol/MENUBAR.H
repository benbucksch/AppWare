#ifndef _MENUBAR_H_
#define _MENUBAR_H_

class MENUBAR 		// Menu Builder Class
{
	private:
	HWND			  hwnd;
	AOBJECTID	  objid;
	HMENU		   	hBar;
	BARobj* 		barData0;   	 	// data 0
	AOBJECTID*		barData1;   		// data 1
	ITMobj*			itmData0;    		// data 0

	BYTE			  tabItemText[maxEdit+1];

	public:

	BUILD ( VOID )
	  {
		hBar = 0L;
	  }

	~BUILD ( VOID );

	void Build ( HWND hwnd, AOBJECTID objid );
	void EachMenu ( HMENU hmenu, AOBJECTID objid, BOOL disable=FALSE);
	void APopMenu ( AOBJECTID menuObject );
	void EachItem ( HMENU hmenu, AOBJECTID objid);
	BOOL AppendItem ( HMENU hMenu );
	BOOL AppendSeparator ( HMENU hMenu );
	void ValidateIDs(AOBJECTID objid);
	void Design_ClearAllHandles(AOBJECTID objid);
	HMENU BuildHandle ( AOBJECTID objId );

  };

#endif /*_MENUBAR_H_*/
