///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"strclass.hpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	STRING class header for object management
//
////////////////////////////////////////////////////////////////////////////

struct Header {
  int		 count;
  int		 length;
  char 	 string;   	// just to get address
  };

////////////////////////////////////////////////////////////////////////////

class STRING {
	private:
	  long			data;           // which data pointer
	  AOBJECTID	objid;          // object id
	  void*			dataPtr;        // object data pointer

	  Header*		head;

	  long			length;
	  long			count;
	  char**		arrayPtr;  	// pointer to each string offset value

    char**		arrayWrk;
	  char*			vestp;

	  void 			Offsets(int);
	  void 			Resize();

	  void 			MakeWork();
	  void 			VestWork(char*);
	  void 			VestWork(int, int);
	  void			DeleteWork();

	public:
	  STRING(AOBJECTID, long);
	  ~STRING();

	  void 	Clear();
	  BOOL 	Insert(char*, WORD);
	  WORD 	Append(char*);
	  BOOL 	Delete(WORD);
		void 	CopyDataPtr(AOBJECTID, long );
	  long 	Count()				{return count;}

	  char* Get(WORD index) { // return pointer to string
	    if (index < count) {
	      return arrayPtr[index];
              }

	    return NULL;
	    }
	};

////////////////////////////////////////////////////////////////////////////
