REM This requires Visual C++ and MFC2.0 real-mode large-model library

cl /c /W3 /AL /G2 /Zip /F 2000 makelist.cpp
link /COD /NOE makelist.obj,makelist.exe,,lafxcr.lib,makelist.def

cl /c /W3 /AL /G2 /Zip /F 2000 scanfile.cpp
link /COD /NOE scanfile.obj,scanfile.exe,,lafxcr.lib,scanfile.def

cl /W3 /AL basename.c
