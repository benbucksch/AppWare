#ifndef UTILS_H
#define UTILS_H

void PostObjectChanged(AOBJECTID oi, USHORT usButton=I_UNDEFINED_INDEX, USHORT usOtherButton=I_UNDEFINED_INDEX);


int CalcNumberOfTableEntries(int nColorBits);

///void huge_memset(void huge *dest, char c, long len);
void huge_memset(void *dest, char c, long len);
///void huge_memmove(void huge *dest, void huge *src, long len);
void huge_memmove(void *dest, void *src, long len);
long CopyBitmapHandleToMemblock(AMEMBLOCKID mi, HBITMAP hBitmap, HPALETTE hPalette);
long CopyMemBlocks(AMEMBLOCKID miDest, AMEMBLOCKID miSource);
long CopyDatasToMemBlocks(AMEMBLOCKID miDest, AOBJECTID oiSource, LONG idxSource);
long CopyMemBlocksToDatas(AOBJECTID oiDest, LONG idxDest, AMEMBLOCKID miSource);
long CopyDatas(AOBJECTID oiDest, LONG idxDest, AOBJECTID oiSource, LONG idxSource);

BOOL CopyBitmapFromClipboardToMemblock(AMEMBLOCKID miBitmap, HWND hwnd);
BOOL LoadBitmapToMemblock(AMEMBLOCKID miBitmap);

HBITMAP CreateBitmapHandle(HPALETTE hPalette, AMEMBLOCKID miBitmap);
HBITMAP CreateBitmapHandle(HPALETTE hPalette, AOBJECTID oiData, long lIdxData);

HPALETTE CreateBitmapPalette(AMEMBLOCKID miBitmap);


void GetBitmapSize(AMEMBLOCKID miBitmap, LPPOINT pPtSize);


#endif
