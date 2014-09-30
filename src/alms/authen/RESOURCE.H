/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#ifndef __RESOURCE_H_
#define __RESOURCE_H_

/*
 * Buttons and dialog definitions for the pauthen pop-up window
 */
#define AUTHEN_DEFAULT_BUTTON		101
#define AUTHEN_NDSONLY_BUTTON		102
#define AUTHEN_BINDERYONLY_BUTTON	103
#define AUTHEN_DIALOG_SERVER			104
#define AUTHEN_DIALOG_USER			105
#define AUTHEN_DIALOG_PASSWORD		106
#define AUTHEN_DIALOG_VERSION		107

#define LOGIN_DIALOG_SERVER			201
#define LOGIN_DIALOG_USER				202
#define LOGIN_DIALOG_PASSWORD		203
#define LOGIN_DEFAULT_BUTTON			204
#define LOGIN_NDSONLY_BUTTON			205
#define LOGIN_BINDERYONLY_BUTTON	206
#define LOGIN_DIALOG_PROFILE			207
#define LOGIN_DIALOG_SCRIPT			208
#define LOGIN_DIALOG_GROUP			209
#define LOGIN_DIALOG_VERSION			210

#define SCRIPT_DIALOG_BOX				301
#define SCRIPT_DOPROCESS				302
#define IDCONTINUE						303

#define LOGOUT_DIALOG_SERVER			401
#define LOGOUT_TREE_BUTTON			402
#define LOGOUT_SERVER_BUTTON			403
#define LOGOUT_DIALOG_GROUP			404
#define LOGOUT_DIALOG_VERSION		405

/*
 * Resource string defines.
 */
