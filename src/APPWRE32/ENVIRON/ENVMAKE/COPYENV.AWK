# COPYENV.AWK
#
# Copies the set of files named in the file envmake\intfiles.lst
#

global  szListFile                      
global  dTargetDir                      
global  dHomeDir                                = "o:"

global  dTargetMap                      # (ary) mapping from keyword to directory

global  fClearTarget            # delete directory contents before copy:  -c option                                             
global  szLogFile                               # logfile to record to:  -l or -a option
global  fLogAppend                      # append to logfile:  -a option
global  fLogCleared
global  fAppend
global  fDebugBuild
global  fShipBuild
global  fMakeDirs


################################################################
#
#
#
################################################################

function VerifyDirectory(dir)
	{
  if(fMakeDirs)
		ExecuteCmd(sprintf("IF NOT EXIST %s MKDIR %s", dir, dir))
	}

################################################################
#
#
#
################################################################

function GetFileName(file)
	{
	local           idx

	gsub(/\//, "\\", file)

	for (;;)
		{
		idx = index(file, "\\")
		if (idx == 0)
			break;
		file = substr(file, idx + 1)
		}

	return file
	}

################################################################
#
# 
#
################################################################

function GetDir(dir)
	{
	local           idx
	local           outdir

	gsub(/\//, "\\", dir)

	for (idx = length(dir); idx > 0; idx--)
		{
		if (substr(dir, idx, 1) == "\\")
			break;
		}

	if (idx == 0)
		outdir = dir
	else
		outdir = substr(dir, 1, idx - 1)

	return outdir
	}

################################################################
#
#
#
################################################################

function InitCopy()
	{
	szListFile                      = dHomeDir "\\environ\\envmake\\intfiles.lst"
	dTargetDir                      = dHomeDir "\\internal"
  
  VerifyDirectory(dTargetDir)
	
	dTargetMap["BIN"]     = sprintf("%s\\%s", dTargetDir, "BIN"    )
	dTargetMap["INCLUDE"] = sprintf("%s\\%s", dTargetDir, "INCLUDE")
	dTargetMap["LIB"]     = sprintf("%s\\%s", dTargetDir, "LIB"    )
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

function ClearTarget(dir)
	{
	LogMessage(sprintf("--> Deleting contents of %s\n", dir))
	ExecuteCmd(sprintf("attrib -r %s\\*.*", dir))
	ExecuteCmd(sprintf("echo y | del %s\\*.*", dir))
	}

################################################################
#
# Returns the destination directory:  . or DEBUG or SHIP
#
################################################################

function GetDstDirectory(dir)
	{
	local           outdir

	if (fDebugBuild)
		outdir = sprintf("%s\\DEBUG", dir)
	else if (fShipBuild)
		outdir = sprintf("%s\\SHIP", dir)
	else
		outdir = dir

	VerifyDirectory(outdir)
    
	return outdir
	}

################################################################
#
#
#
################################################################

BEGIN   {
	local           fHelp
	local           fError
	local           opt
	local           dTarget
	local           file
	local           dstdir
	local           sreplace

	for (opt = 1; opt < ARGC; opt++)
		{
		if (ARGV[opt] == "-h" || ARGV[opt] == "-?" || ARGV[opt] == "?")
			{
			fHelp = 1
			break
			}
		else if (ARGV[opt] == "-c")
			fClearTarget = 1
		else if (ARGV[opt] == "-d")
			fDebugBuild = 1
		else if (ARGV[opt] == "-s")
			fShipBuild = 1
    else if (ARGV[opt] == "-md")
	fMakeDirs = 1
    else if (substr(ARGV[opt], 1, 2) == "-h")
	dHomeDir = substr(ARGV[opt], 3)
      
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
		printf("      -c:           clear target directories before copy\n")
		printf("      -d:           copy debug build files (external)\n")
    printf("      -hdirectory:  home directory\n")
		printf("      -llogfile:    record output to logfile (destroys old)\n")
		printf("      -md:          make needed directories\n")
		printf("      -s:           copy ship build files (external)\n")
		}

	else if (!fError)
		{
		InitCopy()
    
		LogMessage(sprintf("Performing copy of environment files to %s\n", dTargetDir))
		
		if (fClearTarget)
			{
			for (dTarget in dTargetMap)
				ClearTarget(GetDstDirectory(dTargetMap[dTarget]))
			}

		if (fDebugBuild)
			sreplace = "\\DOBJ\\"
		else if (fShipBuild)
			sreplace = "\\SOBJ\\"
		else
			sreplace = ""

		while ((getline < szListFile) > 0)
			{
			if ($0 == "" || substr($0, 1, 1) == "#")
				;
			else if ($2 == "" || dTargetMap[toupper($1)] == "")
				printf("!!Invalid line in %s:  \"%s\"\n", szListFile, $0)
			else
				{
        #add on the home directory
        $2 = dHomeDir "\\" $2
        
				gsub(/\//, "\\", $2)

				# Now make path substitutions for DEBUG/SHIP
				if (sreplace != "")
					gsub(/\\[oO][bB][jJ]\\/, sreplace, $2)

				file   = GetFileName($2)
				dstdir = GetDstDirectory(dTargetMap[toupper($1)])

				if (!fClearTarget)
					ExecuteCmd(sprintf("attrib -r %s\\%s", dstdir, file))

				if ($3 == "")
					ExecuteCmd(sprintf("copy %s\t%s", $2, dstdir))
				else
					ExecuteCmd(sprintf("copy %s\t%s\\%s", $2, dstdir, $3))

				if (!fClearTarget)
					ExecuteCmd(sprintf("attrib +r %s\\%s", dstdir, file))
				}
			}
		}
	}
