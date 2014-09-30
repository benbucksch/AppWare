#ifndef DT_MACRO_H
#define DT_MACRO_H

#ifdef ADBG_DEBUG
#define I_ASSERT_DATE(date, funName, exp)           if (!date)                  {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_DATE, funName); exp;}
#define I_ASSERT_HDI(hdi, funName, exp)             if (!hdi)                   {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_HDI, funName); exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp)         if (!IsWindow(hwnd))        {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, funName); exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp)       if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, funName); exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)   if (!id)                    {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, idx, funName); exp;}
#define I_ASSERT_PSUEDO(id, funName, exp)           if (!id)                    {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, funName); exp;}
#else
#define I_ASSERT_DATE(date, funName, exp)           if (!date)                  {exp;}
#define I_ASSERT_HDI(hdi, funName, exp)             if (!hdi)                   {exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp)         if (!IsWindow(hwnd))        {exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp)       if (!AObjCheckType(oi, ti)) {exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)   if (!id)                    {exp;}
#define I_ASSERT_PSUEDO(id, funName, exp)           if (!id)                    {exp;}
#endif

#define LockDateObject(oi) ((LPDate)AObjLockData(oi, ZeroBase))
#define UnlockDateObject(oi) (AObjUnlockData(oi, ZeroBase))

inline HwndDateItem* GetThisPointer(HWND hwnd) {return (HwndDateItem*)GetWindowLong(hwnd, g_iThisPointerOffset);}
inline void SetThisPointer(HWND hwnd, HwndDateItem* This) {SetWindowLong(hwnd, g_iThisPointerOffset, (long)This);}

#endif

