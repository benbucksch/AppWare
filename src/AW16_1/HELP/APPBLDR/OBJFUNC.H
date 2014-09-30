#define  idh_contents                 1012


/* CALENDAR */
#define  IDH_calendar	          1018
#define  IDH_clear_one		      1088
#define  IDH_clear_range	      1391
#define  IDH_get_alarm		      1396
#define  IDH_get_date		      1397
#define  IDH_n1_get_selection     1398
#define  IDH_next_month		      1089
#define  IDH_next_year		      1090
#define  IDH_previous_month	      1091
#define  IDH_previous_year	      1092
#define  IDH_set_date		      1093
#define  IDH_set_general	      1094
#define  IDH_set_selection	      1095
#define  IDH_set_specific	      1096
#define  IDH_table_to_events      1097
#define  IDH_view_one		      1098
#define  IDH_view_range		      1099


/* CATEGORIES */

#define  IDH_cat_application          2000
#define  IDH_cat_browser              2005
#define  IDH_cat_bundle               2010
#define  IDH_cat_button               2015
#define  IDH_cat_calculation          2020
#define  IDH_cat_calendar             2025
#define  IDH_cat_connection           2030
#define  IDH_cat_cursor               2035
#define  IDH_cat_database             2040
#define  IDH_cat_date                 2045

#define  IDH_cat_dde_client           2050
#define  IDH_cat_dde_server           2055
#define  IDH_cat_dll                  2060
#define  IDH_cat_file                 2065
#define  IDH_cat_file_operations      2070
#define  IDH_cat_file_transfer        2075
#define  IDH_cat_flag                 2080
#define  IDH_cat_general              2085
#define  IDH_cat_group                2090             
#define  IDH_cat_import_export        2095

#define  IDH_cat_keyboard             2100
#define  IDH_cat_launch               2105
#define  IDH_cat_list                 2110
#define  IDH_cat_loop                 2115
#define  IDH_cat_menu                 2120
#define  IDH_cat_menu_bar             2125
#define  IDH_cat_menu_item            2130
#define  IDH_cat_movie                2135
#define  IDH_cat_movie_controller     2140
#define  IDH_cat_movie_display        2145

#define  IDH_cat_movie_editing        2150
#define  IDH_cat_movie_import_export  2155
#define  IDH_cat_movie_info           2160
#define  IDH_cat_movie_metrics        2165
#define  IDH_cat_notification         2170
#define  IDH_cat_number               2175
#define  IDH_cat_odbc_connection      2180
#define  IDH_cat_odbc_sql             2185
#define  IDH_cat_ole                  2190
#define  IDH_cat_oracle_connection    2200

#define  IDH_cat_oracle_sql           2205
#define  IDH_cat_page_layout          2210
#define  IDH_cat_picture              2215
#define  IDH_cat_polybutton           2220
#define  IDH_cat_printer              2225
#define  IDH_cat_radio_group          2230
#define  IDH_cat_scale                2235
#define  IDH_cat_scroll_bar           2240
#define  IDH_cat_signal               2245
#define  IDH_cat_sound                2250

#define  IDH_cat_subroutine           2255
#define  IDH_cat_table                2260
#define  IDH_cat_terminal             2265
#define  IDH_cat_text                 2270
#define  IDH_cat_text_builder         2275
#define  IDH_cat_time                 2280
#define  IDH_cat_window               2285


/*========= BEGIN AUTO GENERATED DEFINES ===========*/

/*  Configuration File: APPLICAT.CFG
 *  Function Category:  Application
 */

/* Objects */
#define IDH_application                      0x5379736D    /* Sysm */
/* Functions */
#define IDH_function_priority                0x66507479    /* fPty */
#define IDH_visual_priority                  0x76507479    /* vPty */
#define IDH_idle                             0x7349646C    /* sIdl */
#define IDH_stop_idle                        0x7449646C    /* tIdl */
#define IDH_update_windows                   0x75506477    /* uPdw */
#define IDH_mouse_button                     0x6D44776E    /* mDwn */
#define IDH_save_persistents                 0x73507273    /* sPrs */
#define IDH_get_command_line                 0x67436D64    /* gCmd */


/*  Configuration File: BROWSER.CFG
 *  Function Category:  Browser
 */

/* Objects */
#define IDH_browser                          0x42727773    /* Brws */
/* Functions */
#define IDH_cancel_operation                 0x6252636C    /* bRcl */


/*  Configuration File: BUNDLE.CFG
 *  Function Category:  Bundle
 */

/* Objects */
#define IDH_bundle                           0x426E646C    /* Bndl */
/* Functions */
#define IDH_package_bundle                   0x70426E64    /* pBnd */


/*  Configuration File: BUTTON.CFG
 *  Function Category:  Button
 */

/* Objects */
#define IDH_button                           0x4275746E    /* Butn */
/* Functions */
#define IDH_clear_button                     0x62436C72    /* bClr */
#define IDH_disable_button                   0x62447362    /* bDsb */
#define IDH_enable_button                    0x62456E62    /* bEnb */
#define IDH_get_title                        0x62477474    /* bGtt */
#define IDH_button_set                       0x62547374    /* bTst */
#define IDH_set_button                       0x62536574    /* bSet */
#define IDH_set_title                        0x62537474    /* bStt */


/*  Configuration File: CALCULAT.CFG
 *  Function Category:  Calculation
 */

/* Objects */
#define IDH_calculation                      0x43616C63    /* Calc */
/* Functions */
#define IDH_calculate                        0x63416C63    /* cAlc */
#define IDH_set_expression                   0x63416C65    /* cAle */
#define IDH_auto_update_on                   0x63416F6E    /* cAon */
#define IDH_auto_update_off                  0x63416F66    /* cAof */


/*  Configuration File: COMMCNT.CFG
 *  Function Category:  Connection
 */

