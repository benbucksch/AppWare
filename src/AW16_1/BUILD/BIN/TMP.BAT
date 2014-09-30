REM   Batch file to create the N:\SHIP directory contents from the
REM   N:\INTERNAL directories

n:
cd \ship

REM Library files
copy \internal\lib\ship\almkrnl.lib  lib
copy \internal\lib\ship\alm_butn.lib lib
copy \internal\lib\ship\alm_date.lib lib
copy \internal\lib\ship\alm_flag.lib lib
copy \internal\lib\ship\alm_mhs.lib  lib
copy \internal\lib\ship\alm_msg.lib  lib
copy \internal\lib\ship\alm_nmbr.lib lib
copy \internal\lib\ship\alm_pict.lib lib
copy \internal\lib\ship\alm_text.lib lib
copy \internal\lib\ship\alm_time.lib lib
copy \internal\lib\ship\alm_wnd.lib  lib

