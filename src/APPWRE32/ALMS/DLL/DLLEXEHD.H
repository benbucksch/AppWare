
////////////////////////////////////////////////////////
// The followind is the definitions and structures
// needed to access the old and new exe file heades.
////////////////////////////////////////////////////////

#define OLDEXESIGNATURE		0x5A4D				// ZM
#define NEWEXESIGNATURE		0x454E				// EN

#define	IDERR_SUCCESS						0
#define	IDERR_OPENFAIL					1
#define	IDERR_READFAIL					2
#define	IDERR_FILETYPEBAD				3
#define	IDERR_EXETYPEBAD				4
#define	IDERR_WINVERSIONBAD			5
#define	IDERR_RESNAMETBLBAD			6
#define	IDERR_NONRESNAMETBLBAD	7

#define	WINDOWSEXE							2

#define	SEEK_SET	0
#define	SEEK_END	2
#define	OF_ERROR	-1


// exe, dll file headers...
typedef struct {						// dos 1, 2, 3, 4 .exe header.
	USHORT	ehSignature;			// signature bytes 
	USHORT	ehcbLP;						// bytes on last page of file.
	USHORT	ehcp;							// pages in file
	USHORT	ehcRelocation;		// count of relocation table entries
	USHORT	ehcParagraphHdr;	// size of headers in paragraphs
	USHORT	ehMinAlloc;				// minimum extra paragraphs needed
	USHORT	ehMaxAlloc;				// maximum extra paragraphs needed
	USHORT	shSS;							// initial (relative) SS value
	USHORT	ehSP;							// initial SP value
	USHORT	ehChecksum;				// checksum
	USHORT	ehIP;							// initial IP value
	USHORT	ehCS;							// initial (relative) CS value 
	USHORT	ehlpRelocation;		// file address of relocation table
	USHORT	ehOverlayNo;			// overlay number
	USHORT	ehReserved[16];		// reserved words
	LONG		ehPosNewHdr;			// file address of new exe header
} EXEHDR;										// eh

typedef struct {								// new .EXE header
	WORD		nhSignature;					// signature bytes
	CHAR		nhVer;						    // LINK version number
	CHAR		nhRev;								// LINK revision number
	WORD		nhoffEntryTable;			// offset of entry table
	WORD		nhcbEntryTable;				// number of bytes in entry table 
	LONG		nhCRC;								// checksum of whole file
	WORD		nhFlags;							// flag word
	WORD		nhAutoData;						// automatic data segment number
	WORD		nhHeap;								// initial heap allocation
	WORD		nhStack;							// initial stack allocation
	LONG		nhCSIP;								// initial CS:IP setting
	LONG		nhSSSP;								// initial SS:SP setting
	WORD		nhcSeg;								// count of file segments
	WORD		nhcMod;								// entries in Module Referance Table 
	WORD		nhcbNonResNameTable;	// size of non-resident name table
	WORD		nhoffSegTable;				// offset of segment table
	WORD		nhoffResourceTable;		// offset of resource table
	WORD		nhoffResNameTable;		// offset of resident name table
	WORD		nhoffModRefTable;			// offset of module referance table
	WORD		nhoffImpNameTable;		// offset to imported name table
	LONG		nhoffNonResNameTable;	// offset of non-resident name table
	WORD		nhcMovableEntries;		// count of movable entries
	WORD		nhcAlign;							// segment alignment shift count
	WORD		nhcRes;								// count of resourcesegments
	BYTE		nhExeType;						// target OS (OS/2=1, Windows=2)
	BYTE		nhFlagsOther;					// additional exe flags
	WORD		nhGangStart;					// offset to gangload area
	WORD		nhGangLength;					// length of gangload area
	WORD		nhSwapArea;						// minimum code swap area size
	WORD		nhExpVer;							// expected windows version number		
} NEWHDR;												// nh

typedef struct {
	char	nameLen;								// length of the function name.
	char	funcName[255];					// function name.
	int		ordinalNo;							// function oridnal number.
} FUNCNAME;