/* Objects */
#define IDH_connection                       0x436F6E6E    /* Conn */
/* Functions */
#define IDH_open_connection                  0x634E6F70    /* cNop */
#define IDH_close                            0x634E636C    /* cNcl */
#define IDH_break                            0x634E6272    /* cNbr */
#define IDH_configure                        0x634E6667    /* cNfg */
#define IDH_unschedule_text                  0x634E7578    /* cNux */
#define IDH_send_text                        0x634E7374    /* cNst */
#define IDH_schedule_text                    0x634E7378    /* cNsx */
#define IDH_activate_search                  0x634E6973    /* cNis */
#define IDH_deactivate_search                0x634E7273    /* cNrs */
#define IDH_set_search                       0x634E7373    /* cNss */
#define IDH_listen                           0x634E6C73    /* cNls */
#define IDH_cancel_listen                    0x634E636E    /* cNcn */
#define IDH_hex_to_text                      0x634E6874    /* cNht */
#define IDH_set_configuration                0x634E7363    /* cNsc */
#define IDH_get_configuration                0x634E6763    /* cNgc */
#define IDH_text_to_hex                      0x634E7468    /* cNth */
#define IDH_value_to_char                    0x634E7663    /* cNvc */
#define IDH_char_to_value                    0x634E6376    /* cNcv */
#define IDH_check_sum                        0x634E6373    /* cNcs */



/*  Configuration File: CURSOR.CFG
 *  Function Category:  Cursor
 */

/* Objects */
#define IDH_cursor                           0x41637572    /* Acur */
/* Functions */
#define IDH_start_cursor                     0x73546372    /* sTcr */
#define IDH_stop_cursor                      0x73506372    /* sPcr */
#define IDH_show_cursor                      0x73486372    /* sHcr */
#define IDH_hide_cursor                      0x68446372    /* hDcr */
#define IDH_arrow_cursor                     0x61527277    /* aRrw */
#define IDH_hour_cursor                      0x77416368    /* wAch */
#define IDH_hand_cursor                      0x68416E64    /* hAnd */


/*  Configuration File: DATABASE.CFG
 *  Function Category:  Database
 */

/* Objects */
#define IDH_database                         0x44617462    /* Datb */
/* Functions */
#define IDH_add_record                       0x64426164    /* dBad */
#define IDH_append_found_ids                 0x64426166    /* dBaf */
#define IDH_close_database                   0x6442636C    /* dBcl */
#define IDH_copy_found_ids                   0x64426366    /* dBcf */
#define IDH_count_records                    0x6442636E    /* dBcn */
#define IDH_db_file_name                     0x6442666E    /* dBfn */
#define IDH_delete_record                    0x6442646C    /* dBdl */
#define IDH_find_exact_record                0x64426665    /* dBfe */
#define IDH_find_record                      0x64426672    /* dBfr */
#define IDH_find_record_id                   0x64426669    /* dBfi */
#define IDH_get_found_record                 0x64426766    /* dBgf */
#define IDH_get_record                       0x64426772    /* dBgr */
#define IDH_load_database                    0x64426C64    /* dBld */
#define IDH_macro_search                     0x64426D73    /* dBms */
#define IDH_new_database                     0x64426E64    /* dBnd */
#define IDH_open_database                    0x64426F66    /* dBof */
#define IDH_power_search                     0x64427073    /* dBps */
#define IDH_read_found_record                0x64427266    /* dBrf */
#define IDH_read_record                      0x64427272    /* dBrr */
#define IDH_record_index                     0x64427269    /* dBri */
#define IDH_recover_database                 0x64426678    /* dBfx */
#define IDH_remove_found_id                  0x6442726D    /* dBrm */
#define IDH_set_found_ids                    0x64427366    /* dBsf */
#define IDH_sort_found                       0x64427372    /* dBsr */
#define IDH_start_exclusive                  0x64427378    /* dBsx */
#define IDH_stop_exclusive                   0x64427478    /* dBtx */
#define IDH_unlock_record                    0x6442756C    /* dBul */
#define IDH_update_record                    0x64427572    /* dBur */
#define IDH_get_next_record                  0x6442676E    /* dBgn */
#define IDH_get_previous_record              0x64426770    /* dBgp */
#define IDH_read_next_record                 0x6442726E    /* dBrn */
#define IDH_read_previous_record             0x64427270    /* dBrp */
#define IDH_lock_record                      0x64426C72    /* dBlr */
#define IDH_set_all_mode                     0x6442616D    /* dBam */
#define IDH_set_found_mode                   0x6442666D    /* dBfm */


/*  Configuration File: DATE.CFG
 *  Function Category:  Date
 */

/* Objects */
#define IDH_date                             0x44617465    /* Date */
/* Functions */
#define IDH_change_date                      0x694E6474    /* iNdt */
#define IDH_compare_dates                    0x634D6474    /* cMdt */
#define IDH_current_date                     0x63526474    /* cRdt */
#define IDH_date_difference                  0x64546466    /* dTdf */
#define IDH_date_font_size                   0x64546673    /* dTfs */
#define IDH_parse_date                       0x64546272    /* dTbr */


/*  Configuration File: DDECLIEN.CFG
 *  Function Category:  DDE Client
 */

