#ifndef __CLASSDEF_H
#define __CLASSDEF_H

////////////////////////////////////////////////////////////////////
//APPBUILDER GRAPHIC Classes

class 		WINDOWLIST;
typedef 	WINDOWLIST * 					LPWINDOWLIST;

class 		ZLIST;
typedef 	ZLIST * 								LPZLIST;

class 		PRINTER;
typedef 	PRINTER * 							LPPRINTER;

class 		BASEWINDOW;
typedef 	BASEWINDOW * 					LPBASEWINDOW;

class 		NAVIGATORITEM;
typedef 	NAVIGATORITEM * 				LPNAVIGATORITEM;

class 		PRINTLABEL;
typedef 	PRINTLABEL * 					LPPRINTLABEL;

class 		COMPILESTATUS;
typedef 	COMPILESTATUS * 				LPCOMPILESTATUS;

class 		SCOPESTACK;
typedef 	SCOPESTACK * 					LPSCOPESTACK;

class 		BASETYPE;
typedef 	BASETYPE * 						LPBASETYPE;

class			DIBITMAP;
typedef		DIBITMAP *							LPDIBITMAP;

class 		FLOWSTACK;
typedef 	FLOWSTACK * 						LPFLOWSTACK;

class 		VOIDSTACK;
typedef		VOIDSTACK *						LPVOIDSTACK;

class			PRINTFLOWSTACK;
typedef 	PRINTFLOWSTACK *				LPPRINTFLOWSTACK;

class 		FONT;
typedef 	FONT * 								LPFONT;

class 		HELPCLS;
typedef 	HELPCLS * 							LPHELPCLS;

class 		MDI;
typedef		MDI * 									LPMDI;

class 		MESSAGEWIN;
typedef 	MESSAGEWIN * 					LPMESSAGEWIN;

class 		NAVIGATOR;
typedef 	NAVIGATOR * 						LPNAVIGATOR;

class 		NAVIGATORITEM;
typedef 	NAVIGATORITEM * 				LPNAVIGATORITEM;

class 		OBJECTSHARE;
typedef 	OBJECTSHARE * 					LPOBJECTSHARE;

class 		PALETTE;
typedef 	PALETTE * 							LPPALETTE;

class 		POPUPMENU;
typedef 	POPUPMENU * 						LPPOPUPMENU;

class 		PRINTER;
typedef 	PRINTER * 							LPPRINTER;

class 		PROJECTWINDOW;
typedef 	PROJECTWINDOW * 				LPPROJECTWINDOW;

class 		PROGSYSTEM;
typedef 	PROGSYSTEM * 					LPPROGSYSTEM;

class 		SUBJECTWINDOW;
typedef 	SUBJECTWINDOW * 				LPSUBJECTWINDOW;

class 		SEARCHMANAGER;
typedef 	SEARCHMANAGER * 				LPSEARCHMANAGER;

class			BITREE;
typedef		BITREE *								LPBITREE;

class			BITREEMEMBER;
typedef		BITREEMEMBER *					LPBITREEMEMBER;

class			UNIQUETREE;
typedef 	UNIQUETREE *						LPUNIQUETREE;

///////////////////////////////////////////////////////////////////
//APPBUILDER PROJECT STRUCTURE classes
class 		SUBJECT;
typedef 	SUBJECT * 							LPSUBJECT;

class 		COMMENT;
typedef 	COMMENT * 							LPCOMMENT;

class 		OBJINST;
typedef 	OBJINST * 							LPOBJINST;

class 		PARAM;
typedef 	PARAM * 								LPPARAM;

class 		OBJGROUP;
typedef 	OBJGROUP * 						LPOBJGROUP;

class 		WORKSHEET;
typedef 	WORKSHEET * 						LPWORKSHEET;

class 		PROJECT;
typedef 	PROJECT * 							LPPROJECT;

class 		OBJTYPE;
typedef 	OBJTYPE * 							LPOBJTYPE;

class 		OBJECTRECORD;
typedef 	OBJECTRECORD * 				LPOBJECTRECORD;

class 		FUNCINST;
typedef 	FUNCINST * 						LPFUNCINST;

class 		FLOW;
typedef 	FLOW * 								LPFLOW;

class 		FLOWMGR;
typedef 	FLOWMGR * 							LPFLOWMGR;

class 		FUNCINST;
typedef 	FUNCINST * 						LPFUNCINST;

class 		FUNCWINOBJ;
typedef 	FUNCWINOBJ * 					LPFUNCWINOBJ;

class 		OBJECTRECORD;
typedef 	OBJECTRECORD * 				LPOBJECTRECORD;

class 		PARAMMASK;
typedef 	PARAMMASK * 						LPPARAMMASK;

class			SUBTYPE;
typedef 	SUBTYPE *							LPSUBTYPE;


////////////////////////////////////////////////////////////////
//Miscellaneous Classes
class			MASTERTYPELIST;
typedef		MASTERTYPELIST * 			LPMASTERTYPELIST;

