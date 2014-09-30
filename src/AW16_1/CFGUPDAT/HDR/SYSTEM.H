#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "fileobj.h"
#include "ememstr.h"

#define MAXSTACKTOP	9

class SYSTEM
	{
private:
	static LPSERIUSMEMORY	theMemory;
	static LPFILEOBJECT		contextStack[MAXSTACKTOP+1];
	static int						stackTop;
	static int						numberOfColors;
	static LPSTRINGSPACE	tempStringSpace;

	static void InitMemory(
		void);


public:

	static void Initialize(
		void);

	static LPFILEOBJECT GetFileObject(
		void)
		{
		if (stackTop < 0 || stackTop > MAXSTACKTOP)
			return NULL;
		else
			return contextStack[stackTop];
		}

	static void SetFileObject(
		LPFILEOBJECT	newContext);

	static BOOL PushContext(
		LPFILEOBJECT	newContext);

	static LPFILEOBJECT PopContext(
		void);

	static void CleanUp(
		void);

	static LPSERIUSMEMORY GetTheMemory()
		{
		return theMemory;
		}

  static void SetTheMemory(
    LPSERIUSMEMORY  pTheMemory)
    {
    theMemory = pTheMemory;
    }

	static int GetNumColors(
		void)
		{
		return numberOfColors;
		}

	static BOOL IsRuntime(
		void)
		{
		return FALSE;
		}

	static BOOL IsExeStructures(
		void)
		{
		return FALSE;
		}

	static LPSTRINGSPACE GetTempStringSpace(
		void)
		{
		return tempStringSpace;
		}

  static void SetTempStringSpace(
    LPSTRINGSPACE  pStringSpace)
    {
    tempStringSpace = pStringSpace;
    }

	};

#endif