/* Objects */
#define IDH_dde_client                       0x44646563    /* Ddec */
/* Functions */
#define IDH_start_conversation               0x64637343    /* dcsC */
#define IDH_stop_conversation                0x64637843    /* dcxC */
#define IDH_start_transaction                0x64637354    /* dcsT */
#define IDH_stop_transaction                 0x64637854    /* dcxT */
#define IDH_stop_all_conversations           0x64636143    /* dcaC */
#define IDH_start_server                     0x64637353    /* dcsS */
#define IDH_get_data                         0x64636744    /* dcgD */
#define IDH_service_available                0x64637341    /* dcsA */
#define IDH_send_data                        0x64637344    /* dcsD */
#define IDH_suspend_conversation             0x64636243    /* dcbC */
#define IDH_resume_conversation              0x64637243    /* dcrC */
#define IDH_suspend_all_conversations        0x64636241    /* dcbA */
#define IDH_resume_all_conversations         0x64637241    /* dcrA */
#define IDH_execute_on_server                0x64636558    /* dceX */
#define IDH_stop_server                      0x64637853    /* dcxS */
#define IDH_set_transaction_options          0x64647354    /* ddsT */
#define IDH_get_conversation_info            0x64646743    /* ddgC */
#define IDH_last_new_transaction             0x64636754    /* dcgT */
#define IDH_get_aborted_transaction          0x64636758    /* dcgX */
#define IDH_last_updated_transaction         0x64636C54    /* dclT */
#define IDH_get_transaction_parameters       0x64636954    /* dciT */



/*  Configuration File: DDESERVE.CFG
 *  Function Category:  DDE Server
 */

/* Objects */
#define IDH_dde_server                       0x44646573    /* Ddes */
/* Functions */
#define IDH_copy_link                        0x63446465    /* cDde */


/*  Configuration File: DLL.CFG
 *  Function Category:  DLL
 */

/* Objects */
#define IDH_dll                              0x646C6C73    /* dlls */
/* Functions */
#define IDH_n1_call_dll                         0x63446C6C    /* cDll */



/*  Configuration File: FILE.CFG
 *  Function Category:  File
 */

/* Objects */
#define IDH_file                             0x46696C65    /* File */
/* Functions */
#define IDH_file_name                        0x66466E6D    /* fFnm */
#define IDH_load_file                        0x664C6F64    /* fLod */
#define IDH_new_file                         0x664E6577    /* fNew */
#define IDH_save_file                        0x66536176    /* fSav */
#define IDH_save_file_as                     0x66537673    /* fSvs */
#define IDH_open_file                        0x664F706E    /* fOpn */



/*  Configuration File: FILEOPS.CFG
 *  Function Category:  File Operations
 */
#define  IDH_file_operations		1027

/* Objects */
/* Functions */
#define IDH_choose_file                      0x664F6366    /* fOcf */
#define IDH_copy_file                        0x664F6370    /* fOcp */
#define IDH_delete_file                      0x664F646C    /* fOdl */
#define IDH_directory                        0x664F6472    /* fOdr */
#define IDH_disk_drive_letter                0x664F646E    /* fOdn */
#define IDH_create_directory                 0x664F6E66    /* fOnf */
#define IDH_path_name                        0x664F7074    /* fOpt */
#define IDH_get_files                        0x664F6766    /* fOgf */
#define IDH_move_file                        0x664F6D76    /* fOmv */
#define IDH_get_file_names                   0x664F676E    /* fOgn */
#define IDH_put_file                         0x664F7066    /* fOpf */
#define IDH_rename_file                      0x664F726E    /* fOrn */
#define IDH_write___ini                      0x77496E69    /* wIni */
#define IDH_read___ini                       0x72496E69    /* rIni */
#define IDH_get_creation_info                0x664F6763    /* fOgc */
#define IDH_get_modification_info            0x664F676D    /* fOgm */
#define IDH_get_size                         0x664F677A    /* fOgz */
#define IDH_get_types                        0x664F6774    /* fOgt */


/*  Configuration File: FILETRN.CFG
 *  Function Category:  File Transfer
 */

/* Objects */
#define IDH_file_transfer                    0x4674726E    /* Ftrn */
/* Functions */
#define IDH_n1_configure                     0x66546667    /* fTfg */
#define IDH_send_file                        0x66547366    /* fTsf */
#define IDH_receive_file                     0x66547266    /* fTrf */
#define IDH_cancel                           0x6654636E    /* fTcn */
#define IDH_n1_get_configuration             0x66546763    /* fTgc */
#define IDH_n1_set_configuration             0x66547363    /* fTsc */



/*  Configuration File: FLAG.CFG
 *  Function Category:  Flag
 */

/* Objects */
#define IDH_flag                             0x466C6167    /* Flag */
/* Functions */
#define IDH_flag_set                         0x6B466C67    /* kFlg */
#define IDH_toggle_flag                      0x74466C67    /* tFlg */
#define IDH_set_flag                         0x73466C67    /* sFlg */
#define IDH_clear_flag                       0x63466C67    /* cFlg */


/*  Configuration File: GENERAL.CFG
 *  Function Category:  General
 */
#define  IDH_general		1030

/* Objects */
/* Functions */
#define IDH_assign                           0x6153696E    /* aSin */
#define IDH_beep                             0x62456570    /* bEep */
#define IDH_copy                             0x634F7079    /* cOpy */
#define IDH_cut                              0x63557474    /* cUtt */
#define IDH_quit                             0x71556974    /* qUit */
#define IDH_paste                            0x70417374    /* pAst */
#define IDH_delay                            0x644C6179    /* dLay */
#define IDH_free_memory                      0x6652656D    /* fRem */
#define IDH_color_on                         0x634C726E    /* cLrn */
#define IDH_same_object                      0x734D6F62    /* sMob */
#define IDH_same_type                        0x734D6F74    /* sMot */
#define IDH_undo                             0x754E646F    /* uNdo */
#define IDH_yes_no_cancel                    0x79536E63    /* ySnc */
#define IDH_get_ticks                        0x67546B73    /* gTks */
#define IDH_screen_size                      0x67526573    /* gRes */


/*  Configuration File: GROUP.CFG
 *  Function Category:  Group
 */

