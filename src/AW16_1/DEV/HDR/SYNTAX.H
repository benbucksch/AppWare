#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include "dbsarray.h"
#include "tstring.h"

class SYNTAX
	{
private:
	DBSTRINGARRAY	syntaxArray;

public:

	SYNTAX();

	SYNTAX(
		const SYNTAX far&	srcSyntax);

	SYNTAX(
		void far*	objPtr):
			syntaxArray(objPtr)
		{
		}


	BOOL Update(
		LPSTR	  newValue,
		int 		index);

	int Add(
		LPSTR	syntaxString);

	int Delete(
		int	index);

	void Resize(
		unsigned short newSize)
		{
		syntaxArray.Resize(newSize);
		}

	TSTRING GetSyntax(
		unsigned long	index)
		{
		return syntaxArray[index];
		}

	unsigned int GetCount(
		void)
		{
		return syntaxArray.GetCount();
		}

	void ConvertToPointers(
		void);

	};

#endif
