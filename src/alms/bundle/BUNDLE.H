/*************************************************************
 Bundle Dialog Box Defination
*************************************************************/
#define OBJECT_INDEX    0
#define Bundle          300

// Object Definition Dialog Box Constants
#define AVAILABLE_OBJ   101
#define SELECTED_OBJ    102

#define editB           104
#define addB            105
#define removeB         106

#define textB           108
#define numberB         109
#define pictureB        110
#define listB           111
#define otherB          112
#define IDD_OBJECT      200
#define IDHELP					998

#define OBJECTNAMESIZE  32
#define OTYPE_BUNDLE		ATYPE_MAKEID('B','n','d','l')

#define BUNDLEDATAS     1

extern HINSTANCE hInstance ;
extern "C"
{
void ALMCALLBACK BuildFunc(pAEvtInfo, pAObjMessage) ;
LONG ALMCALLBACK BundleAdministration(AOBJECTID, pAObjMessage) ;
}

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/ BundleObjectProc (HWND, WORD, WORD, LONG) ;
void InitDialog (HWND, AOBJECTID) ;
void AddItem (HWND) ;
void RemoveItem(HWND) ;
void EditItem (HWND) ;
void saveObj(HWND, AOBJECTID) ;
void doValidateObjectIDs (AOBJECTID, pAObjMessage) ;
void DisplayObj(HWND, ATYPEID) ;
void UpdateObjectList(HWND, WORD) ;
void CheckObjID (AOBJECTID) ;