/* Objects */
#define IDH_group                            0x47727570    /* Grup */
/* Functions */
#define IDH_count_objects                    0x6752636E    /* gRcn */
#define IDH_find_object                      0x6752666E    /* gRfn */
#define IDH_get_object                       0x67526774    /* gRgt */
#define IDH_insert_object                    0x6752696E    /* gRin */
#define IDH_remove_object                    0x6752726D    /* gRrm */


/*  Configuration File: IMP_EXP.CFG
 *  Function Category:  Import-Export
 */

/* Objects */
#define IDH_import_export                    0x496D6578    /* Imex */
/* Functions */
#define IDH_choose_i_e_file                  0x69456366    /* iEcf */
#define IDH_configure_export                 0x69456365    /* iEce */
#define IDH_configure_import                 0x69456369    /* iEci */
#define IDH_create_export_file               0x69456372    /* iEcr */
#define IDH_export_record                    0x69457872    /* iExr */
#define IDH_import_record                    0x69456972    /* iEir */
#define IDH_start_import_export              0x69457374    /* iEst */
#define IDH_stop_import_export               0x69457370    /* iEsp */


/*  Configuration File: KEYBOARD.CFG
 *  Function Category:  Keyboard
 */

/* Objects */
#define IDH_keyboard                         0x4B657962    /* Keyb */
/* Functions */
#define IDH_shift_key                        0x6B597368    /* kYsh */
#define IDH_alt_key                          0x6B596F70    /* kYop */
#define IDH_control_key                      0x6B59636E    /* kYcn */
#define IDH_keyboard_on                      0x6B596F6E    /* kYon */
#define IDH_keyboard_off                     0x6B596F66    /* kYof */
#define IDH_command_key                      0x6B59636D    /* kYcm */


/*  Configuration File: LAUNCHER.CFG
 *  Function Category:  Launch
 */

/* Objects */
#define IDH_launch                           0x4170706C    /* Appl */
/* Functions */
#define IDH_launch_application               0x6C417070    /* lApp */
#define IDH_set_application                  0x73417070    /* sApp */
#define IDH_launch_help                      0x6C486C70    /* lHlp */
#define IDH_close_help                       0x63486C70    /* cHlp */


/*  Configuration File: LIST.CFG
 *  Function Category:  List
 */

/* Objects */
#define IDH_list                             0x4C697374    /* List */
/* Functions */
#define IDH_clear_selections                 0x6C436C72    /* lClr */
#define IDH_count_elements                   0x6C436E74    /* lCnt */
#define IDH_delete_all                       0x6C44616C    /* lDal */                    
#define IDH_delete_element                   0x6C44656C    /* lDel */
#define IDH_get_element                      0x6C476574    /* lGet */
#define IDH_new_element                      0x6C4E6577    /* lNew */
#define IDH_set_element                      0x6C536574    /* lSet */
#define IDH_set_selection                 0x6C53656C    /* lSel */
#define IDH_sort_list                        0x6C537274    /* lSrt */
#define IDH_get_selection                    0x6C47736C    /* lGsl */


/*  Configuration File: LOOP.CFG
 *  Function Category:  Loop
 */

/* Objects */
#define IDH_loop                             0x4C6F6F70    /* Loop */
/* Functions */
#define IDH_abort_loop                       0x6C416274    /* lAbt */
#define IDH_start_loop                       0x6C537474    /* lStt */


/*  Configuration File: MENU.CFG
 *  Function Category:  Menu
 */

/* Objects */
#define IDH_menu                             0x4D656E75    /* Menu */
/* Functions */
#define IDH_count_menu_items                 0x6D436E74    /* mCnt */
#define IDH_delete_menu_item                 0x6D44656C    /* mDel */
#define IDH_disable_menu                     0x6D447362    /* mDsb */
#define IDH_enable_menu                      0x6D456E62    /* mEnb */
#define IDH_get_item_title                   0x6D476974    /* mGit */
#define IDH_get_popup_item                   0x6D477070    /* mGpp */
#define IDH_insert_menu_item                 0x6D496E73    /* mIns */
#define IDH_set_item_title                   0x6D52746C    /* mRtl */
#define IDH_set_popup_item                   0x6D537070    /* mSpp */


/*  Configuration File: MENUBAR.CFG
 *  Function Category:  Menu Bar
 */

/* Objects */
#define IDH_menu_bar                         0x4D626172    /* Mbar */
/* Functions */
#define IDH_enable_menu_bar                  0x6D42656E    /* mBen */
#define IDH_disable_menu_bar                 0x6D426473    /* mBds */
#define IDH_show_menu_bar                    0x6D427368    /* mBsh */
#define IDH_hide_menu_bar                    0x6D426864    /* mBhd */
#define IDH_install_menu_bar                 0x6D42696E    /* mBin */


/*  Configuration File: MENUITEM.CFG
 *  Function Category:  Menu Item
 */

/* Objects */
#define IDH_menu_item                        0x4D69746D    /* Mitm */
/* Functions */
#define IDH_disable_menu_item                0x6D496473    /* mIds */
#define IDH_enable_menu_item                 0x6D49656E    /* mIen */
#define IDH_toggle_item_off                  0x6D497466    /* mItf */
#define IDH_toggle_item_on                   0x6D49746E    /* mItn */


/*  Configuration File: MOVCTRL.CFG
 *  Function Category:  Movie Controller
 */

/* Objects */
#define IDH_movie_controller                 0x4D43746C    /* MCtl */
/* Functions */
#define IDH_allow_selecting                  0x4D437475    /* MCtu */
#define IDH_disallow_selecting               0x4D437476    /* MCtv */
#define IDH_hide_speaker                     0x4D437472    /* MCtr */
#define IDH_hide_steppers                    0x4D437474    /* MCtt */
#define IDH_selecting_enabled                0x4D43747B    /* MCt{ */
#define IDH_show_speaker                     0x4D437471    /* MCtq */
#define IDH_show_steppers                    0x4D437473    /* MCts */
#define IDH_speaker_visible                  0x4D437479    /* MCty */
#define IDH_steppers_visible                 0x4D43747A    /* MCtz */
#define IDH_clear_movie                      0x4D43746E    /* MCtn */
#define IDH_get_movie_s_controller           0x4D437470    /* MCtp */
#define IDH_assign_movie                     0x4D43746D    /* MCtm */


