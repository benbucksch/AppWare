template <class T> inline T MIN( T t1, T t2 ) { return t1>t2 ? t2 : t1;}
template <class T> inline T MAX( T t1, T t2 ) { return t1>t2 ? t1 : t2;}

#define APB_NOINFO					0x0000
#define APB_UPSFOPEN				0x0001		
#define APB_UPSFFATALERROR	0x0002

//Animates the src rectangle to the dst rectangle.
void AnimateRectToRect(
	HDC hdc,
	LPRECT src,
	LPRECT dst,
	HPEN pen,
	double steps,
	DWORD delay);


void apb_ShowWaitCursor();

void apb_RestoreOldCursor();

void apb_FlashRectangle(
	HDC hdc,
	LPRECT rc);

void apb_DoFrameDraw(
	HDC hdc,
	LPRECT bounds,
	HPEN frame,
	HPEN highlight,
	HPEN shadow);

void apb_DoThinFrameDraw(
	HDC hdc,
	LPRECT bounds,
	HPEN highlight,
	HPEN shadow);

long apb_LogUpsfError(
	long upsfError,
	LPSTR comment,
	WORD info = APB_NOINFO);

BOOL apb_BuildCheckErrorMessage(
	short code,
	LPSTR buffer,
	int bufLength);

int	apb_ExtractDllLibs(
	char* DLLNamePtr,
	AMEMBLOCKID	bufferID);

BOOL apb_ReplaceIconResource(
	LPVOID fileBase,
	LPCSTR resName,
	LPVOID rawData,
	int dataSize);
