


int CalcNumberOfTableEntries(int nColorBits);

void huge_memset(void huge *dest, char c, long len);
void huge_memmove(void huge *dest, void huge *src, long len);
long CopyBitmapHandleToMemblock(MEMBLOCKID mi, HBITMAP hBitmap, HPALETTE hPalette);
long CopyMemBlocks(MEMBLOCKID miDest, MEMBLOCKID miSource);
long CopyDatasToMemBlocks(MEMBLOCKID miDest, OBJECTID oiSource, LONG idxSource);
long CopyMemBlocksToDatas(OBJECTID oiDest, LONG idxDest, MEMBLOCKID miSource);

HBITMAP CreateDefaultNormalBitmap();
void CreateDefaultNormalBitmap(MEMBLOCKID miNormal);

HBITMAP CreateDefaultFocusedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultFocusedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal);
void CreateDefaultFocusedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal, MEMBLOCKID miFocused);

HBITMAP CreateDefaultPressedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultPressedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal);
void CreateDefaultPressedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal, MEMBLOCKID miPressed);

HBITMAP CreateDefaultGrayedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultGrayedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal);
void CreateDefaultGrayedBitmap(HPALETTE hPalette, MEMBLOCKID miNormal, MEMBLOCKID miGrayed);
	
