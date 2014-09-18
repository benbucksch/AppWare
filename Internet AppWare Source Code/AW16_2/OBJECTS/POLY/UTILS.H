#ifndef UTILS_H
#define UTILS_H

void PostObjectChanged(OBJECTID oi, USHORT usButton=I_UNDEFINED_INDEX, USHORT usOtherButton=I_UNDEFINED_INDEX);


int CalcNumberOfTableEntries(int nColorBits);

void huge_memset(void huge *dest, char c, long len);
void huge_memmove(void huge *dest, void huge *src, long len);
long CopyBitmapHandleToMemblock(MEMBLOCKID mi, HBITMAP hBitmap, HPALETTE hPalette);
long CopyMemBlocks(MEMBLOCKID miDest, MEMBLOCKID miSource);
long CopyDatasToMemBlocks(MEMBLOCKID miDest, OBJECTID oiSource, LONG idxSource);
long CopyMemBlocksToDatas(OBJECTID oiDest, LONG idxDest, MEMBLOCKID miSource);
long CopyDatas(OBJECTID oiDest, LONG idxDest, OBJECTID oiSource, LONG idxSource);

BOOL CopyBitmapFromClipboardToMemblock(MEMBLOCKID miBitmap, HWND hwnd);
BOOL LoadBitmapToMemblock(MEMBLOCKID miBitmap);

HBITMAP CreateBitmapHandle(HPALETTE hPalette, MEMBLOCKID miBitmap);
HBITMAP CreateBitmapHandle(HPALETTE hPalette, OBJECTID oiData, long lIdxData);

HPALETTE CreateBitmapPalette(MEMBLOCKID miBitmap);


void GetBitmapSize(MEMBLOCKID miBitmap, LPPOINT pPtSize);


#endif