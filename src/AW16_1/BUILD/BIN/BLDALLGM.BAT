n:
cd \

REM internal
\build\bin\makeenv -i -n  -u -ln:\build\log\intern
\build\bin\copyenv -c -an:\build\log\intern
\build\bin\makeobj -i -n -an:\build\log\intern make
\build\bin\makeobj -i -n  -u -an:\build\log\intern
\build\bin\makeobj -i -n -an:\build\log\intern

REM debug
\build\bin\makeenv -d -i -n  -ln:\build\log\debug
\build\bin\copyenv -d -c -an:\build\log\debug
\build\bin\makeobj -d -i -n -an:\build\log\debug make
\build\bin\makeobj -d -i -n  -an:\build\log\debug
\build\bin\makeobj -d -i -n -an:\build\log\debug

REM ship
\build\bin\makeenv -s -i -n  -ln:\build\log\ship
\build\bin\copyenv -s -c -an:\build\log\ship
\build\bin\makeobj -s -i -n -an:\build\log\ship make
\build\bin\makeobj -s -i -n  -an:\build\log\ship
\build\bin\makeobj -s -i -n -an:\build\log\ship
