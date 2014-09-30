

void ValidateNumberFormatString(LPSTR str, LPNFORM_INFO formatInfo, BOOL bDropDecimal);
unsigned short FormatFloatNumber(long double num, LPSTR format, LPSTR str, unsigned short len, LPNFORM_INFO formatInfo=NULL);
BOOL DoesStringContainRealNumberThingies(LPCSTR str);
long double ParseFormatedNumber(LPCSTR str, LPSTR format, LPNFORM_INFO formatInfo=NULL, LPCSTR endIdx=NULL);
long double GetDlgEditFormatedReal(HWND hWndDlg, int item, LPSTR format);
void SetDlgEditFormatedReal(HWND hWndDlg, int item, long double num, LPSTR format);
long GetDlgEditFormatedInteger(HWND hWndDlg, int item, LPSTR format);
void SetDlgEditFormatedInteger(HWND hWndDlg, int item, long num, LPSTR format);

// These are in n_math.asm
extern "C" BOOL   ONmbr_lAdd(long, long, LPLONG);
extern "C" BOOL   ONmbr_lSub(long, long, LPLONG);
extern "C" BOOL   ONmbr_lMul(long, long, LPLONG);
extern "C" BOOL   ONmbr_lInc(LPLONG);
extern "C" BOOL   ONmbr_lDec(LPLONG);