class			ADMNNODE;
typedef		ADMNNODE *							LPADMNNODE;

////////////////////////////////////////////////////////////////
//CLIPBOARD Classes

class 		CLIPSJT;
typedef 	CLIPSJT * 							LPCLIPSJT;

class 		CLIPCOMMENT;
typedef 	CLIPCOMMENT * 					LPCLIPCOMMENT;

class 		CLIPFLOW;
typedef 	CLIPFLOW * 						LPCLIPFLOW;

class 		CLIPFUNC;
typedef 	CLIPFUNC * 						LPCLIPFUNC;

class 		CLIPHEAD;
typedef 	CLIPHEAD * 						LPCLIPHEAD;

class 		CLIPOBJ;
typedef 	CLIPOBJ * 							LPCLIPOBJ;

class 		CLIPPARAM;
typedef 	CLIPPARAM * 						LPCLIPPARAM;

class 		CLIPGROUP;
typedef		CLIPGROUP *						LPCLIPGROUP;

////////////////////////////////////////////////////////////////////
//MEMORY STUFF Classes

class 		MEMORY;
typedef 	MEMORY * 							LPMEMORY;

union 		SERUID;
typedef 	SERUID * 							LPSERUID;

class     BASEBLOCK;
typedef   BASEBLOCK *            LPBASEBLOCK;

class 		SERIUSBLOCK;
typedef 	SERIUSBLOCK * 					LPSERIUSBLOCK;

class 		SERIUSARRAY;
typedef 	SERIUSARRAY *					LPSERIUSARRAY;

class 		SERIUSRECORD;
typedef 	SERIUSRECORD *					LPSERIUSRECORD;

class 		BASERECORDBLOCK;
typedef 	BASERECORDBLOCK *			LPBASERECORDBLOCK;

class 		RECORDBLOCK;
typedef 	RECORDBLOCK *					LPRECORDBLOCK;

struct 		RECORDPREFIX;
typedef 	RECORDPREFIX *					LPRECORDPREFIX;

class 		DBSTRINGARRAY;
typedef 	DBSTRINGARRAY * 				LPDBSTRINGARRAY;

union 		DBSTRING;
typedef 	DBSTRING *							LPDBSTRING;

struct 		FREEBLOCK;
typedef 	FREEBLOCK * 						LPFREEBLOCK;

struct 		MEMCHUNK;
typedef 	MEMCHUNK * 						LPMEMCHUNK;

struct 		SERIUSMEMORY;
typedef 	SERIUSMEMORY *					LPSERIUSMEMORY;

class 		EXERECORDBLOCK;
typedef 	EXERECORDBLOCK *				LPEXERECORDBLOCK;

class 		EXESERIUSBLOCK;
typedef 	EXESERIUSBLOCK *				LPEXESERIUSBLOCK;

class 		EXESERIUSRECORD;
typedef 	EXESERIUSRECORD *			LPEXESERIUSRECORD;

union 		EXESTRING;
typedef 	EXESTRING *						LPEXESTRING;

class 		FASTRECMGR;
typedef 	FASTRECMGR * 					LPFASTRECMGR;

class 		FILEOBJECT;
typedef 	FILEOBJECT * 					LPFILEOBJECT;

class 		LIST;
typedef 	LIST * 								LPLIST;

class 		LISTMEMBER;
typedef 	LISTMEMBER * 					LPLISTMEMBER;

class 		MEMBLOCK;
typedef 	MEMBLOCK *							LPMEMBLOCK;

class 		MEMORY;
typedef 	MEMORY * 							LPMEMORY;

class 		OWNERLIST;
typedef 	OWNERLIST *  					LPOWNERLIST;

struct 		FREEHEAD;
typedef 	FREEHEAD * 						LPFREEHEAD;

struct 		FREETAIL;
typedef 	FREETAIL *							LPFREETAIL;

struct 		STRHEAD;
typedef 	STRHEAD *							LPSTRHEAD;

struct 		BLOCKHEADER;
typedef 	BLOCKHEADER *					LPBLOCKHEADER;

struct 		STRINGBLOCK;
typedef 	STRINGBLOCK *					LPSTRINGBLOCK;

class 		STRINGSPACE;
typedef 	STRINGSPACE *					LPSTRINGSPACE;

class 		SYNTAX;
typedef 	SYNTAX *								LPSYNTAX;

class 		TEMPDATAS;
typedef 	TEMPDATAS *  					LPTEMPDATAS;

class 		TEMPOBJECT;
typedef 	TEMPOBJECT *  					LPTEMPOBJECT;

class 		TSTRING;
typedef 	TSTRING *							LPTSTRING;

class 		UIDARRAY;
typedef 	UIDARRAY * 						LPUIDARRAY;

class 		UIDLIST;
typedef 	UIDLIST * 							LPUIDLIST;