/*  Configuration File: MOVDSP.CFG
 *  Function Category:  Movie Display
 */

/* Objects */
/* Functions */
#define IDH_activation_border                0x4D6F6FF5    /* Mooõ */
#define IDH_badge                            0x4D6F6FF6    /* Mooö */
#define IDH_border                           0x4D6F6FF4    /* Mooô */
#define IDH_get_display_mode                 0x4D6F6FF3    /* Mooó */
#define IDH_get_volume                       0x4D6F6F71    /* Mooq */
#define IDH_hide_activation_border           0x4D6F6F2C    /* Moo, */
#define IDH_hide_badge                       0x4D6F6F2B    /* Moo+ */
#define IDH_hide_logo                        0x4D6F6F2E    /* Moo. */
#define IDH_logo                             0x4D6F6FF9    /* Mooù */
#define IDH_set_display_mode                 0x4D6F6FF2    /* Mooò */
#define IDH_set_volume                       0x4D6F6F72    /* Moor */
#define IDH_show_act__border                 0x4D6F6F20    /* Moo  */
#define IDH_show_badge                       0x4D6F6FFC    /* Mooü */
#define IDH_show_logo                        0x4D6F6F22    /* Moo" */
#define IDH_hide_border                      0x4D6F6F2A    /* Moo* */
#define IDH_show_border                      0x4D6F6FFB    /* Mooû */
#define IDH_get_scroll                       0x4D6F6F7B    /* Moo{ */
#define IDH_set_scroll                       0x4D6F6F7C    /* Moo| */
#define IDH_get_scale                        0x4D6F6F79    /* Mooy */
#define IDH_set_scale                        0x4D6F6F7A    /* Mooz */


/*  Configuration File: MOVEDIT.CFG
 *  Function Category:  Movie Edit
 */

/* Objects */
/* Functions */
#define IDH_clear_selection                  0x4D6F6FEE    /* Mooî */
#define IDH_get_sel_duration                 0x4D6F6FD1    /* MooÑ */
#define IDH_get_sel_time                     0x4D6F6FC7    /* MooÇ */
#define IDH_scale_selection                  0x4D6F6FEF    /* Mooï */
#define IDH_set_sel_duration                 0x4D6F6FD6    /* MooÖ */
#define IDH_set_sel_time                     0x4D6F6FC9    /* MooÉ */


/*  Configuration File: MOVIE.CFG
 *  Function Category:  Movie
 */

/* Objects */
#define IDH_movie                            0x4D6F6F56   /* MooV */
/* Functions */
#define IDH_load_movie                       0x4D6F6F24    /* Moo$ */
#define IDH_movie_loaded                     0x4D6F6F63    /* Mooc */
#define IDH_movie_playing                    0x4D6F6F5A    /* MooZ */
#define IDH_play_movie                       0x4D6F6F57    /* MooW */
#define IDH_preroll_movie                    0x4D6F6F61    /* Mooa */
#define IDH_quicktime_present                0x4D6F6F62    /* Moob */
#define IDH_stop_movie                       0x4D6F6F59    /* MooY */
#define IDH_update_movie                     0x4D6F6F58    /* MooX */
#define IDH_show_movie_poster                0x4D6F6FE2    /* Mooâ */
#define IDH_movie_done                       0x4D6F6F5B    /* Moo[ */


/*  Configuration File: MOVIEINF.CFG
 *  Function Category:  Movie Info
 */

/* Objects */
/* Functions */
#define IDH_get_dates                        0x4D6F6FE1    /* Mooá */
#define IDH_get_duration                     0x4D6F6F6B    /* Mook */
#define IDH_get_preferred_rate               0x4D6F6F66    /* Moof */
#define IDH_get_preferred_volume             0x4D6F6F73    /* Moos */
#define IDH_sound_track                      0x4D6F6F74    /* Moot */
#define IDH_video_track                      0x69566964    /* iVid */
#define IDH_get_max_scroll                   0x4D6F6F7D    /* Moo} */
#define IDH_get_total_frames                 0x4D6F6F6E    /* Moon */


/*  Configuration File: MOVIMPEX.CFG
 *  Function Category:  Movie Import/Export
 */

/* Objects */
/* Functions */
#define IDH_poster_to_picture                0x4D6F6FE5    /* Mooå */
#define IDH_movie_to_picture                 0x4D6F6F27    /* Moo' */


/*  Configuration File: MOVMETR.CFG
 *  Function Category:  Movie Metrics
 */

/* Objects */
/* Functions */
#define IDH_get_loop_value                   0x4D6F6F67    /* Moog */
#define IDH_set_loop_value                   0x4D6F6F68    /* Mooh */
#define IDH_get_rate                         0x4D6F6F64    /* Mood */
#define IDH_get_time                         0x4D6F6F69    /* Mooi */
#define IDH_go_to_beginning                  0x4D6F6F5C    /* Moo\ */
#define IDH_go_to_end                        0x4D6F6F5D    /* Moo] */
#define IDH_go_to_time                       0x4D6F6F6A    /* Mooj */
#define IDH_get_poster_time                  0x4D6F6FE4    /* Mooä */
#define IDH_get_frame                        0x4D6F6F6C    /* Mool */
#define IDH_frame_to_time                    0x4D6F6F70    /* Moop */
#define IDH_go_to_frame                      0x4D6F6F6D    /* Moom */
#define IDH_time_to_frame                    0x4D6F6F6F    /* Mooo */


/*  Configuration File: NOTIFY.CFG
 *  Function Category:  Notification
 */

