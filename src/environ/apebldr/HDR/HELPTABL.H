#ifndef __HELPTABLE_H
#define __HELPTABLE_H

#include "menuhelp.h"

#define MENUHELPCT 53
#define OBJHELPCT 34

extern struct
	{
	DWORD	menuID;
  DWORD helpID;
	} menuHelpIdTable[] =
  {
	  { IDM_NEWPROJECT,    IDH_file_new_project },   
	  { IDM_NEWSUBJECT,    IDH_file_new_subject },   
	  { IDM_OPEN,          IDH_file_open_project },   
	  { IDM_CLOSE,         IDH_file_close },          
    { IDM_SAVE,          IDH_file_save },           
	  { IDM_SAVEAS,        IDH_file_save_as },        
	  { IDM_IMPORTUPSF,    IDH_file_import },         
	  { IDM_EXPORTUPSF,    IDH_file },         
	  { IDM_PRINT,         IDH_file_print },          
	  { IDM_PAGESETUP,     IDH_file_print_setup },    
	  { IDM_QUIT,          IDH_file_exit },           

	  { IDM_UNDO,          IDH_edit_undo },                
	  { IDM_CUT,           IDH_edit_cut },                 
	  { IDM_COPY,          IDH_edit_copy },                
	  { IDM_PASTE,         IDH_edit_paste },               
	  { IDM_CLEAR,         IDH_edit_delete },              
	  { IDM_SELECTALL,     IDH_edit_select_all },          
	  { IDM_SHOWSELECT,    IDH_edit_show_selection },      
		{	IDM_FIND,					 IDH_edit_find },               
		{	IDM_FINDNEXT,			 IDH_edit_find_next },          

    { IDM_PROJERRORS,      	 IDH_project_check_for_errors },        
	  { IDM_RUN,             	 IDH_project_run },                     
	  { IDM_CONTINUERUN,     	 IDH_project_continue },                
	  { IDM_RESETRUN,        	 IDH_project_reset },                   
    { IDM_ENABLESTOPS,   	   IDH_project_stops_enabled },           
    { IDM_PROJCLEARSTOPS,  	 IDH_project_clear_stops },             
    { IDM_AUTOSAVE,        	 IDH_project_auto_save  },              
    { IDM_CONFIRMAUTOSAVES,	 IDH_project_confirm_saves },          
	  { IDM_ARRANGESUBJECTS, 	 IDH_project_arrange_subject_icons },  
	  { IDM_PROJECTGRID,     	 IDH_project_use_subject_grid },       
	  { IDM_EDITICON,        	 IDH_project_edit_application_icon },  
    { IDM_CREATEAPP,       	 IDH_project_create_application },      

    { IDM_SUBJERRORS,     	 IDH_subject_check_for_errors },    
		{ IDM_SMALLICONS,     	 IDH_subject_use_small_icons },     
		{	IDM_SETSMALLICONS,		 IDH_subject_set_to_small_icons },  
		{	IDM_SETLARGEICONS,		 IDH_subject_set_to_large_icons },  
    { IDM_SIGNALNAME,     	 IDH_subject_show_flow_names },     
	  { IDM_FUNCGRID,       	 IDH_subject_use_grid },            
    { IDM_TOGGLESTOPS,    	 IDH_subject_toggle_stops },        
    { IDM_CLEARSTOPS,     	 IDH_subject_clear_stops },         
	  { IDM_COMMENT,        	 IDH_subject_new_comment },         

		{ IDM_OBJERRORS,      	 IDH_object_check_for_errors },     
		{ IDM_MAKEPERSIST,			 IDH_object_make_persistent },      
		{ IDM_MAKENONPERSIST,		 IDH_object_make_nonpersistent },   
    { IDM_SHARE,          	 IDH_object_share },                
    { IDM_UNSHARE,        	 IDH_object_unshare },              
    { IDM_ALIAS,          	 IDH_object_new_alias },            

	  { IDM_PALETTE,         	 IDH_windows_hide_palette },   
	  { IDM_NAVIGATOR,       	 IDH_windows_show_navigator }, 
	  { IDM_CASCADE,         	 IDH_windows_cascade },        
	  { IDM_TILE,            	 IDH_windows_tile },           
	  { IDM_ARRANGE,         	 IDH_windows_arrange_icons },  

	  { IDM_HELP_INDEX,     	 IDH_help_index },      
	  { IDM_USING_HELP,     	 IDH_help_using_help },  
	  { IDM_ABOUT,          	 IDH_help_about }       
	};

extern struct
	{
	char	catName[25];
  DWORD catID;
	} categoryHelpIdTable[] =
	{
    { "Application",     HELPID_CAT_Sysm },
    { "Browser",         HELPID_CAT_Brws },
    { "Bundle",          HELPID_CAT_Bndl },
    { "Button",          HELPID_CAT_Butn },
    { "Calculation",     HELPID_CAT_Calc },
    { "Cursor",          HELPID_CAT_Acur },
    { "Database",        HELPID_CAT_Datb },
    { "Date",            HELPID_CAT_Date },
    { "File",            HELPID_CAT_File },
    { "File Operations", HELPID_CAT_Flop },
    { "General",         HELPID_CAT_Genl },
    { "Group",           HELPID_CAT_Grup },
    { "Import-Export",   HELPID_CAT_Imex },
    { "Keyboard",        HELPID_CAT_Keyb },
    { "Launcher",        HELPID_CAT_Appl },
    { "List",            HELPID_CAT_List },
    { "Loop",            HELPID_CAT_Loop },
    { "Menu",            HELPID_CAT_Menu },
    { "Menu Bar",        HELPID_CAT_Mbar },
    { "Menu Item",       HELPID_CAT_Mitm },
    { "Notification",    HELPID_CAT_Ntfy },
    { "Number",          HELPID_CAT_Nmbr },
    { "Page Layout",     HELPID_CAT_Page },
    { "Picture",         HELPID_CAT_Pict },
    { "Printer",         HELPID_CAT_Prnt },
    { "Radio Group",     HELPID_CAT_Rdgp },
    { "Scale",           HELPID_CAT_Scal },
    { "Subroutine",      HELPID_CAT_Subr },
    { "Table",           HELPID_CAT_Tabl },
    { "Text",            HELPID_CAT_Text },
    { "Text Factory",    HELPID_CAT_Tfct },
    { "Time",            HELPID_CAT_Time },

    { "Window",          HELPID_CAT_Wino }
  };


#endif