#define NO_SERVER										1
#define NO_USER											2
#define CONNECTION_FULL								3
#define MEMORY_ERROR									4
#define DIFF_USER_SAME_TREE							5
#define SYSTEM_ERROR									6
#define NO_OBJECT_BAD_PASSWORD						7
#define NO_SERVER_OR_TREE								8
#define AUTHENTICATION_DENIED						9
#define LOGOUT_ERROR									10
#define NO_DEFAULT_CONN								11
#define NO_DRIVE_SPECIFIED							12
#define NO_PATH_SPECIFIED								13
#define ALL_DRIVES_MAPPED								14
#define BAD_INPUT_TABLE								15
#define NO_MAPPINGS_SPECIFIED						16
#define MAPDRIVES_ERROR								17
#define TRUE_STRING										18
#define FALSE_STRING									19
#define INVALID_DRIVE									20
#define DELETEMAP_ERROR								21
#define NO_DRIVE_MAPPED								22
#define VOLUME_NOT_EXIST								23
#define NO_MAP_LOCAL									24
#define UNKNOWN_SERVER									25
#define NO_CONNECTION_TOSERVER						26
#define NOT_LOGGED_IN									27
#define INVALIDPATH										28
#define COLUMN1_HEADER									29
#define COLUMN2_HEADER									30
#define COLUMN3_HEADER									31
#define COLUMN1_FORMAT									32
#define COLUMN2_NDSFORMAT								33
#define COLUMN2_FORMAT									34
#define INVALID_LOGIN_TYPE							35
#define DS_TO_BINDERY_SERVER							36
#define LOGOUT_TREE_W_SERVER							37
#define OK_OR_STOP										38
#define NO_SCRIPT_SPECIFIED							39
#define SCRIPT_ERROR									40
#define NO_LOGIN_PNW									41
#define CANNOT_FREE_CONNECTIONSLOT					42
#define BAD_PRIMARY_CONNECTIONID					43
#define MSG_VERSION										44
#define COPYRIGHT										45
#define CANNOT_CREATE_CONTEXT						46
#define SYSTEM_SCRIPT_FORMAT							47
#define USER_SCRIPT_FORMAT							48
#define NO_LOGOUT										49
#define SCRIPT_LINE_TOO_LONG							50
#define SCRIPT_GEN_ERROR								51
#define DEFAULT_SCRIPTNAME							52
#define SCRIPT_NO_END_QUOTE							53
#define SCRIPT_NO_PARSE								54
#define SCRIPT_NO_GOTO									55
#define SCRIPT_IF_NESTED								56
#define CANNOT_OPEN_SCRIPTFILE						57
#define SCRIPT_BAD_SYNTAX								58
#define SCRIPT_TOO_MANY_ARGS							59
#define SCRIPT_STRING_TABLE							60
#define SCRIPT_CANONICALIZE							61
#define DIVIDE_BY_ZERO									62
#define SCRIPT_NO_LABEL								63
#define SCRIPT_NO_VALID_CONNECTION					64
#define MON_JANUARY										65
#define MON_FEBRUARY									66
#define MON_MARCH										67
#define MON_APRIL										68
#define MON_MAY											69
#define MON_JUNE											70
#define MON_JULY											71
#define MON_AUGUST										72
#define MON_SEPTEMBER									73
#define MON_OCTOBER										74
#define MON_NOVEMBER									75
#define MON_DECEMBER									76
#define DAY_SUN											77
#define DAY_MON											78
#define DAY_TUES											79
#define DAY_WED											80
#define DAY_THURS										81
#define DAY_FRI											82
#define DAY_SAT											83
#define TIME_AM											84
#define TIME_PM											85
#define MORNING											86
#define AFTERNOON										87
#define EVENING											88
#define SCRIPT_PROPERTY_ERROR						89
#define PAUTHEN_CANCEL									90
#define PAUTHEN_DIFF_USER								91
#define PAUTHEN_ERROR									92
#define PAUTHEN_DS_OK									93
#define PAUTHEN_BINDERY_OK							94
#define SCRIPT_ATTACHED								95
#define SCRIPT_CANON_CONTEXT							96
#define SCRIPT_CONTEXT_INVALID						97
#define SCRIPT_READ_OBJECTINFO						98
#define SCRIPT_CANNOT_SET_CONTEXT					99
#define SCRIPT_CONTEXT_CHANGED						100
#define SCRIPT_PARSE_PATH								101
#define SCRIPT_CANT_SET_DRIVE						102
#define SCRIPT_NOT_SUPPORTED_EXIT					103
#define SCRIPT_UNKNOWN_COMMAND						104
#define SCRIPT_BAD_FLOW_COMMAND						105
#define MAPPINGS_TO_LOCAL_DISK						107
#define SINGLE_MAP_TO_LOCAL_DISK					106		
#define SCRIPT_NOT_SUPPORTED_SEARCH_DRIVE		108
#define FULL_DS_MAPPED_PATH							109
#define FULL_MAPPED_PATH								110
#define DISK_UNDEFINED									111
#define SCRIPT_PAUSE									112
#define SCRIPT_PUTENV									113
#define SCRIPT_NOT_SUPPORTED_COMSPEC				114
#define DISPLAY_LAST_LOGIN_TIME						115
#define SCRIPT_UNEXPECTED_INTERNAL_ERROR			116
#define SCRIPT_SPAWN_TOO_BIG							117
#define SCRIPT_SPAWN_NOT_FOUND						118
#define SCRIPT_SPAWN_NO_EXEC							119
#define SCRIPT_SPAWN_NO_MEM							120
#define SCRIPT_SPAWN_UNKNOWN							121
#define SCRIPT_CANNOT_LOGOUT							122
#define SCRIPT_MUST_BE_DOS31							123
#define ANSWER_YES										124
#define ANSWER_NO										125
#define SCRIPT_DATE_TIME								126
#define SCRIPT_NOT_SUPPORTED_EXTERNAL_COMMAND	127
#define E_UNKNOWN_NAME									128
#define SCRIPT_CANT_GET_CONNECTION_INFO			129
#define SCRIPT_NO_SHELL								130
#define SCRIPT_PAUSE_CAPTION							131
#define ERR_EXE_LOGINSCRIPTS							132
#define SCRIPT_PROMPTED_DEFAULT						133
#define PROFILE_PROMPTED_DEFAULT					134
#define SCRIPT_GEN_QUESTION							135
#define SCRIPT_PROCESSING_OKAY						136
#define SCRIPT_PROCESSING_ERROR						137
#define SCRIPT_CANT_INCLUDE							138
#define SCRIPT_CANT_OPEN_SCRIPT						139
#define VERSION_STRING									140
#define NO_CONNECTIONS									141
#define PWD_EXPIRED										142
#define PAUTHEN_DS_OK_PE								143
#define PAUTHEN_BINDERY_OK_PE						144

#endif