class 		UIDSTACK;
typedef 	UIDSTACK * 						LPUIDSTACK;

class 		UIDTABLE;
typedef 	UIDTABLE *							LPUIDTABLE;

class     RUNTIMERECLIST;
typedef   RUNTIMERECLIST *       LPRUNTIMERECLIST;

class     RUNTIMEREC;
typedef   RUNTIMEREC *           LPRUNTIMEREC;


///////////////////////////////////////////////////////////////////
//EXECUTABLE STUFF Classes
class 		EXEHEAD;
typedef 	EXEHEAD * 							LPEXEHEAD;

class 		EXEFLOW;
typedef 	EXEFLOW * 							LPEXEFLOW;

class 		EXEFUNCINST;
typedef 	EXEFUNCINST * 					LPEXEFUNCINST;

class 		EXEBEAD;
typedef 	EXEBEAD * 							LPEXEBEAD;

class 		EXEBEADLIST;
typedef 	EXEBEADLIST *					LPEXEBEADLIST;

class 		EXEEVENT;
typedef 	EXEEVENT * 						LPEXEEVENT;

class 		EXEEVENTLIST;
typedef 	EXEEVENTLIST *					LPEXEEVENTLIST;

class 		EXEFLOWMGR;
typedef 	EXEFLOWMGR * 					LPEXEFLOWMGR;

class 		EXEINFO;
typedef 	EXEINFO *							LPEXEINFO;

struct 		EXEINFOAUX;
typedef 	EXEINFOAUX *						LPEXEINFOAUX;

class 		EXELIB;
typedef 	EXELIB * 							LPEXELIB;

class 		EXELIBLIST;
typedef 	EXELIBLIST *						LPEXELIBLIST;

class 		EXELISTBASE;
typedef 	EXELISTBASE *					LPEXELISTBASE;

class 		EXEOBJINST;
typedef 	EXEOBJINST * 					LPEXEOBJINST;

class 		EXEPARAM;
typedef 	EXEPARAM * 						LPEXEPARAM;

class 		EXEPROC;
typedef 	EXEPROC * 							LPEXEPROC;

class 		EXEPROCLIST;
typedef 	EXEPROCLIST *					LPEXEPROCLIST;

class 		EXERESPONSE;
typedef 	EXERESPONSE * 					LPEXERESPONSE;

class 		EXERESPONSELIST;
typedef 	EXERESPONSELIST *			LPEXERESPONSELIST;

class 		EXEOBJTYPE;
typedef 	EXEOBJTYPE *						LPEXEOBJTYPE;

//////////////////////////////////////////////////////////////////
//CONFIGURATION Classes

class 		ACFGDESC;
typedef 	ACFGDESC *							LPCFGDESC;

class 		COLORPALETTE;
typedef 	COLORPALETTE * 				LPCOLORPALETTE;

class 		ACOMPDESC;
typedef 	ACOMPDESC *							LPCOMPDESC;

class 		DEVSYSTEM;
typedef 	DEVSYSTEM * 						LPDEVSYSTEM;

class 		AEVENTDESC;
typedef 	AEVENTDESC *						LPEVENTDESC;

struct    AFLOWDESC;
typedef 	AFLOWDESC *							LPFLOWDESC;


class 		FLOWDESCARRAY;
typedef 	FLOWDESCARRAY *				LPFLOWDESCARRAY;

class 		AFUNCDESC;
typedef         AFUNCDESC *                                                     LPAFUNCDESC;

class 		ICONEDITOR;
typedef 	ICONEDITOR * 					LPICONEDITOR;

class 		MDIAPP;
typedef 	MDIAPP *								LPMDIAPP;

class 		MDICHILD;
typedef 	MDICHILD * 						LPMDICHILD;

struct 		METHODDESC;
typedef 	METHODDESC *						LPMETHODDESC;

struct 		AOBJDESC;
typedef 	AOBJDESC *    					LPOBJDESC;

struct		APARAMDESC;
typedef 	APARAMDESC *						LPPARAMDESC;

class 		PARAMDESCARRAY;
typedef 	PARAMDESCARRAY *				LPPARAMDESCARRAY;

class 		PIXELWINDOW;
typedef 	PIXELWINDOW * 					LPPIXELWINDOW;

class			RESPONSE;
typedef		RESPONSE *							LPRESPONSE;

class 		RESPONSEARRAY;
typedef 	RESPONSEARRAY *				LPRESPONSEARRAY;

class 		ROUTINES;
typedef 	ROUTINES *							LPROUTINES;

class 		SERIUSICON;
typedef 	SERIUSICON *						LPSERIUSICON;


//temp defs so I can compile
class SERIUSBLOCKBLOCK;
typedef SERIUSBLOCKBLOCK * LPSERIUSBLOCKBLOCK;

class EXESERIUSBLOCKBLOCK;
typedef EXESERIUSBLOCKBLOCK * LPEXESERIUSBLOCKBLOCK;


#endif
