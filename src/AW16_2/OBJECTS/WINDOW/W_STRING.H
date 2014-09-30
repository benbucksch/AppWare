
////////////////////////////////////////////////////////////////
// This file contains the defines for the string resources
////////////////////////////////////////////////////////////////

#ifndef W_STRING_H
#define W_STRING_H


#define ERR_NULL_PTR									2 		//"NULL pointer in %s line #%d"
#define ERR_WI_IDX_RANGE							5 		//"Window Item Index out of range in %s line #%d"

#define WARN_CAPTUREFAILED						6 		//"Failed to set the capture"

#define ERR_WRONG_TYPEID							7 		//"Incorrect Object Type in %s line #%d"

#define WARN_WINDOWNOTOPENED					8 		//"Window is not open "

#define ERR_HWND_WI_EXPECTED	 				9			//"Hwnd'd window item expected in %s line #%d"
#define ERR_LITE_WI_EXPECTED			 		10		//"Lite window item expected in %s line #%d"
#define ERR_UNKNOWN_WI_TYPE						11		//"Unknown type of window item in %s line #%d"

#define ERR_INVALIDWINDOW							12		//"Invalid window handle in %s line #%d"

#define INFO_METHOD_NOT_PROVIDED			13    //"A %s method was not provided"

#define WARN_DOESNT_SUPPORT_PWND			15		//"Item in the window doesn't support the window item protocol"

#define ERR_HOOK_FAILED								16		//"Installation of %s Hook failed"

#define ERR_GET_PARAMETER_FAILED			17	 //"Unable to get parameter in %s line #%d"
#define ERR_CREATING_PSUEDO_OBJECT  	18   //"Unable to create psuedo object in %s line #%d"

#define ERR_ITEM_NOT_IN_WINDOW				19   //"Invalide item.  Item not correct window in %s line #%d"

#define ERR_MEMBLOCK									20   //"Invalid MEMBLOCKID in %s line #%d"

#define ERR_INVALIDITEMNUM						21   //"Invalid item number of %d in %s line #%d"

#define ERR_INCORECT_MAPPING_MODE			22   //"Incorrect mapping mode set requested"

#define ERR_SHAPE_LIB_ADMIN						24   //"ShapeLibAdmin not found"

#define ERR_SHAPE_CREATEFN_MISMATCH		25   //"Shape Create Function mismatch in %s @ line #%l"
#define ERR_LONG_RANGE								26   //"Long number too big in %s"

#define ERR_SELFID_MISMATCH						27   //"(Window Object)->m_oiSelf != id"

#endif

