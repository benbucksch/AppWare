n:
cd \
\build\bin\makeenv -i -n -m -u -lc:\tmp\log
\build\bin\copyenv -c -ac:\tmp\log
\build\bin\makeobj -i -n -ac:\tmp\log make
\build\bin\makeobj -i -n -m -u -ac:\tmp\log
\build\bin\makeobj -i -n -ac:\tmp\log

REM debug
\build\bin\makeenv -d -i -n -m -lc:\tmp\debug
\build\bin\copyenv -d -c -ac:\tmp\debug
\build\bin\makeobj -d -i -n -ac:\tmp\debug make
\build\bin\makeobj -d -i -n -m -ac:\tmp\debug
\build\bin\makeobj -d -i -n -ac:\tmp\debug

REM ship
\build\bin\makeenv -s -i -n -m -lc:\tmp\ship
\build\bin\copyenv -s -c -ac:\tmp\ship
\build\bin\makeobj -s -i -n -ac:\tmp\ship make
\build\bin\makeobj -s -i -n -m -ac:\tmp\ship
\build\bin\makeobj -s -i -n -ac:\tmp\ship
