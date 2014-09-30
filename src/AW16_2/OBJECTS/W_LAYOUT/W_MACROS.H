///////////////////////////////////////////////
/// This file contains macros and inline functions
/// for the window object
///////////////////////////////////////////////

#ifndef W_MACROS_H
#define W_MACROS_H

#ifdef ADBG_DEBUG
#define I_VERIFY_POINTER(ptr, exp) 					 if (!ptr){ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_PTR, __FILE__, __LINE__); exp;}
#define I_VERIFY_TYPEID(oi, ti,  exp)        if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, __FILE__, __LINE__); exp;}
#define I_VERIFY_WI_HWND(wi, exp)            if (!wi || !WINDOWAPI::IsHwndItem(wi)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_HWND_WI_EXPECTED, __FILE__, __LINE__); exp;}
#define I_VERIFY_WI_LITE(wi, exp)            if (!wi || !WINDOWAPI::IsLiteItem(wi)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_LITE_WI_EXPECTED, __FILE__, __LINE__); exp;}
#define I_VERIFY_WINDOW(hwnd, exp)           if (!IsWindow(hwnd)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDWINDOW, __FILE__, __LINE__); exp;}
#define I_VERIFY_PARAMETER(id, idx, exp)     if (!id) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, idx, __FILE__, __LINE__);exp;}
#define I_VERIFY_PSUEDO(id, exp)             if (!id) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, __FILE__, __LINE__);exp;}
#define I_VERIFY_MI(mi, exp)                 if (!mi) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_MEMBLOCK, __FILE__, __LINE__);exp;}

#define I_ASSERT_ITEM_NUMBER(wnd, num, exp)  if (num >= wnd->GetNumberOfItems() || num<0) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_INVALIDITEMNUM, num, __FILE__, __LINE__);exp;}
#define I_ASSERT_LONG_RANGE(l, exp)          if (l>32768L) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_LONG_RANGE, __FILE__, __LINE__); exp;}
#define I_ASSERT_WI_IN_WND(wi, wnd, exp)     if (wnd->m_oiSelf != wi->m_oiParent) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_ITEM_NOT_IN_WINDOW, __FILE__, __LINE__); exp;}
#define I_ASSERT_HOOK(hook, hookName)        if (!hook) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_HOOK_FAILED, hookName);}

#define I_PROTOCOL_NOT_SUPPORTED(type)       ADbgPrintRes(ADBG_WARNING, type, g_hInstance, WARN_DOESNT_SUPPORT_PWND);

#else
#define I_VERIFY_POINTER(ptr, exp) 					 if (!ptr){exp;}
#define I_VERIFY_TYPEID(oi, ti, exp)         if(!AObjCheckType(oi, ti))                  {exp;}
#define I_VERIFY_WI_HWND(wi, exp)            if(!wi || !WINDOWAPI::IsHwndItem(wi))       {exp;}
#define I_VERIFY_WI_LITE(wi, exp)            if(!wi || !WINDOWAPI::IsLiteItem(wi))       {exp;}
#define I_VERIFY_WINDOW(hwnd, exp)           if(!IsWindow(hwnd))                         {exp;}
#define I_VERIFY_PARAMETER(id, idx, exp)     if(!id)                                     {exp;}
#define I_VERIFY_PSUEDO(id, exp)             if(!id)                                     {exp;}
#define I_VERIFY_MI(mi, exp)                 if(!mi)                                     {exp;}

#define I_ASSERT_ITEM_NUMBER(wnd, num, exp)  
#define I_ASSERT_LONG_RANGE(l, exp)          
#define I_ASSERT_WI_IN_WND(wi, wnd,  exp)             
#define I_ASSERT_HOOK(hook, hookName)

#define I_PROTOCOL_NOT_SUPPORTED(type)

#endif


#define GetWindowRuntime()                  ((WINDOWRUNTIME*)ATypeGetRuntimeInfo(OTYPE_WINDOW))

#define LockItemData(id, itemNum)           ((WNDITEM_DATA*)AObjLockData(id, itemNum+I_DATA_ITEMS_OFFSET))
#define UnlockItemData(id, itemNum)         AObjUnlockData(id, itemNum+I_DATA_ITEMS_OFFSET)
#define GetItemDataSize(id, itemNum)        AObjGetDataSize(id, itemNum+I_DATA_ITEMS_OFFSET)
#define SetItemDataSize(id, itemNum, size)  AObjResizeData(id, itemNum+I_DATA_ITEMS_OFFSET, size)


#ifdef THE_OLD_INLINE_FUNCTIONS
inline WINDOW* LockWindowData(OBJECTID id) {return (WINDOW*)AObjLockData(id, I_DATA_WINDOW); }
inline BOOL UnlockWindowData(OBJECTID id) {return AObjUnlockData(id, I_DATA_WINDOW); }

inline WINDOWRUNTIME* GetWindowRuntime() {return (WINDOWRUNTIME*) ATypeGetRuntimeInfo(OTYPE_WINDOW);}

inline WNDITEM_DATA* LockItemData(OBJECTID id, int itemNum) {return (WNDITEM_DATA*)AObjLockData(id, itemNum+I_DATA_ITEMS_OFFSET);}
inline BOOL UnlockItemData(OBJECTID id, int itemNum) {return AObjUnlockData(id, itemNum+I_DATA_ITEMS_OFFSET);}
inline LONG GetItemDataSize(OBJECTID id, int itemNum) {return AObjGetDataSize(id, itemNum+I_DATA_ITEMS_OFFSET);}
inline BOOL SetItemDataSize(OBJECTID id, int itemNum, LONG size) {return AObjResizeData(id, itemNum+I_DATA_ITEMS_OFFSET, size);}
#endif


// My special templates ......
template <class T> inline T min( T t1, T t2 )         { return t1>t2 ? t2 : t1;}
template <class T> inline T max( T t1, T t2 )         { return t1>t2 ? t1 : t2;}
template <class T> inline T range(T t0, T t1, T t2 )  { return max(t0, min(t1, t2));}

inline BOOL IsShiftKeyDown() { return (GetKeyState(VK_SHIFT)   & 0x8000);}  //Test the hi-order bit
inline BOOL IsAltKeyDown()   { return (GetKeyState(VK_MENU)    & 0x8000);}  //Test the hi-order bit
inline BOOL IsCtrlKeyDown()  { return (GetKeyState(VK_CONTROL) & 0x8000);}  //Test the hi-order bit


inline BOOL StoreIDinExtraBytes(HWND hwnd, OBJECTID oi) {
  I_VERIFY_WINDOW(hwnd, return FALSE);
  SetWindowLong(hwnd, I_EXTRABYTES_ID_OFFSET, (long)oi);
  return TRUE;
  }

inline OBJECTID GetIDfromExtraBytes(HWND hwnd) {
  I_VERIFY_WINDOW(hwnd, return 0);
  return (OBJECTID)GetWindowLong(hwnd, I_EXTRABYTES_ID_OFFSET);
  }



#endif

