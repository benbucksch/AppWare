#ifndef MACROS_H
#define MACROS_H

#ifdef I_VERIFY
#undef I_VERIFY
#endif

#ifdef ADBG_DEBUG
#define I_VERIFY(test, exp) 					 			 if (!(test)){ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_ASSERTION_FAILED, __FILE__, __LINE__); exp;}
#define I_VERIFY_POINTER(ptr, exp) 					 if (!ptr){ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_NULL_PTR, __FILE__, __LINE__); exp;}
#define I_VERIFY_TYPEID(oi, ti,  exp)        if (!AObjCheckType(oi, ti)) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_WRONG_TYPEID, __FILE__, __LINE__); exp;}
#define I_VERIFY_PARAMETER(id, exp)  if (!id) { ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_GET_PARAMETER_FAILED, __FILE__, __LINE__);exp;}
#define I_VERIFY_PSUEDO(id, exp)             if (!id) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_CREATING_PSUEDO_OBJECT, __FILE__, __LINE__);exp;}
#define I_VERIFY_MI(mi, exp)                 if (!mi) {ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_MEMBLOCK, __FILE__, __LINE__);exp;}
#define I_FAILURE()													 {ADbgPrint(ADBG_ERROR, 0, "Failure @ %s %d",  __FILE__, __LINE__);}

#ifdef I_ASSERT
#undef I_ASSERT
#define I_ASSERT(exp) if (!(exp)) {ADbgPrint(ADBG_ERROR, 0, "%s", #exp);}
#endif //the redefinition of I_ASSERT

#else
#define I_VERIFY(test, exp) 					 			 if (!(test)){exp;}
#define I_VERIFY_POINTER(ptr, exp) 					 if (!ptr){exp;}
#define I_VERIFY_TYPEID(oi, ti, exp)         if(!AObjCheckType(oi, ti))                  {exp;}
#define	I_VERIFY_PARAMETER(id, exp) if (!id){exp;}
#define I_VERIFY_PSUEDO(id, exp)             if(!id)                                     {exp;}
#define I_VERIFY_MI(mi, exp)                 if(!mi)                                     {exp;}
#define I_FAILURE()	
#endif


#define GetPolyDataSize(oiPoly) (AObjGetDataSize(oiPoly, I_DATA_POLY))
#define SetPolyDataSize(oiPoly, size) (AObjResizeData(oiPoly, I_DATA_POLY, size))

#define LockPoly(oiPoly) (LPPOLY)(AObjLockData(oiPoly, I_DATA_POLY))
#define UnlockPoly(oiPoly) (AObjUnlockData(oiPoly, I_DATA_POLY))

// My special templates ......
template <class T> inline T min( T t1, T t2 ) { return t1>t2 ? t2 : t1;}
template <class T> inline T max( T t1, T t2 ) { return t1>t2 ? t1 : t2;}
template <class T> inline T range(T t0, T t1, T t2 ) {return max(t0, min(t1, t2) ); }


#endif //MACROS_H