#include "windows.h"
#include "syntax.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SYNTAX
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SYNTAX::SYNTAX():syntaxArray()
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SYNTAX
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SYNTAX::SYNTAX(
	const SYNTAX far&	srcSyntax):
		syntaxArray(srcSyntax.syntaxArray.GetCount())
	{
	int i;
	unsigned short count;

	count = syntaxArray.GetCount();

	for (i=0; i<count; i++)
		syntaxArray[i] = srcSyntax.syntaxArray[i];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:  Updates an existing syntax string with a new value
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL SYNTAX::Update(
	LPSTR  newValue,
	int 	 index)
	{
	BOOL result = TRUE;

	if ((index < syntaxArray.GetCount()) && (index >= 0))
		syntaxArray[index] = newValue;
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Add
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:	Adds a new syntax string to the end of the array
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int SYNTAX::Add(
	LPSTR	syntaxString)
	{
	int newCount;

	newCount = syntaxArray.GetCount() + 1;

	syntaxArray.Resize(newCount);

	syntaxArray[newCount-1] = syntaxString;

	return newCount;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Delete
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:	Deletes an existing syntax string
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
int SYNTAX::Delete(
	int	index)
	{
	BOOL result = TRUE;

	if ((index < syntaxArray.GetCount()) && (index >= 0))
		syntaxArray.Delete(index);
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ConvertToPointers
//
// CLASS.......:  SYNTAX
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void SYNTAX::ConvertToPointers(
	void)
	{

	syntaxArray.ConvertToPointers();

	}