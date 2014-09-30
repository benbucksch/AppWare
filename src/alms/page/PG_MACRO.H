#ifndef PG_MACRO_H
#define PG_MACRO_H

#ifdef ADBG_DEBUG
#define I_ASSERT_PAGE(page, funName, exp) if (!page){	ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_NULL_PAGE, funName); exp;}
#define I_ASSERT_HDI(hdi, funName, exp) if (!hdi){	ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_NULL_HDI, funName); exp;}
#define I_ASSERT_WINDOW(hwnd, funName, exp) if (!IsWindow(hwnd)) {ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_INVALIDWINDOW, funName); exp;}
#define I_ASSERT_TYPEID(oi, ti, funName, exp) if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_WRONG_TYPEID, funName); exp;}
#define I_ASSERT_PARAMETER(id, idx, funName, exp)  if (!id) { ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_GET_PARAMETER_FAILED, idx, funName);exp;}
#define I_ASSERT_PSUEDO(id, funName, exp) if (!id) {ADbgPrintRes(ADBG_ERROR, 0, vG.hInstance, ERR_CREATING_PSUEDO_OBJECT, funName);exp;}
#else
#define I_ASSERT_PAGE(page, funName, exp)
#define I_ASSERT_HDI(hdi, funName, exp)
#define I_ASSERT_WINDOW(hwnd, funName, exp)
#define I_ASSERT_TYPEID(oi, ti, funName, exp)
#define I_ASSERT_PARAMETER(id, idx, funName, exp)
#define I_ASSERT_PSUEDO(id, funName, exp) 
#endif

#define LockPageData0(oi) ((LPDATA0)AObjLockData(oi, 0L))
#define LockPageData1(oi) ((LPDATA1)AObjLockData(oi, 1L))
#define UnlockPageData0(oi) (AObjUnlockData(oi, 0L))
#define UnlockPageData1(oi) (AObjUnlockData(oi, 1L))

#endif
