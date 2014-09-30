REM   Batch file to create the N:\SHIP directory contents from the
REM   N:\INTERNAL directories

n:
cd \ship

REM Delete everything
attrib -r *.* /s
echo y | del bin\debug
echo y | del bin\nodebug
echo y | del bin
echo y | del config
echo y | del include
echo y | del lib

md bin
md bin\debug
md bin\nodebug
md config
md include
md lib

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

REM Include files
copy \internal\include\ship\a_alm.h    include
copy \internal\include\ship\a_almutl.h include
copy \internal\include\ship\ap_macro.h include
copy \internal\include\ship\o_button.h include
copy \internal\include\ship\o_date.h   include
copy \internal\include\ship\o_flag.h   include
copy \internal\include\ship\o_nwmhs.h  include 
copy \internal\include\ship\o_nwmsg.h  include
copy \internal\include\ship\o_number.h include
copy \internal\include\ship\o_pict.h   include
copy \internal\include\ship\o_text.h   include
copy \internal\include\ship\o_time.h   include
copy \internal\include\ship\o_wnd.h    include
copy \internal\include\ship\op_wnd.h   include

REM Configuration files
copy \objects\configs\applicat.cfg     config
copy \objects\configs\browser.cfg      config
copy \objects\configs\bundle.cfg       config
copy \objects\configs\button.cfg       config
copy \objects\configs\calculat.cfg     config
copy \objects\configs\commcnt.cfg      config
copy \objects\configs\cursor.cfg       config
copy \objects\configs\date.cfg         config
copy \objects\configs\database.cfg     config
copy \objects\configs\datastrm.cfg     config
copy \objects\configs\ddeclien.cfg     config
copy \objects\configs\ddeserve.cfg     config
copy \objects\configs\dll.cfg          config
copy \objects\configs\file.cfg         config
copy \objects\configs\fileops.cfg      config
copy \objects\configs\filetrn.cfg      config
copy \objects\configs\flag.cfg         config
copy \objects\configs\general.cfg      config
copy \objects\configs\group.cfg        config
copy \objects\configs\imp_exp.cfg      config
copy \objects\configs\keyboard.cfg     config
copy \objects\configs\launcher.cfg     config
copy \objects\configs\list.cfg         config
copy \objects\configs\loop.cfg         config
copy \objects\configs\menu.cfg         config
copy \objects\configs\menubar.cfg      config
copy \objects\configs\menuitem.cfg     config
copy \objects\configs\movctrl.cfg      config
copy \objects\configs\movdsp.cfg       config
copy \objects\configs\movedit.cfg      config
copy \objects\configs\movie.cfg        config
copy \objects\configs\movieinf.cfg     config
copy \objects\configs\movimpex.cfg     config
copy \objects\configs\movmetr.cfg      config
copy \objects\configs\notify.cfg       config
copy \objects\configs\nwsrvr.cfg       config
copy \objects\configs\number.cfg       config
copy \objects\configs\nwbind.cfg       config
copy \objects\configs\nwds.cfg         config
copy \objects\configs\nwer.cfg         config
copy \objects\configs\nwmhs.cfg        config
copy \objects\configs\nwmsg.cfg        config
copy \objects\configs\nwws.cfg         config
copy \objects\configs\nwauthen.cfg     config
copy \objects\configs\nwbr.cfg         config
copy \objects\configs\nwdsch.cfg       config
copy \objects\configs\nwfs.cfg         config
copy \objects\configs\nwncpext.cfg     config
copy \objects\configs\ole1cli.cfg      config
copy \objects\configs\page.cfg         config
copy \objects\configs\picture.cfg      config
copy \objects\configs\poly.cfg         config
copy \objects\configs\printer.cfg      config
copy \objects\configs\qplsecnc.cfg     config
copy \objects\configs\qplsesql.cfg     config
copy \objects\configs\radiogrp.cfg     config
copy \objects\configs\scale.cfg        config
copy \objects\configs\scrolbar.cfg     config
copy \objects\configs\signal.cfg       config
copy \objects\configs\sound.cfg        config
copy \objects\configs\subroutn.cfg     config
copy \objects\configs\table.cfg        config
copy \objects\configs\time.cfg         config
copy \objects\configs\terminal.cfg     config
copy \objects\configs\textfact.cfg     config
copy \objects\configs\text.cfg         config
copy \objects\configs\window.cfg       config

REM Executables and DLLs
copy \internal\bin\ship\almcfgup.dll   bin
copy \internal\bin\ship\almbldr.exe   bin
copy \internal\bin\ship\appware.exe    bin
copy \internal\bin\ship\almkrnl.dat    bin

