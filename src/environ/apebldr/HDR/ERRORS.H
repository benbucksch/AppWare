////////////////////////////////////////////////////////////////////////////
//  Constants used by the various error checking routines
////////////////////////////////////////////////////////////////////////////

#define CHECK_NOERRORS  					0  	//no errors were found
#define CHECK_CONTINUE  					1  	//errors were found, but continue checking
#define CHECK_CANCEL    					2  	//errors were found, and stop checking
#define CHECK_WARNING   					4  	//errors/warnings found but object stable
#define CHECK_DUMMYALIASWARNING   8		//unlinked alias found
#define CHECK_ERROR								16  //errors found
