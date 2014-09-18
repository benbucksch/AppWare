#ifndef T_MACROS_H
#define T_MACROS_H

#ifdef ADBG_DEBUG
#define I_ASSERT_TXT(txt, funName, exp) if (!txt){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_TXT, funName); exp;}
#define I_ASSERT_TR(tr, funName, exp) if (!tr){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_TR, funName); exp;}
#define I_ASSERT_HTI(hti, funName, exp) if (!hti){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_HTI, funName); exp;}
#define I_ASSERT_STRING(string, funName, exp) if (!string){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_STRING, funName); exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp) if (!IsWindow(hwnd)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, funName); exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp) if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, funName); exp;}

#define I_VERIFY_PARAMETER(id, idx, funName, exp)  if (!id) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, idx, funName);exp;}
#define I_VERIFY_PSUEDO(id, funName, exp) if (!id) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, funName);exp;}

#ifdef I_ASSERT
#undef I_ASSERT
#define I_ASSERT(exp) if (!(exp)) {ADbgPrint(ADBG_ERROR, 0, "%s", #exp);}
#endif //the redefinition of I_ASSERT

#else //ADBG_DEBUG

#define I_ASSERT_TXT(txt, funName, exp)
#define I_ASSERT_TR(tr, funName, exp)
#define I_ASSERT_HTI(hti, funName, exp)
#define I_ASSERT_STRING(string, funName, exp)
#define I_ASSERT_WINDOW(hwnd, funName, exp)
#define I_ASSERT_TYPEID(oi, ti, funName, exp)		

#define	I_VERIFY_PARAMETER(id, idx, funName, exp)	if (!id) {exp;}
#define	I_VERIFY_PSUEDO(id,funName,exp)	if (!id) {exp;}
#endif

#define I_VERIFY_TYPE_FOR_API(oi, funName, exp) if (!AObjCheckType(oi, OTYPE_TEXT))	{ADBG_EVAL(ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, funName)); exp;}
#define I_VERIFY_FOR_API(bool, funName, exp) if (!(bool))	{ADBG_EVAL(ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_BAD_API_PARAMETER, funName)); exp;}


#define LockTextStyle(oi) ((TEXT*)AObjLockData(oi, I_DATA_STYLE))
#define UnlockTextStyle(oi) (AObjUnlockData(oi, I_DATA_STYLE))

#define LockTextString(oi) ((LPSTR)AObjLockData(oi, I_DATA_STRING))
#define UnlockTextString(oi) (AObjUnlockData(oi, I_DATA_STRING))
#define ResizeTextString(oi, len) (AObjResizeData(oi, I_DATA_STRING, len))
#define GetTextStringSize(oi) ((int)AObjGetDataSize(oi, I_DATA_STRING))

inline TEXTRUNTIME* GetTextRuntime() {return (TEXTRUNTIME*) ATypeGetRuntimeInfo(OTYPE_TEXT);}

inline HwndTextItem* GetThisPointer(HWND hwnd) {return (HwndTextItem*)GetWindowLong(hwnd, g_iThisPointerOffset);}
inline void SetThisPointer(HWND hwnd, HwndTextItem* hti) {SetWindowLong(hwnd, g_iThisPointerOffset, (LONG)hti);}

// My special templates ......
#if !defined( __MINMAX_DEFINED) && defined(__cplusplus)
#define __MINMAX_DEFINED
template <class T> inline const T _FAR &min( const T _FAR &t1, const T _FAR &t2 )
{
    if  (t1 < t2)
        return t1;
    else
        return t2;
}

template <class T> inline const T _FAR &max( const T _FAR &t1, const T _FAR &t2 )
{
    if  (t1 > t2)
        return t1;
    else
        return t2;
}
#endif

template <class T> inline const T _FAR &range(const T _FAR &t0, const T _FAR &t1, const T _FAR &t2 )
{
    return max(t0, min(t1, t2) );
}



#endif

