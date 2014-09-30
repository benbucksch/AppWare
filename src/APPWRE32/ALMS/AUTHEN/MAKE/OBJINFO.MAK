# Object-specific make information:
#
#  DLL name
#  .DEF name

OBJECTDLLNAME = AW32auth.DLL
OBJECTDEFNAME = authen.DEF
LOCALCCFLAGS  = -g150 -K
LOCALINCLUDE  = ..\include;q:\include
LOCALLIBS     = AW32flag.lib AW32nmbr.lib AW32nwer.lib AW32tabl.lib AW32text.lib .\AW32mx.lib q:\win\dstools.lib q:\win\nwcalls.lib q:\win\nwlocale.lib q:\win\nwnet.lib
OBJECTAPI     = AW32auth.LIB

# Q: is mapped to MAP ROOT Q:=MUR-SERIUS1\SYS:DEVTOOLS\NWCLNT41.SDK\BLD\940225\CLNT
