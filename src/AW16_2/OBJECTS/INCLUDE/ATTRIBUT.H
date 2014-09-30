#ifndef ATTRIBUT_H_
#define ATTRIBUT_H_

#include <commdlg.h>

#define ATTR_CHANGE_NOTHING					0x00000000L
#define ATTR_CHANGE_FACE						0x00000001L
#define ATTR_CHANGE_HEIGHT					0x00000002L
#define ATTR_CHANGE_WEIGHT					0x00000004L
#define ATTR_CHANGE_ITALIC					0x00000008L
#define ATTR_CHANGE_UNDERLINED			0x00000010L
#define ATTR_CHANGE_STRUCKOUT				0x00000020L

////////////////////////////

class ATTRIBUTES
	{
	private:
		HFONT									m_hfont;
		HBRUSH								m_hbrush;
		int										m_fontHeightInTMM;
		char									m_szFace[LF_FACESIZE];
		int										m_fontWeight;				//FW_BOLD, FW_EXTRABOLD....
		BOOL									m_bItalic;
		BOOL									m_bUnderlined;
		BOOL									m_bStruckout;
		COLORREF							m_clrForeground;
		COLORREF							m_clrBackground;

	private:
		void AttrLogfontToAttributes(LPLOGFONT logFont);
		void AttrAttributesToLogfont(LPLOGFONT logFont);

	public:
		ATTRIBUTES(BOOL createStuff = TRUE);
		ATTRIBUTES(ATTRIBUTES far *attSource);
		~ATTRIBUTES();
		void AttrInitialize();

		BOOL AttrCreateFont(HDC hdcTarget);	//Creates the font using the stored info for the target dc
		BOOL AttrDeleteFont();							//Deletes the font handle
		HFONT AttrGetFontHandle();					//Returns the font handle
		HFONT AttrSelectFont(HDC hdc);			//Selects the font handle into the dc
		BOOL AttrSetFont(HWND hWnd);					//Sets the font for the control

		BOOL AttrChangeFont(HDC hdcTarget, HWND hWndSelectedInto, long fChange, 
												LPSTR szFace=NULL, int iHeight=0, int iWeight=0, 
												BOOL bItalic=FALSE, BOOL bUnderlined=FALSE, BOOL bStruckout=FALSE);
		BOOL AttrChangeFontIndirect(HDC hdcTarget, HWND hWndSelectedInto, long fChange, ATTRIBUTES far *attr);

		int AttrGetFontHeight(int opCode);
		int AttrGetFontHeightAscent();
		int AttrGetFontCellHeight();
		int AttrGetFontFullHeight();

		BOOL AttrCreateBrush();
		BOOL AttrDeleteBrush();
		HBRUSH AttrGetBrushHandle();
		HBRUSH AttrSelectBrush(HDC hdc);
		BOOL AttrUpdateBrushDueToWinIniChanged();

		COLORREF AttrGetForeground();
		void AttrSetForeground(COLORREF f);

		COLORREF AttrGetBackground();
		void AttrSetBackground(COLORREF b);

		void AttrEditFont(HWND owner = NULL,
									LPSTR szSample="AaBbYyZz123",
									LPSTR szTitle = "Edit Attributes",
									BOOL doColors=TRUE,
									DWORD flags = CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT,
									BOOL doBackground=TRUE);


    // member functions to get the data
    inline void AttrGetFace(char* szFace, int nLen) {lstrcpyn(szFace, m_szFace, nLen); szFace[nLen-1] = '\0';}
    inline int  AttrGetHeight()                     {return m_fontHeightInTMM;}
    inline int  AttrGetWeight()                     {return m_fontWeight;}
    inline BOOL AttrIsItalic()                      {return m_bItalic;}
    inline BOOL AttrIsUnderlined()                  {return m_bUnderlined;}
    inline BOOL AttrIsStruckout()                   {return m_bStruckout;}
		//member functions to set the data
		inline void AttrSetFontFace(LPSTR szFace) {lstrcpyn(m_szFace, szFace, LF_FACESIZE); szFace[LF_FACESIZE-1] = '\0';}
		inline void AttrSetFontHeightTMM(int height)	{m_fontHeightInTMM = height;}
		inline void AttrSetFontWeight(int weight)			{m_fontWeight = weight;}
		inline void AttrSetItalic(BOOL b)							{m_bItalic = b;}
		inline void AttrSetUnderlined(BOOL b)					{m_bUnderlined = b;}
		inline void AttrSetStruckout(BOOL b) 					{m_bStruckout = b;}


		BOOL Export(void);
		BOOL Import(void);
	};
typedef ATTRIBUTES far * LPATTRIBUTES;


////////////////////////////////

#endif
