#ifndef B_MACROS_H
#define B_MACROS_H

#ifdef ADBG_DEBUG
#define I_ASSERT_POINTER(ptr, exp) if (!ptr){ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_PTR, __FILE__, __LINE__); exp;}
#define I_ASSERT_WINDOW(hwnd, exp) if (!IsWindow(hwnd)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, __FILE__, __LINE__); exp;}

#define I_VERIFY_POINTER(ptr, exp) if (!ptr){ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_PTR, __FILE__, __LINE__); exp;}
#define I_VERIFY_TYPEID(oi, ti, exp) if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, __FILE__, __LINE__); exp;}
#define I_VERIFY_PARAMETER(id, exp)  if (!id) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, __FILE__, __LINE__);exp;}
#define I_VERIFY_PSUEDO(id, exp) if (!id) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, __FILE__, __LINE__);exp;}

#else	 /*!ADBG_DEBUG*/
#define I_ASSERT_POINTER(ptr, exp)
#define I_ASSERT_WINDOW(hwnd, exp)
#define I_ASSERT_TYPEID(oi, ti, exp)

#define I_VERIFY_POINTER(ptr, exp) if (!ptr){exp;}
#define I_VERIFY_TYPEID(oi, ti, exp) if (!AObjCheckType(oi, ti)) {exp;}
#define	I_VERIFY_PARAMETER(id, exp) if (!id){exp;}
#define	I_VERIFY_PSUEDO(id, exp) if (!id){exp;}
#endif /*!ADBG_DEBUG*/



#define LockButtonObject(oi) ((BUTTON*)AObjLockData(oi, I_DATA_STYLE))
#define UnlockButtonObject(oi) (AObjUnlockData(oi, I_DATA_STYLE))

#define LockNormalBitmap(oi)		((BMP_DATA*)AObjLockData(oi, I_DATA_BMP_NORMAL))
#define UnlockNormalBitmap(oi)	(AObjUnlockData(oi, I_DATA_BMP_NORMAL))

#define LockFocusedBitmap(oi)		((BMP_DATA*)AObjLockData(oi, I_DATA_BMP_FOCUSED))
#define UnlockFocusedBitmap(oi)	(AObjUnlockData(oi, I_DATA_BMP_FOCUSED))

#define LockPressedBitmap(oi)		((BMP_DATA*)AObjLockData(oi, I_DATA_BMP_PRESSED))
#define UnlockPressedBitmap(oi)	(AObjUnlockData(oi, I_DATA_BMP_PRESSED))

#define LockGrayedBitmap(oi)		((BMP_DATA*)AObjLockData(oi, I_DATA_BMP_GRAYED))
#define UnlockGrayedBitmap(oi)	(AObjUnlockData(oi, I_DATA_BMP_GRAYED))

inline HWND_VI* GetThisPointer(HWND hwnd) {return (HWND_VI*)GetWindowLong(hwnd, g_iThisPointerOffset);}
inline void SetThisPointer(HWND hwnd, HWND_VI* wi) {SetWindowLong(hwnd, g_iThisPointerOffset, (long)wi);}

// My special templates ......
template <class T> inline T min( T t1, T t2 ) { return t1>t2 ? t2 : t1;}
template <class T> inline T max( T t1, T t2 ) { return t1>t2 ? t1 : t2;}
template <class T> inline T range(T t0, T t1, T t2 ) {return max(t0, min(t1, t2) ); }



#endif

