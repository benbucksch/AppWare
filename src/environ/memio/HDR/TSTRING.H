#ifndef __TSTRING_H
#define __TSTRING_H

class TSTRING
	{
	friend union DBSTRING;
	friend union EXESTRING;

private:

	short	blockIndex;   //index of string block
	WORD	offset;       //offset in string block
               
public:

	TSTRING(void)
		{
		blockIndex = -1;
    offset     =  0;
		};

	TSTRING(
		const TSTRING & srcString);

	TSTRING(
		const char *		srcString);

	TSTRING(
		DBSTRING	srcString);

	TSTRING(
		HANDLE	hFile);

	~TSTRING(void);

	TSTRING & operator =(
		const TSTRING &		srcString);

	int strlen(void);

  BOOL operator ==(
    TSTRING  & str2);

  BOOL operator !=(
    TSTRING  & str2);

	operator const char *();

	};

#endif