/* Objects */
#define IDH_notification                     0x4E746679    /* Ntfy */
/* Functions */
#define IDH_alert_notify                     0x6E416C74    /* nAlt */
#define IDH_start_notify                     0x6E537474    /* nStt */
#define IDH_stop_notify                      0x6E537470    /* nStp */


/*  Configuration File: NUMBER.CFG
 *  Function Category:  Number
 */

/* Objects */
#define IDH_number                           0x4E6D6272    /* Nmbr */
/* Functions */
#define IDH_add                              0x6E416464    /* nAdd */
#define IDH_compare_numbers                  0x6E436D70    /* nCmp */
#define IDH_decrement_number                 0x6E446563    /* nDec */
#define IDH_divide                           0x6E446976    /* nDiv */
#define IDH_increment_number                 0x6E496E63    /* nInc */
#define IDH_multiply                         0x6E4D756C    /* nMul */
#define IDH_random_number                    0x6E526E64    /* nRnd */
#define IDH_randomize                        0x6E526E7A    /* nRnz */
#define IDH_subtract                         0x6E537562    /* nSub */
#define IDH_expression                       0x6E457870    /* nExp */
#define IDH_number_font_size                 0x6E46737A    /* nFsz */


/*  Configuration File: OLE1CLI.CFG
 *  Function Category:  OLE
 */

/* Objects */
/* #define IDH_ole_object                    0x4F6C6531 */
#define IDH_ole                              0x4F6C6531    /* Ole1 */
/* Functions */
#define IDH_check_object                     0x6F43686B    /* oChk */
#define IDH_create_object                    0x6F437274    /* oCrt */
#define IDH_delete_object                    0x6F44656C    /* oDel */
#define IDH_activate_object                  0x6F416374    /* oAct */


/*  Configuration File: ORACLE.CFG
 *  Function Category:  Oracle Connection
 */

/* Objects */
#define IDH_oracle_connection                0x6F52634C    /* oRcL */
/* Functions */
#define IDH_n1_connect                       0x6F72464E    /* orFN */
#define IDH_auto_commit_off                  0x61544F66    /* aTOf */
#define IDH_auto_commit_on                   0x61546346    /* aTcF */
#define IDH_n1_disconnect                    0x6465464E    /* deFN */
#define IDH_connections_status               0x6C477346    /* lGsF */
#define IDH_oracle_error                     0x6F724552    /* orER */
#define IDH_set_connection                   0x73744F52    /* stOR */
#define IDH_set_buffer_size                  0x73477346    /* sGsF */
#define IDH_n1_commit                        0x634F6D46    /* cOmF */
#define IDH_n1_rollback                      0x724F6C46    /* rOlF */


/*  Configuration File: SQL.CFG
 *  Function Category:  Oracle SQL
 */

/* Objects */
#define IDH_oracle_sql                       0x73516C54    /* sQlT */
/* Functions */
#define IDH_close_cursor                     0x636C4352    /* clCR */
#define IDH_execute_sql                      0x7873464E    /* xsFN */
#define IDH_get_next                         0x67744E58    /* gtNX */
#define IDH_open_cursor                      0x6F704352    /* opCR */
#define IDH_column_names                     0x6F436C4C    /* oClL */
#define IDH_table_names                      0x4F54624C    /* OTbL */


/*  Configuration File: PAGE.CFG
 *  Function Category:  Page Layout
 */

/* Objects */
#define IDH_page_layout                      0x50616765    /* Page */
/* Functions */
#define IDH_clear_layout                     0x7047636C    /* pGcl */
#define IDH_count_pages                      0x7047636E    /* pGcn */
#define IDH_next_window                      0x70476E77    /* pGnw */
#define IDH_spool_window                     0x70477377    /* pGsw */


/*  Configuration File: PICTURE.CFG
 *  Function Category:  Picture
 */

/* Objects */
#define IDH_picture                          0x50696374    /* Pict */
/* Functions */
#define IDH_load_picture                     0x6C506374    /* lPct */
#define IDH_save_picture                     0x73506374    /* sPct */
#define IDH_picture_empty                    0x65506374    /* ePct */


/*  Configuration File: POLY.CFG
 *  Function Category:  PolyButton
 */

/* Objects */
#define IDH_polybutton                       0x506F6C79    /* Poly */
/* Functions */
#define IDH_compress_polybutton              0x63506C79    /* cPly */
#define IDH_count_regions                    0x6354706C    /* cTpl */
#define IDH_edit_polybutton                  0x65506C79    /* ePly */
#define IDH_get_click                        0x67436C6B    /* gClk */
#define IDH_get_index                        0x67496478    /* gIdx */
#define IDH_get_name                         0x674E616D    /* gNam */
#define IDH_get_pictures                     0x67506963    /* gPic */
#define IDH_hilite_region                    0x684C6274    /* hLbt */
#define IDH_hilited                          0x684C743F    /* hLt? */
#define IDH_set_name                         0x734E616D    /* sNam */
#define IDH_set_pictures                     0x73506963    /* sPic */
#define IDH_unhilite_region                  0x754E6862    /* uNhb */


/*  Configuration File: PRINTER.CFG
 *  Function Category:  Printer
 */

/* Objects */
#define IDH_printer                          0x50726E74    /* Prnt */
/* Functions */
#define IDH_print                            0x70526F62    /* pRob */
#define IDH_print_now                        0x70526F6E    /* pRon */
#define IDH_print_setup                      0x70527067    /* pRpg */
#define IDH_print_dialog                     0x70526A62    /* pRjb */


/*  Configuration File: QPLSECNC.CFG
 *  Function Category:  ODBC Connection
 */

