
typedef UINT far * LPUINT;

UINT ParameterStringLength(LPSTR szText);
BOOL ParameterPosAndCountToActual(LPSTR szText, LPUINT ptrPos, LPUINT ptrCount); //returns TRUE if within the string
void ActualPosAndCountToParameter(LPSTR szText, LPUINT ptrPos, LPUINT ptrCount);
BOOL IsCaseSensitive(LPSTR szText);

enum BOOL_POSTEDITED {DONTPOSTEDITED=FALSE, POSTEDITED=TRUE};
enum BOOL_POSTWITHDELAY {NODELAY=FALSE, DELAY=TRUE};
LONG PostObjectChanged(OBJECTID oiText, LONG lModifiers, BOOL_POSTEDITED postEdited, BOOL_POSTWITHDELAY postWithDelay);
	
