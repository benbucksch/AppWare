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

///////////////////////////////////////////////////////////////
// strclass.h
// Robert Lundahl/ Edgar C. Jerez
//
// String Object By Index
// Warning: indexes are fixed, string delete will change all
//					subsequent indexes.
//   Append String
//	 Insert String
//   Delete String
//   Get String
//   Count
//
// Start Date 07/01/92
// End   Date 07/02/92
///////////////////////////////////////////////////////////////

//#include "a_alm.h"
//#include "a_almutl.h"

///////////////////////////////////////////////////////////////
// struct Header: Storage Format
//
// count 				total number of strings   WORD
// length                                 WORD
// string ...   sequential strings
//
///////////////////////////////////////////////////////////////

struct Header
{
	UINT	 count;
	UINT	 length;
	BYTE 	 string;					   	// just to get address
};


///////////////////////////////////////////////////////////////
// STRING class.
//
///////////////////////////////////////////////////////////////

class STRING
{
	private:
		LONG			data;           // which data pointer
		OBJECTID	objid;          // object id
		void*			dataPtr;        // object data pointer
		Header*		head;
		UINT			length;
		UINT			count;
		BYTE**		arrayPtr;  			// pointer to each string offset value
		BYTE**		arrayWrk;
		BYTE*			vestp;

		void Offsets ( WORD cnt );
		void Resize ( void );
		void MakeWork ( void );
		void VestWork ( BYTE* lpstr );
		void VestWork ( WORD start, WORD end);
		void DeleteWork ( void );

	public:
		STRING ( OBJECTID objid, LONG thedata );
		~STRING();
		void Clear ( void );
		WORD Append ( BYTE* lpstr );
		BOOL Delete ( WORD index );
		LONG Count( void ) {return count;}
		void CopyDataPtr( OBJECTID, long);
		BYTE* Get(UINT index) 	// return pointer to string
		{
			if (index < count)
				return arrayPtr[index];
			return NULL;
		}
		void SetData( OBJECTID, long, void*, long);

};
