REM need -d date to specify date
REM need -t time to spedify time

attrib -r n:\ship\*.* /s
                          
n:\build\bin\touch %1 %2 n:\ship\*.*
n:\build\bin\touch %1 %2 n:\ship\bin\debug\*.*
n:\build\bin\touch %1 %2 n:\ship\bin\nodebug\*.* 
n:\build\bin\touch %1 %2 n:\ship\bin\*.*
n:\build\bin\touch %1 %2 n:\ship\config\*.*
n:\build\bin\touch %1 %2 n:\ship\include\*.*
n:\build\bin\touch %1 %2 n:\ship\lib\*.*
n:\build\bin\touch %1 %2 n:\ship\update\*.*
n:\build\bin\touch %1 %2 n:\ship\help\*.*

attrib +r n:\ship\*.* /s
