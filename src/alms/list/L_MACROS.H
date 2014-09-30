#ifndef L_MACROS_H
#define L_MACROS_H

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


#define LockListObject(oi) 						((LPLIST)AObjLockData(oi, I_DATA_STYLE))
#define UnlockListObject(oi) 					(AObjUnlockData(oi, I_DATA_STYLE))

#define LockListTable(oi)							((LPLIST_TABLE)AObjLockData(oi, I_DATA_TABLE))
#define UnlockListTable(oi)						(AObjUnlockData(oi, I_DATA_TABLE))
#define SetNumberTableItems(oi, num)	(AObjResizeData(oi, I_DATA_TABLE, num * sizeof(LIST_TABLE)))
#define SizeofListTable(oi)							(AObjGetDataSize(oi, I_DATA_TABLE))

#define LockListStrings(oi)						((LPSTR)AObjLockData(oi, I_DATA_STRINGS))
#define UnlockListStrings(oi)					(AObjUnlockData(oi, I_DATA_STRINGS))
#define ResizeListStrings(oi, s)	 		(AObjResizeData(oi, I_DATA_STRINGS, s))
#define SizeofListStrings(oi)	 				(AObjGetDataSize(oi, I_DATA_STRINGS))

#define LockListEdit(oi)							((LPSTR)AObjLockData(oi, I_DATA_EDIT))
#define UnlockListEdit(oi)						(AObjUnlockData(oi, I_DATA_EDIT))
#define ResizeListEdit(oi, s)	 				(AObjResizeData(oi, I_DATA_EDIT, s))
#define SizeofListEdit(oi)						(AObjGetDataSize(oi, I_DATA_EDIT))

inline LIST_VI_LISTBOX* GetListboxThisPointer(HWND hwnd) {return hwnd ? (LIST_VI_LISTBOX*)GetWindowLong(hwnd, g_iListboxThisPointerOffset) : NULL;}
inline void SetListboxThisPointer(HWND hwnd, LIST_VI_LISTBOX* wi) {SetWindowLong(hwnd, g_iListboxThisPointerOffset, (long)wi);}

inline LIST_VI_COMBOBOX* GetComboboxThisPointer(HWND hwnd) {return hwnd ? (LIST_VI_COMBOBOX*)GetWindowLong(hwnd, g_iComboboxThisPointerOffset) : NULL;}
inline void SetComboboxThisPointer(HWND hwnd, LIST_VI_COMBOBOX* wi) {SetWindowLong(hwnd, g_iComboboxThisPointerOffset, (long)wi);}

// My special templates ......
/*
template <class T> inline T min( T t1, T t2 ) { return t1>t2 ? t2 : t1;}
template <class T> inline T max( T t1, T t2 ) { return t1>t2 ? t1 : t2;}
template <class T> inline T range(T t0, T t1, T t2 ) {return max(t0, min(t1, t2) ); }
*/


#endif

