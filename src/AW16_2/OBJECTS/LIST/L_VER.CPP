#include "l_includ.h"


struct ATTRIBUTES_VERSION2
	{
	int					fontHeight;
	char				fontFace[LF_FACESIZE];
	int					fontWeight;				//FW_BOLD, FW_EXTRABOLD....
	BOOL				italic;
	BOOL				underlined;
	BOOL				struckout;
	COLORREF		foreground;
	COLORREF		background;
	};

struct LIST_VERSION2
	{
	int											version;
	LONG										style;
	int											nItems;
	int											curSelection;
	LONG										stringsSize;	//number of characters in the string table
	ATTRIBUTES_VERSION2			attributes;
	OBJECTID								oiText;	//id of text object to send selection to
	OBJECTID								oiList;	//id of this object
	};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

extern long CheckListObjectVersion(
	OBJECTID								oiList)
	{
	LIST_VERSION2 *					list_v2;
	LPLIST									list;
	short										version;
	long										rVal = A_OK;

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return A_ERROR);
	version = list->m_version;
	UnlockListObject(oiList);

	if (version < 100)
		{
		list_v2 =   ((LIST_VERSION2 *)AObjLockData(oiList, I_DATA_STYLE));
		list = (LPLIST)farmalloc(sizeof(LIST));

		list->m_version														= 100;
		list->m_oiSelf														= oiList;
		list->m_style															= list_v2->style;
		list->m_nItems														= list_v2->nItems;
		list->m_curSelection											= list_v2->curSelection;
		list->m_lStringsSize											= list_v2->stringsSize;	
		list->m_oiText														= list_v2->oiText;

		list->m_attributes.AttrSetFontHeightTMM((list_v2->attributes.fontHeight *  2.645833333333) - 0.5);
		list->m_attributes.AttrSetFontFace(list_v2->attributes.fontFace);
		list->m_attributes.AttrSetFontWeight(list_v2->attributes.fontWeight);
		list->m_attributes.AttrSetItalic(list_v2->attributes.italic);
		list->m_attributes.AttrSetUnderlined(list_v2->attributes.underlined);
		list->m_attributes.AttrSetStruckout(list_v2->attributes.struckout);
		list->m_attributes.AttrSetForeground(list_v2->attributes.foreground);
		list->m_attributes.AttrSetBackground(list_v2->attributes.background);

		AObjUnlockData(oiList, I_DATA_STYLE);
		AObjSetData(oiList, I_DATA_STYLE, list, sizeof(LIST));
		farfree(list);
		rVal = A_MODIFIED;
					
		}
	
	return rVal;
	}