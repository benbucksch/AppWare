/*************************************************************
 Group Dialog Box Defination
*************************************************************/
#define OBJECT_INDEX    0
#define OTYPE_GROUP			MAKETYPEID('G','r','u','p')


// Object Definition Dialog Box Constants
#define IDD_OBJECT      200
#define IDD_OBJECTSLIST 101
#define IDD_FIELDSLIST  102

#define editB           104
#define addB            105
#define removeB         106

#define textB           108
#define numberB         109
#define pictureB        110
#define dateB           111
#define listB           112
#define otherB          113

#define IDHELP					998

#define OBJECTNAMESIZE  32

extern HINSTANCE hInstance ;

extern "C"
{
LONG ALMCALLBACK GroupAdministration(OBJECTID, pAObjMessage) ;
BOOL ALMCALLBACK GroupObjectProc(HWND, WORD, WORD, LONG) ;
void ALMCALLBACK CountFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void ALMCALLBACK FindFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void ALMCALLBACK GetFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void ALMCALLBACK InsertFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void ALMCALLBACK RemoveFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;
}

void InitDialog (HWND, OBJECTID) ;
void AddItem (HWND) ;
void RemoveItem(HWND) ;
void EditItem (HWND) ;
void saveObj(HWND, OBJECTID) ;
void doValidateObjectIDs (OBJECTID, pAObjMessage) ;
void DisplayObj(HWND, TYPEID) ;
void UpdateObjectList(HWND, WORD) ;
void CheckObjID (OBJECTID) ;


