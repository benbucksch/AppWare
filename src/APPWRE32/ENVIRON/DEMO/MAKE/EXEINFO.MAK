# Exe-specific make information:
#
#  EXE name
#  .DEF name

EXENAME       = AW32DEMO.EXE
DEFNAME       = ..\APEBLDR\SRC\SERPROG.DEF
LOCALCCFLAGS  = -ml -DDEMO
LOCALINCLUDE  = ..\APEBLDR\HDR;..\APEBLDR\RC
LOCALLIBS     = AW32RUN.LIB CTL3D32.LIB
CPPDIR        = .
RCDIR         = ..\APEBLDR\RC
