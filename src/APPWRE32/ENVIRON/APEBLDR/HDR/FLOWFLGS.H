////////////////////////////////////////////////////////////////////////
//
// flowflgs.h -- values for flags member of FLOW and EXEFLOW
//
//  jon  5-Aug-92  ripped out of FLOW.H so EXEFLOW.H could share
//  jon  5-Aug-92  added FL_STOP
//
////////////////////////////////////////////////////////////////////////


	#define FL_DRAW 				0x00000001
	#define FL_SELECTED 		0x00000002
	#define FL_FUNCSOURCE 	0x00000004
	#define FL_OBJSOURCE  	0x00000008
	#define FL_SIGNALNAME 	0x00000010
	#define FL_STOP       	0x00000020  // a debugger Stop is set on this flow
	#define FL_STOPPED    	0x00000040  // program being debugged has reached
																	// and stopped at this particular flow
	#define FL_STOPDISABLED 0x00000080 //flow stop has been disabled

	#define FL_PRINT      	0x00000100  // indicates that flow needs to be printed
																	// since either the flow is a signal or
																	// it is part of a group of multiple flows
																	// coming off of a function.

	#define FL_CLOSEPRINT 	0x00000200  // indicates that flow needs closing
																	// brackets printed only


	//used internally for drawing purposes
	#define FL_DRAWSIGNAL 0x00000400

	//This bit is set during compile time.  It's never cleared with the
	//following assumptions:
	//	- If the flow is copied during debugging time, the FL_LINKEDTOEXEFLOW bit
	//		must not be copied over.
	//	-	When testing to see if the bit is set, two tests are needed. One to
	//		see if it's run time and another to see if the bit is set.
	//Other than runtime, we should never care if this bit is set or not.
	#define FL_LINKTOEXEFLOW	0x00001000

	//indicates that object is an unlinked alias
	#define FL_SIGNALNOTLINKED		0x00004000

	//used during object version verification to indicate that a flow which
	//was incompatable with the current version has already been changed.
	#define FL_SIGNALUPDATED			0x00008000

// ---- end of flowflgs.h ----
