#ifndef N_MACROS_H
#define N_MACROS_H

#ifdef ADBG_DEBUG
#define I_ASSERT_NUM(num, funName, exp) if (!num){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_NUM, funName); exp;}
#define I_ASSERT_HNI(hni, funName, exp) if (!hni){	ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_HNI, funName); exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp) if (!IsWindow(hwnd)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, funName); exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp) if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, funName); exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)  if (!id) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, idx, funName);exp;}
#define I_ASSERT_PSUEDO(id, funName, exp) if (!id) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, funName);exp;}
#else
#define I_ASSERT_NUM(num, funName, exp)
#define I_ASSERT_HNI(hni, funName, exp)
#define I_ASSERT_WINDOW(hwnd, funName, exp)
#define I_ASSERT_TYPEID(oi, ti, funName, exp)
#define I_ASSERT_PARAMETER(id, idx, funName, exp)
#define I_ASSERT_PSUEDO(id, funName, exp) if (!id)
#endif

#define LockNumberObject(oi) ((LPNUMBER)AObjLockData(oi, I_DATA_STYLE))
#define UnlockNumberObject(oi) (AObjUnlockData(oi, I_DATA_STYLE))

inline HwndNumberItem* GetThisPointer(HWND hwnd) {return (HwndNumberItem*)GetWindowLong(hwnd, g_iThisPointerOffset);}
inline void SetThisPointer(HWND hwnd, HwndNumberItem* This) {SetWindowLong(hwnd, g_iThisPointerOffset, (long)This);}

#endif