REM Production object DLLs
copy \internal\bin\ship\almkrnl.dll    bin\nodebug
copy \internal\bin\ship\alm_acur.dll   bin\nodebug
copy \internal\bin\ship\alm_appl.dll   bin\nodebug
copy \internal\bin\ship\alm_auth.dll   bin\nodebug
copy \internal\bin\ship\alm_bind.dll   bin\nodebug
copy \internal\bin\ship\alm_bndl.dll   bin\nodebug
copy \internal\bin\ship\alm_brws.dll   bin\nodebug
copy \internal\bin\ship\alm_butn.dll   bin\nodebug
copy \internal\bin\ship\alm_calc.dll   bin\nodebug
copy \internal\bin\ship\alm_cnct.dll   bin\nodebug
copy \internal\bin\ship\alm_ctrl.dll   bin\nodebug
copy \internal\bin\ship\alm_datb.dll   bin\nodebug
copy \internal\bin\ship\alm_ddec.dll   bin\nodebug
copy \internal\bin\ship\alm_ddes.dll   bin\nodebug
copy \internal\bin\ship\alm_date.dll   bin\nodebug
copy \internal\bin\ship\alm_dll.dll    bin\nodebug
copy \internal\bin\ship\alm_file.dll   bin\nodebug
copy \internal\bin\ship\alm_fltr.dll   bin\nodebug
copy \internal\bin\ship\alm_flag.dll   bin\nodebug
copy \internal\bin\ship\alm_flop.dll   bin\nodebug
copy \internal\bin\ship\alm_gnrl.dll   bin\nodebug
copy \internal\bin\ship\alm_grup.dll   bin\nodebug
copy \internal\bin\ship\alm_imex.dll   bin\nodebug
copy \internal\bin\ship\alm_keyb.dll   bin\nodebug
copy \internal\bin\ship\alm_list.dll   bin\nodebug
copy \internal\bin\ship\alm_loop.dll   bin\nodebug
copy \internal\bin\ship\alm_menu.dll   bin\nodebug
copy \internal\bin\ship\alm_mhs.dll    bin\nodebug
copy \internal\bin\ship\alm_mov.dll    bin\nodebug
copy \internal\bin\ship\alm_msg.dll    bin\nodebug
copy \internal\bin\ship\alm_nmbr.dll   bin\nodebug
copy \internal\bin\ship\alm_ntfy.dll   bin\nodebug
copy \internal\bin\ship\alm_nwds.dll   bin\nodebug
copy \internal\bin\ship\alm_nwer.dll   bin\nodebug
copy \internal\bin\ship\alm_nwws.dll   bin\nodebug
copy \internal\bin\ship\alm_nwsv.dll   bin\nodebug
copy \internal\bin\ship\nwxlat.dll     bin\nodebug
copy \internal\bin\ship\alm_dsch.dll   bin\nodebug
copy \internal\bin\ship\alm_nwbr.dll   bin\nodebug
copy \internal\bin\ship\alm_nwfs.dll   bin\nodebug
copy \internal\bin\ship\alm_ncpx.dll   bin\nodebug
copy \internal\bin\ship\alm_ole1.dll   bin\nodebug
copy \internal\bin\ship\alm_page.dll   bin\nodebug
copy \internal\bin\ship\alm_pict.dll   bin\nodebug
copy \internal\bin\ship\alm_poly.dll   bin\nodebug
copy \internal\bin\ship\alm_prnt.dll   bin\nodebug
copy \internal\bin\ship\alm_qsql.dll   bin\nodebug
copy \internal\bin\ship\alm_radg.dll   bin\nodebug
copy \internal\bin\ship\resizer.dll    bin\nodebug
copy \internal\bin\ship\alm_sbar.dll   bin\nodebug
copy \internal\bin\ship\alm_scal.dll   bin\nodebug
copy \internal\bin\ship\alm_shap.dll   bin\nodebug
copy \internal\bin\ship\alm_sigs.dll   bin\nodebug
copy \internal\bin\ship\alm_soun.dll   bin\nodebug
copy \internal\bin\ship\alm_strm.dll   bin\nodebug
copy \internal\bin\ship\alm_subr.dll   bin\nodebug
copy \internal\bin\ship\alm_sysm.dll   bin\nodebug
copy \internal\bin\ship\alm_tabl.dll   bin\nodebug
copy \internal\bin\ship\alm_text.dll   bin\nodebug
copy \internal\bin\ship\alm_tfct.dll   bin\nodebug
copy \internal\bin\ship\alm_time.dll   bin\nodebug
copy \internal\bin\ship\alm_tmnl.dll   bin\nodebug
copy \internal\bin\ship\alm_wlay.dll   bin\nodebug
copy \internal\bin\ship\alm_wnd.dll    bin\nodebug


