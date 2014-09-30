# COPYLIST.AWK
#
# Copies the set of files named in the file envmake\copylist.lst
#

global	dTargetDir			
global  dHomeDir			

global  copyList

global	szLogFile				# logfile to record to:  -l or -a option
global  szListFile
global	fLogAppend			# append to logfile:  -a option
global	fLogCleared
global	fAppend



################################################################
#
#
#
################################################################

function InitCopy()
	{
  
  if(dHomeDir == "")
  	dHomeDir = "G:"
    
  if(dTargetDir == "")
  	dTargetDir = "N:"
    
	szListFile			= dHomeDir "\\envmake\\copylist.lst"
	}

################################################################
#
#
#
################################################################

function LogMessage(msg)
	{
	if (!fLogCleared && szLogFile != "")
		{
		if (fAppend)
			fopen(szLogFile, "a")
		else
			fopen(szLogFile, "w")

		fwrite("******************************\n", szLogFile)
		fLogCleared = 1
		}
	else
		fopen(szLogFile, "a")

	if (szLogFile != "")
		fwrite(sprintf("%s\n", msg), szLogFile)

	print msg

	if (szLogFile != "")
		close(szLogFile)
	}

################################################################
#
#
#
################################################################

function ExecuteCmd(cmd)
	{
	LogMessage(sprintf("\t+++  %s\n", cmd))

	if (szLogFile != "")
		system(sprintf("%s >> %s", cmd, szLogFile))
	else
		system(cmd)
	}

################################################################
#
#
#
################################################################

function GetDstDirectory(dir)
	{
  local outdir = dTargetDir "\\" dir
    
	gsub(/\//, "\\", outdir)

	return outdir
	}

################################################################
#
#
#
################################################################

function GetSrcDirectory(dir)
	{
  local outdir = dHomeDir "\\" dir
    
	gsub(/\//, "\\", outdir)

	return outdir
	}

################################################################
#
#
#
################################################################

BEGIN	{
	local		fHelp
	local		fError
	local		dstdir
	local		srcdir
  local   fMode
  local   opt

	for (opt = 1; opt < ARGC; opt++)
		{
		if (ARGV[opt] == "-h" || ARGV[opt] == "-?" || ARGV[opt] == "?")
			{
			fHelp = 1
			break
			}
    else if (substr(ARGV[opt], 1, 2) == "-s")
    	dHomeDir = substr(ARGV[opt], 3)
      
    else if (substr(ARGV[opt], 1, 2) == "-t")
    	dTargetDir = substr(ARGV[opt], 3)
      
		else if (substr(ARGV[opt], 1, 2) == "-a")
			{
			szLogFile = substr(ARGV[opt], 3);
			fAppend = 1
			}
		else if (substr(ARGV[opt], 1, 2) == "-l")
			szLogFile = substr(ARGV[opt], 3);
		else
			{
			printf("Unknown option %s\n", ARGV[opt])
			fError = 1
			}
		}

	if (fHelp)
		{
		printf("Usage:  copyenv [-?] [options]\n\n")
		printf("    Options:\n")
		printf("      -?:           display this message\n")
		printf("      -alogfile:    append output to logfile (keeps old)\n")
    printf("      -sdirectory:  source home directory\n")
    printf("      -tdirectory:  target home directory\n")
		printf("      -llogfile:    record output to logfile (destroys old)\n")
		}

	else if (!fError)
		{
		InitCopy()
    
		while ((getline < szListFile)>0)
			{
			if ($0 == "" || substr($0, 1, 1) == "#")
				;
			else
				{
				dstdir = GetDstDirectory(toupper($1))
				srcdir = GetSrcDirectory(toupper($1))
        
        fMode = filemode(srcdir)
        if(fMode == "")
        	{
          fError = 1
        	LogMessage(sprintf("%s is an invalid source directory or file", srcdir))
          }
          
        fMode = filemode(dstdir)
        if(fMode == "")
        	{
          fError = 1
          LogMessage(sprintf("%s is an invalid destination directory or file", dstdir))
          }
        
        if(!fError)
        	{
          #see if we're a directory or file and set up appropriately
          if(0 != index(fMode, "d"))
          	opt = "\\*.*"
          else
          	opt = ""
            
        	ExecuteCmd(sprintf("attrib -r %s%s /s", dstdir, opt))
					ExecuteCmd(sprintf("xcopy %s /s/e %s", srcdir, dstdir))
          ExecuteCmd(sprintf("attrib +r %s%s /s", dstdir, opt))
          }
        else
        	fError = 0  #so we can try to copy more
				}
			}
      
     close(szListFile)
		}
	}