/* Objects */
#define IDH_odbc_connection                  0x51636E63    /* Qcnc */
/* Functions */
#define IDH_connect                          0x7143636E    /* qCcn */
#define IDH_disconnect                       0x71436463    /* qCdc */
#define IDH_status                           0x71437374    /* qCst */
#define IDH_get_error                        0x71436765    /* qCge */
#define IDH_rollback                         0x71437262    /* qCrb */
#define IDH_commit                           0x7143636D    /* qCcm */


/*  Configuration File: QPLSESQL.CFG
 *  Function Category:  ODBC SQL
 */

/* Objects */
#define IDH_odbc_sql                         0x5173716C    /* Qsql */
/* Functions */
#define IDH_n1_get_error                        0x71536765    /* qSge */
#define IDH_fetch                            0x71536665    /* qSfe */
#define IDH_fetch_to_table                   0x71537462    /* qStb */
#define IDH_fetch_to_text                    0x71537478    /* qStx */
#define IDH_flush                            0x7153666C    /* qSfl */
#define IDH_execute                          0x71536578    /* qSex */
#define IDH_get_variables                    0x71536776    /* qSgv */
#define IDH_describe_table                   0x71536474    /* qSdt */
#define IDH_get_tables                       0x71536774    /* qSgt */
#define IDH_append_to_table                  0x71536174    /* qSat */


/*  Configuration File: RADIOGRP.CFG
 *  Function Category:  Radio Group
 */

/* Objects */
#define IDH_radio_group                      0x52646770    /* Rdgp */
/* Functions */


/*  Configuration File: SCALE.CFG
 *  Function Category:  Scale
 */

/* Objects */
#define IDH_scale                            0x5363616C    /* Scal */
/* Functions */


/*  Configuration File: SCROLBAR.CFG
 *  Function Category:  Scroll Bar
 */

/* Objects */
#define IDH_scroll_bar                       0x53626172    /* Sbar */
/* Functions */
#define IDH_set_range                        0x73427372    /* sBsr */
#define IDH_get_range                        0x73426772    /* sBgr */
#define IDH_set_position                     0x73427370    /* sBsp */
#define IDH_get_position                     0x73426770    /* sBgp */
#define IDH_enable                           0x73426561    /* sBea */
#define IDH_disable                          0x73426461    /* sBda */
#define IDH_get_increment                    0x73426769    /* sBgi */
#define IDH_set_increment                    0x73427369    /* sBsi */
#define IDH_enabled                          0x73426965    /* sBie */


/*  Configuration File: SIGNAL.CFG
 *  Function Category:  Signal
 */

/* Objects */
#define IDH_signal                           0x63615365    /* caSe */
/* Functions */
#define IDH_generate_signals                 0x63537769    /* cSwi */



/*  Configuration File: SOUND.CFG
 *  Function Category:  Sound
 */

/* Objects */
#define IDH_sound                            0x536F6E64    /* Sond */
/* Functions */
#define IDH_play                             0x734E706C    /* sNpl */
#define IDH_play_from_file                   0x734E7066    /* sNpf */
#define IDH_stop_sounds                      0x734E7374    /* sNst */
#define IDH_load_sound_resource              0x734E6C64    /* sNld */
#define IDH_record_to_file                   0x734E7266    /* sNrf */
#define IDH_record                           0x734E7263    /* sNrc */
#define IDH_sound_empty                      0x734E6579    /* sNey */
#define IDH_device_present                   0x734E6944    /* sNiD */
#define IDH_play_repeatedly                  0x734E7072    /* sNpr */
#define IDH_wait_until_done                  0x73577464    /* sWtd */


/*  Configuration File: SUBROUTN.CFG
 *  Function Category:  Subroutine
 */

/* Objects */
#define IDH_subroutine                       0x53756272    /* Subr */
/* Functions */
#define IDH_call_subroutine                  0x63416C73    /* cAls */
#define IDH_fetch_parameters                 0x66744370    /* ftCp */
#define IDH_return                           0x72546673    /* rTfs */


/*  Configuration File: TABLE.CFG
 *  Function Category:  Table
 */

/* Objects */
#define IDH_table                            0x5461626C    /* Tabl */
/* Functions */
#define IDH_activate_column                  0x74426174    /* tBat */
#define IDH_active_cell                      0x7442636C    /* tBcl */
#define IDH_active_column                    0x74426163    /* tBac */
#define IDH_add_row                          0x74426172    /* tBar */
#define IDH_cell_edited                      0x74426563    /* tBec */
#define IDH_n1_clear_selections              0x74426373    /* tBcs */
#define IDH_column_calculation               0x74426370    /* tBcp */
#define IDH_count_rows                       0x74426372    /* tBcr */
#define IDH_delete_row                       0x74426472    /* tBdr */
#define IDH_get_cell                         0x74426763    /* tBgc */
#define IDH_get_row                          0x74426772    /* tBgr */
#define IDH_n1_get_selection                 0x74426773    /* tBgs */
#define IDH_row_calculation                  0x74427270    /* tBrp */
#define IDH_scroll_to_selection              0x74427363    /* tBsc */
#define IDH_set_cell                         0x74426E63    /* tBnc */
#define IDH_set_row                          0x74427365    /* tBse */
#define IDH_n1_set_selection                 0x74427373    /* tBss */
#define IDH_sort_row_ascending               0x74427372    /* tBsr */
#define IDH_sort_ascending                   0x74427374    /* tBst */
#define IDH_update_auto_table                0x74427561    /* tBua */
#define IDH_set_column                       0x7442736C    /* tBsl */
#define IDH_get_column                       0x7442676C    /* tBgl */
#define IDH_get_column_width                 0x74426777    /* tBgw */
#define IDH_set_column_width                 0x74427377    /* tBsw */
#define IDH_clear_table                      0x74426374    /* tBct */
#define IDH_get_rows                         0x74427274    /* tBrt */
#define IDH_delete_column                    0x74426463    /* tBdc */
#define IDH_find                             0x74426664    /* tBfd */
#define IDH_find_exact                       0x74426665    /* tBfe */
#define IDH_get_column_title                 0x7442676E    /* tBgn */
#define IDH_get_data_source                  0x74426764    /* tBgd */
#define IDH_get_indexed_field                0x74426769    /* tBgi */
#define IDH_insert_column                    0x74426963    /* tBic */
#define IDH_set_column_title                 0x7442736E    /* tBsn */
#define IDH_set_data_source                  0x74427364    /* tBsd */
#define IDH_set_indexed_field                0x74427369    /* tBsi */
#define IDH_get_record_id                    0x74427269    /* tBri */
#define IDH_refresh_auto_table               0x74427261    /* tBra */
#define IDH_sort_descending                  0x74426473    /* tBds */
#define IDH_sort_row_descending              0x74427264    /* tBrd */


