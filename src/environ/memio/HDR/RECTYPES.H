// rectypes.h
//
// jon 19-Oct-92 EXEPROCUIDLISTs are now EXEBEADLISTs

#ifndef __RECTYPES_H__
#define __RECTYPES_H__

#define RT_CFGDESC			1
#define RT_OBJDESC			2
#define RT_FUNCDESC			3
#define RT_EVENTDESC		4
#define RT_METHODDESC		5
#define RT_SERIUSBLOCK 	6
#define RT_RECORDBLOCK	7
#define RT_FLOWDESC			8
#define RT_PARAMDESC		9
#define RT_PROJECT			10
#define RT_SUBJECT			11
#define RT_FUNCINST			12
#define RT_OBJINST			13
#define RT_OBJTYPE			14
#define RT_FLOW					15
#define RT_PARAM				16
#define RT_COMMENT      17
#define RT_OBJGROUP			30
#define RT_SUBTYPE			31

#define	RT_EXESERIUSBLOCK		18

#define RT_CLIPHEAD					21
#define RT_CLIPSJT					22
#define RT_CLIPPARAM        23
#define RT_CLIPFUNC					24
#define RT_CLIPFLOW					25
#define RT_CLIPOBJ					26
#define RT_CLIPCOMMENT			27
#define RT_CLIPGROUP				28

//exe record types must be negative

#define	RT_EXERECORDBLOCK		-1
#define RT_MEMBLOCK					-2

#define RT_EXEOBJINST				-3
#define RT_EXEOBJTYPE				-4
#define RT_EXEFUNCINST			-5
#define RT_EXEPARAM					-6
#define RT_EXEFLOW					-7
#define RT_EXEHEADREC       -8

#define RT_EXELISTBASE      -9

#define RT_EXEBEADLIST      -10

#define RT_EXELIBLIST       -11
#define RT_EXEEVENTLIST     -12
#define RT_EXERESPONSELIST  -13
#define RT_EXEPROCLIST      -14



#endif
