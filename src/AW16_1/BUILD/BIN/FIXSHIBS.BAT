REM Runs fixlib on all .lib files in the n:\ship\lib directory
n:
cd n:\ship\lib

attrib -r *.lib

FOR %%X IN (*.lib) DO fixlib %%X ;

del *.bak

attrib +r *.lib
