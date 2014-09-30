#ifndef __OPENFILE_H
#define __OPENFILE_H

#include "windows.h"

typedef BOOL (far *CHECKPROC)(int);


int far OpenFileDlgBox(
	HANDLE	hAppInstance,
	HANDLE	hWndParent,
	LPSTR		dirPathIn,
	LPSTR		qualifierIn,
	int			openOptions,
	LPSTR		dirPathOut,
	LPSTR		fileNameOut,
	CHECKPROC checkProc);

int far SaveAsDlgBox(
	HANDLE	hAppInstance,
	HANDLE	hWndParent,
	LPSTR		fileNameIn,
	LPSTR		directoryIn,
	LPSTR		qualifierIn,
	int			openOptions,
	LPSTR		saveType,
	LPSTR		directoryOut,
	LPSTR		fileNameOut);

#endif