REM Development object DLLs
copy \internal\bin\debug\almkrnl.dll    bin\debug
copy \internal\bin\debug\alm_acur.dll   bin\debug
copy \internal\bin\debug\alm_appl.dll   bin\debug
copy \internal\bin\debug\alm_auth.dll   bin\debug
copy \internal\bin\debug\alm_bind.dll   bin\debug
copy \internal\bin\debug\alm_bndl.dll   bin\debug
copy \internal\bin\debug\alm_brws.dll   bin\debug
copy \internal\bin\debug\alm_butn.dll   bin\debug
copy \internal\bin\debug\alm_calc.dll   bin\debug
copy \internal\bin\debug\alm_cnct.dll   bin\debug
copy \internal\bin\debug\alm_ctrl.dll   bin\debug
copy \internal\bin\debug\alm_datb.dll   bin\debug
copy \internal\bin\debug\alm_date.dll   bin\debug
copy \internal\bin\debug\alm_ddec.dll   bin\debug
copy \internal\bin\debug\alm_ddes.dll   bin\debug
copy \internal\bin\debug\alm_dll.dll    bin\debug
copy \internal\bin\debug\alm_file.dll   bin\debug
copy \internal\bin\debug\alm_fltr.dll   bin\debug
copy \internal\bin\debug\alm_flag.dll   bin\debug
copy \internal\bin\debug\alm_flop.dll   bin\debug
copy \internal\bin\debug\alm_gnrl.dll   bin\debug
copy \internal\bin\debug\alm_grup.dll   bin\debug
copy \internal\bin\debug\alm_imex.dll   bin\debug
copy \internal\bin\debug\alm_keyb.dll   bin\debug
copy \internal\bin\debug\alm_list.dll   bin\debug
copy \internal\bin\debug\alm_loop.dll   bin\debug
copy \internal\bin\debug\alm_menu.dll   bin\debug
copy \internal\bin\debug\alm_mhs.dll    bin\debug
copy \internal\bin\debug\alm_mov.dll    bin\debug
copy \internal\bin\debug\alm_msg.dll    bin\debug
copy \internal\bin\debug\alm_nmbr.dll   bin\debug
copy \internal\bin\debug\alm_ntfy.dll   bin\debug
copy \internal\bin\debug\alm_nwds.dll   bin\debug
copy \internal\bin\debug\alm_nwer.dll   bin\debug
copy \internal\bin\debug\alm_nwws.dll   bin\debug
copy \internal\bin\debug\alm_nwsv.dll   bin\debug
copy \internal\bin\debug\nwxlat.dll     bin\debug
copy \internal\bin\debug\alm_dsch.dll   bin\debug
copy \internal\bin\debug\alm_nwbr.dll   bin\debug
copy \internal\bin\debug\alm_nwfs.dll   bin\debug
copy \internal\bin\debug\alm_ncpx.dll   bin\debug
copy \internal\bin\debug\alm_ole1.dll   bin\debug
copy \internal\bin\debug\alm_page.dll   bin\debug
copy \internal\bin\debug\alm_pict.dll   bin\debug
copy \internal\bin\debug\alm_poly.dll   bin\debug
copy \internal\bin\debug\alm_prnt.dll   bin\debug
copy \internal\bin\debug\alm_qsql.dll   bin\debug
copy \internal\bin\debug\alm_radg.dll   bin\debug
copy \internal\bin\debug\resizer.dll    bin\debug
copy \internal\bin\debug\alm_sbar.dll   bin\debug
copy \internal\bin\debug\alm_scal.dll   bin\debug
copy \internal\bin\debug\alm_shap.dll   bin\debug
copy \internal\bin\debug\alm_sigs.dll   bin\debug
copy \internal\bin\debug\alm_soun.dll   bin\debug
copy \internal\bin\debug\alm_strm.dll   bin\debug
copy \internal\bin\debug\alm_subr.dll   bin\debug
copy \internal\bin\debug\alm_sysm.dll   bin\debug
copy \internal\bin\debug\alm_tabl.dll   bin\debug
copy \internal\bin\debug\alm_text.dll   bin\debug
copy \internal\bin\debug\alm_tfct.dll   bin\debug
copy \internal\bin\debug\alm_time.dll   bin\debug
copy \internal\bin\debug\alm_tmnl.dll   bin\debug
copy \internal\bin\debug\alm_wlay.dll   bin\debug
copy \internal\bin\debug\alm_wnd.dll    bin\debug

REM 3rd party crap
CALL n:\build\bin\gettux.bat

REM help stuff
CALL n:\build\bin\gethelp.bat

CALL n:\build\bin\fixshibs.bat

REM Touch all files
CALL n:\build\bin\touchshp.bat
