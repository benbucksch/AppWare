////////////////////////////////////////////////////////////////////////////
//      This file contains the defines for menu item and control ids.
////////////////////////////////////////////////////////////////////////////

#ifndef __RESCONST_H__
#define __RESCONST_H__

// defines for items in the "File" popup

#define IDM_NEWPROJECT       100
#define IDM_NEWSUBJECT       101
#define IDM_OPEN             102
#define IDM_CLOSE            103
#define IDM_SAVE             104
#define IDM_SAVEAS           105
#define IDM_PAGESETUP        106
#define IDM_PRINT            107
#define IDM_DELETE           109
#define IDM_QUIT             110
#define IDM_IMPORTUPSF       111
#define IDM_EXPORTUPSF       112
#define IDM_FIND						 114
#define IDM_FINDNEXT				 115

// defines for items in the "Edit" popup

#define IDM_UNDO             120
#define IDM_CUT              121
#define IDM_COPY             122
#define IDM_PASTE            123
#define IDM_CLEAR            124
#define IDM_SELECTALL        126
#define IDM_SHOWSELECT       127
#define IDM_DODELETE         128

// defines for items in the "Project" popup

#define IDM_ARRANGESUBJECTS  130
#define IDM_PROJECTGRID      132
#define IDM_EDITICON         131
#define IDM_RUN              133
#define IDM_CREATEAPP        134
#define IDM_CONTINUERUN			 135
#define IDM_RESETRUN         136
#define IDM_PROJERRORS       137
#define IDM_PRINTSTORY       138
#define IDM_AUTOSAVE         139
#define IDM_CONFIRMAUTOSAVES 140
#define IDM_ENABLESTOPS      141
#define IDM_PROJCLEARSTOPS   142
#define IDM_STEP             143
#define IDM_IMPORTSPLASH		 144

// defines for items in the "Subject" popup

#define IDM_SKETCH           150
#define IDM_OBJECTALK        151
#define IDM_SUBJERRORS       152
#define IDM_SMALLICONS       154
#define IDM_TOGGLESTOPS      155
#define IDM_CLEARSTOPS       156
#define IDM_FUNCGRID         157
#define IDM_ACCURACY         158
#define IDM_COMMENT          159
#define IDM_ZOOMFUNCWIN      160
#define IDM_SIGNALNAME       161
#define IDM_SETSMALLICONS		 162
#define IDM_SETLARGEICONS		 163

// defines for items in the "Object" popup

#define IDM_OBJERRORS        170
#define IDM_MAKEPERSIST    	 171
#define IDM_SHARE            172
#define IDM_UNSHARE          173
#define IDM_ALIAS            174
#define IDM_MAKENONPERSIST	 175

// defines for items in the "Window" popup

#define IDM_PALETTE          180
#define IDM_NAVIGATOR        181
#define IDM_CASCADE          182
#define IDM_TILE             183
#define IDM_ARRANGE          184

// defines for items in the "Help" popup

#define IDM_HELP_INDEX       190
#define IDM_USING_HELP       191
#define IDM_ABOUT            192

// defines for the icon editor menu

#define IDM_IMPORT           204
#define IDM_SAVEICON         201
#define IDM_CLEARICON        202
#define IDM_RESTOREICON      203

// other defines


#define IDM_FIRSTCHILD       300

#define ID_HELP              350
#define ID_HELPCURSOR        351
#define ID_ESCAPE            352

#define ID_NEWNAME           104
#define ID_STATICTEXT        106

#define ID_OK                IDOK
#define ID_CANCEL            IDCANCEL

#define ID_BCOMMENT          104
#define ID_FCOMMENT          106

#define ID_SLIST             104
#define ID_OLIST             106

#define ID_OBJVALUE          104
#define ID_VALHSCROLL        105
#define ID_VALVSCROLL        106

// defines for string resources

#define STR_FRAMENAME        				211
#define STR_PROJECTNAME      				212
#define STR_SUBJECTNAME      				213
#define STR_NAVIGATOR        				214
#define STR_SHORTPROJECTNAME 				215
#define STR_COMPILING  			 				216
#define STR_CREATING   			 				217
#define STR_SHORTESTPROJECTNAME 		218

#define SUBJECT_NAME_LENGTH  32

//defines for PARAM class dialog box

#define PAR_EDIT             200
#define PAR_PARMNAME				 202


//defines for PRINTER class

#define PRT_JOB 	           WM_USER + 1
#define PRT_PRINTER          WM_USER + 2

//defines for SEARCHNAME dialogbox
#define SRCH_OBJECT	101
#define SRCH_TYPE	102
#define SRCH_FUNCTION	103
#define SRCH_KINDTEXT	104
#define SRCH_SEARCHTEXT	105

//bitmap ID's
#define IDBMP_8BIT_SPLASH		3000
#define IDBMP_4BIT_SPLASH		3002
#define IDBMP_UPARROW				3004

//icon ID's
#define				IDIC_APPWARE	 		4000		
#define				IDIC_PROJECTEXE		4002	 //this is hard coded in context.cpp		
#define				IDIC_SUBJECT			4004	
#define				IDIC_PROJECT			4008	
#define				IDIC_SEARCHWINDOW	4010

//Build Stuff
#define 			IDS_BUILDVER      5001	
#define 			IDS_BUILDDATE			5002

#endif
