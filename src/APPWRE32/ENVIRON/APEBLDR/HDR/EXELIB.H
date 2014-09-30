// exelib.h -- runtime DLL descriptor

#ifndef __EXELIB_H
#define __EXELIB_H

#include "exeprocl.h"
#include "sereclib.h"

class EXELIB
	{

friend class EXELIBLIST;

private:

	EXESTRING  name;
	SERUID     procListUID;
	HINSTANCE  hLib;

public:

	EXELIB()
		: name()
		{
		}

	// A dummy delete to override default ..
	// .. since we never 'new' EXELIBs
	void operator delete(
		void *  /*theExeLib*/,
		size_t     /*theSize*/)
		{
		}

	~EXELIB()
		{
		delete this->GetProcList();
		}


 ///////////////////////////////////////////////////////////////////////
 //   EXELIB::Init
 //
 // Initialize our memory space.  Do not use if the 'name' data member
 // has already been init'd, since we won't be able to reclaim
 // EXESTRING space.
 ///////////////////////////////////////////////////////////////////////
  void Init(
    LPCSTR theLibName)
    {
    // make sure the name has a nil EXESTRING value
    new(&name) EXESTRING;
    name = theLibName;
    procListUID = GetUID(new EXEPROCLIST);
    hLib = 0;
    }

  void SetHLib(
    HINSTANCE theHLib)
    {
    hLib = theHLib;
    }

  LPCSTR GetName()
    {
    return name;
    }

  LPEXEPROCLIST GetProcList()
    {
    return (LPEXEPROCLIST) GetObject(procListUID);
		}

  HINSTANCE GetHLib()
    {
    return hLib;
    }

  };

#endif  // __EXELIB_H
