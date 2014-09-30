#include "b_includ.h"

////////////////////////////////////////////
// Version Check Stuff
////////////////////////////////////////////

#define BF_PUSHBUTTON					0x00000001L
#define BF_CHECKBOX   	      0x00000002L
#define BF_RADIOBUTTON  	    0x00000004L
#define BF_INVISIBLE      	  0x00000008L
#define BF_BITMAP           	0x00000010L
#define BF_NORMAL	           	0x00000020L
#define BF_DEFAULT	          0x00000040L
#define BF_CANCEL   	        0x00000080L
#define BF_ENABLED    	      0x00000100L
#define BF_SELECTED     	    0x00000200L
#define BF_INVERTS        	  0x00000400L
#define BF_HASALTKEY        	0x00000800L
#define BF_INVERTED	         	0x00001000L
#define BF_CAPTUREDBYKEY 	    0x00002000L

#define	BF_BMP_CROP						0x00010000L
#define	BF_BMP_STRETCH				0x00020000L
#define	BF_BMP_ORIGPROP				0x00040000L	
#define	BF_BMP_NORMAL_EXISTS	0x00080000L
#define	BF_BMP_FOCUSED_EXISTS	0x00100000L
#define	BF_BMP_PRESSED_EXISTS	0x00200000L
#define	BF_BMP_GRAYED_EXISTS	0x00400000L

#define BUT_TITLE_LEN			31


struct BUTTON_100
	{
	int							version;
	char						title[BUT_TITLE_LEN+1];
	long						style;
	char						cmdKeyUnused;
	BOOL						beingTracked;
	RECT						visualRect;   	// in screen coords
	ATTRIBUTES    	attributes;   //Font, foreground, background
	int							cmdKey;  //used for bitmap & invisible buttons
	HANDLE					unused2;
	HANDLE					unused3;
	HANDLE					unused4;
	};
typedef BUTTON_100 far * LPBUTTON_100;


long CheckButtonObjectVersion(
	OBJECTID								oiButton)
	{
	LPBUTTON								but;
	LPBUTTON_100						but100;
	short										version;
	char huge *							hptr;
	short										i;
	long										rVal = A_OK;
	LPATTRIBUTES						pAttributes;

	but = LockButtonObject(oiButton);
	I_ASSERT_POINTER(but, return A_ERROR);
	version = but->m_version;
	UnlockButtonObject(oiButton);

	if(version<200)
		{
		but100 = (LPBUTTON_100)AObjLockData(oiButton, 0);
		but = (LPBUTTON)farmalloc(sizeof(BUTTON));

		but->m_version = 200;
		but->m_oiSelf = oiButton;
		lstrcpy(but->m_szTitle, but100->title);
		but->m_style = but100->style;
		but->m_bBeingTracked = FALSE;
		SetRect(&but->m_rcTracking, 0, 0, 0, 0);
		pAttributes = new ATTRIBUTES(TRUE);
		memcpy(&but->m_attributes, pAttributes, sizeof(ATTRIBUTES));
		delete pAttributes;
		but->m_accelVKey = LOWORD(but100->cmdKey);
		but->m_accelMKey = HIWORD(but100->cmdKey);
		but->SetTitleHasBeenEdited(TRUE);	//so we don't change the title
		AObjUnlockData(oiButton, 0);

		AObjSetData(oiButton, I_DATA_STYLE, but, sizeof(BUTTON));
		farfree(but);
		//Got the button style, now do the bitmaps

		but = LockButtonObject(oiButton);
		I_ASSERT_POINTER(but, return A_ERROR);

		if (but->GetButtonType() == OBUTN_BITMAP)
			{
			for (i=1; i<=4; i++)\
				{
				hptr = (char huge *)AObjLockData(oiButton, i);
				huge_memmove(hptr, (hptr+1), AObjGetDataSize(oiButton, i)-1);
				AObjUnlockData(oiButton, i);
				}
			}
		rVal = A_MODIFIED;
		
		UnlockButtonObject(oiButton);
		}
		
	return rVal;
	}


