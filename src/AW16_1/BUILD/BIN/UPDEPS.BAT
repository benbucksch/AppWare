@echo off
del make\deps > NUL

makelist -hOBJECTFILES= -m$(OBJDIR)\ -amake\deps -sobj @make\files.lst
scanfile -amake\deps -m$(OBJDIR)\ -i -p -c @make\files.lst

makelist -hOBJECTRCFILE= -m$(OBJDIR)\ -amake\deps -sres @make\rcfiles.lst
scanfile -amake\deps -m$(OBJDIR)\ -i -p -r @make\rcfiles.lst
