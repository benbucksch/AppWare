#ifndef TM_MACROS_H
#define TM_MACROS_H

#ifdef ADBG_DEBUG
#define I_ASSERT_TIME(tm, funName, exp)            if (!tm)                    {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_TIME, funName); exp;}
#define I_ASSERT_HTI(hti, funName, exp)            if (!hti)                   {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_HTI, funName); exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp)        if (!IsWindow(hwnd))        {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, funName); exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp)      if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, funName); exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)  if (!id)                    {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, idx, funName); exp;}
#define I_ASSERT_PSUEDO(id, funName, exp)          if (!id)                    {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, funName); exp;}
#else
#define I_ASSERT_TIME(tm, funName, exp)            if (!tm)                    {exp;}
#define I_ASSERT_HTI(hti, funName, exp)            if (!hti)                   {exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp)        if (!IsWindow(hwnd))        {exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp)      if (!AObjCheckType(oi, ti)) {exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)  if (!id)                    {exp;}
#define I_ASSERT_PSUEDO(id, funName, exp)          if (!id)                    {exp;}
#endif

#define LockTimeObject(oi) ((LPTime)AObjLockData(oi, ZeroBase))
#define UnlockTimeObject(oi) (AObjUnlockData(oi, ZeroBase))

inline HwndTimeItem* GetThisPointer(HWND hwnd) {return (HwndTimeItem*)GetWindowLong(hwnd, g_iThisPointerOffset);}
inline void SetThisPointer(HWND hwnd, HwndTimeItem* This) {SetWindowLong(hwnd, g_iThisPointerOffset, (long)This);}

#endif

