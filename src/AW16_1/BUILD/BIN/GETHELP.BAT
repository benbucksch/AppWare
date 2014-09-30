cd n:\ship\bin
attrib -r *.hlp
copy n:\help\appbldr\appware.hlp .
ss get $\appware\objects\help\*.hlp
attrib +r *.hlp

cd n:\ship\help\alm
attrib -r alm_api.hlp
copy n:\help\alm_api\alm_api.hlp .
attrib +r alm_api.hlp            
ss get $\appware\help\alm_api.ico
ss get $\appware\help\debuginf.wri

cd n:\ship\help
ss get $\appware\help\docnotes.wri
ss get $\appware\help\knownprb.wri




