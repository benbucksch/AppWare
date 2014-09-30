


WORD CalcNumberOfTableEntries(int nColorBits);

void _memset(void *dest, char c, long len);    
void _memmove(void *dest, void *src, long len);
long CopyBitmapHandleToMemblock(AMEMBLOCKID mi, HBITMAP hBitmap, HPALETTE hPalette);
long CopyMemBlocks(AMEMBLOCKID miDest, AMEMBLOCKID miSource);
long CopyDatasToMemBlocks(AMEMBLOCKID miDest, AOBJECTID oiSource, LONG idxSource);
long CopyMemBlocksToDatas(AOBJECTID oiDest, LONG idxDest, AMEMBLOCKID miSource);

HBITMAP CreateDefaultNormalBitmap();
void CreateDefaultNormalBitmap(AMEMBLOCKID miNormal);

HBITMAP CreateDefaultFocusedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultFocusedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal);
void CreateDefaultFocusedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal, AMEMBLOCKID miFocused);

HBITMAP CreateDefaultPressedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultPressedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal);
void CreateDefaultPressedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal, AMEMBLOCKID miPressed);

HBITMAP CreateDefaultGrayedBitmap(HPALETTE hPalette, HBITMAP hbmpNormal);
HBITMAP CreateDefaultGrayedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal);
void CreateDefaultGrayedBitmap(HPALETTE hPalette, AMEMBLOCKID miNormal, AMEMBLOCKID miGrayed);
	