/*  Configuration File: TERMINAL.CFG
 *  Function Category:  Terminal
 */

/* Objects */
#define IDH_terminal                         0x5465726D    /* Term */
/* Functions */
#define IDH_n2_configure                     0x744D6667    /* tMfg */
#define IDH_clear                            0x744D636C    /* tMcl */
#define IDH_reset                            0x744D7273    /* tMrs */
#define IDH_receive_text                     0x744D7274    /* tMrt */
#define IDH_capture_text                     0x744D6374    /* tMct */
#define IDH_end_capture                      0x744D6563    /* tMec */
#define IDH_n2_get_configuration             0x744D6763    /* tMgc */
#define IDH_n2_set_configuration             0x744D7363    /* tMsc */
#define IDH_get_terminal_text                0x744D6774    /* tMgt */




/*  Configuration File: TEXT.CFG
 *  Function Category:  Text
 */

/* Objects */
#define IDH_text                             0x54657874    /* Text */
/* Functions */
#define IDH_text_font_size                   0x74586673    /* tXfs */
#define IDH_load_text_file                   0x74586C74    /* tXlt */
#define IDH_save_text_file                   0x74587374    /* tXst */
#define IDH_append_return                    0x74586172    /* tXar */
#define IDH_append_tab                       0x74586174    /* tXat */
#define IDH_clear_text                       0x7458636C    /* tXcl */
#define IDH_compare_text                     0x7458636D    /* tXcm */
#define IDH_concatenate                      0x7458636E    /* tXcn */
#define IDH_find_replace                     0x74586672    /* tXfr */
#define IDH_get_field                        0x74586766    /* tXgf */
#define IDH_get_line                         0x7458676C    /* tXgl */
#define IDH_get_number                       0x7458676E    /* tXgn */
#define IDH_get_word                         0x74586777    /* tXgw */
#define IDH_insert_text                      0x74586974    /* tXit */
#define IDH_left_text                        0x74586C73    /* tXls */
#define IDH_mid_text                         0x74586D73    /* tXms */
#define IDH_right_text                       0x74587273    /* tXrs */
#define IDH_text_length                      0x74586C6E    /* tXln */
#define IDH_upper_case                       0x74587563    /* tXuc */
#define IDH_omit_text                        0x74586F6D    /* tXom */
#define IDH_merge_text                       0x7458756E    /* tXun */
#define IDH_intersect_text                   0x74587874    /* tXxt */


/*  Configuration File: TEXTFACT.CFG
 *  Function Category:  Text Builder
 */

/* Objects */
#define IDH_text_builder                     0x54666374    /* Tfct */
/* Functions */
#define IDH_make_text                        0x7446636C    /* tFcl */
#define IDH_n1_set_expression                0x74467374    /* tFst */
#define IDH_n1_auto_update_on                0x74416F6E    /* tAon */
#define IDH_n1_auto_update_off               0x74416F66    /* tAof */


/*  Configuration File: TIME.CFG
 *  Function Category:  Time
 */

/* Objects */
#define IDH_time                             0x54696D65    /* Time */
/* Functions */
#define IDH_change_time                      0x7449696E    /* tIin */
#define IDH_compare_time_hm                  0x7449636D    /* tIcm */
#define IDH_compare_time_hms                 0x74496373    /* tIcs */
#define IDH_current_time                     0x74496372    /* tIcr */
#define IDH_time_difference                  0x74496466    /* tIdf */
#define IDH_time_font_size                   0x74496673    /* tIfs */


/*  Configuration File: WINDOW.CFG
 *  Function Category:  Window
 */

/* Objects */
#define IDH_window                           0x57696E6F    /* Wino */
/* Functions */
#define IDH_activate_item                    0x69416374    /* iAct */
#define IDH_active_item                      0x61437469    /* aCti */
#define IDH_cancel_close                     0x63436C73    /* cCls */
#define IDH_center_window                    0x634E7477    /* cNtw */
#define IDH_close_window                     0x634C7377    /* cLsw */
#define IDH_front_window                     0x66527477    /* fRtw */
#define IDH_n1_get_title                     0x6757746C    /* gWtl */
#define IDH_hide_item                        0x6849746D    /* hItm */
#define IDH_window_open                      0x6953776F    /* iSwo */
#define IDH_move_window                      0x6D4F7677    /* mOvw */
#define IDH_open_window                      0x6F506E77    /* oPnw */
#define IDH_n1_set_title                     0x7357746C    /* sWtl */
#define IDH_show_item                        0x7349746D    /* sItm */
#define IDH_size_window                      0x73497A77    /* sIzw */
#define IDH_mouse_location                   0x6D536C63    /* mSlc */
#define IDH_item_location                    0x69546D6C    /* iTml */
#define IDH_move_item                        0x6D566974    /* mVit */
#define IDH_get_item_selection               0x67576973    /* gWis */
#define IDH_set_item_selection               0x73576973    /* sWis */
#define IDH_window_position                  0x67577073    /* gWps */

