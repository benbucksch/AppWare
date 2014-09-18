# COPYENV.AWK
#
# Copies the set of files named in the file envmake\intfiles.lst
#

global	szListFile			= "n:\\envmake\\intfiles.lst"
global	dTargetDir			= "n:\\internal"

global	dTargetMap			# (ary) mapping from keyword to directory

global	fClearTarget		# delete directory contents before copy:  -c option						
global	szLogFile				# logfile to record to:  -l or -a option
global	fLogAppend			# append to logfile:  -a option
global	fLogCleared
global	fAppend
global	fDebugBuild
global	fShipBuild


################################################################
#
#
#
################################################################

function GetFileName(file)
	{
	local		idx

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
	local		idx
	local		outdir

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
	local		outdir

	if (fDebugBuild)
		outdir = sprintf("%s\\DEBUG", dir)
	else if (fShipBuild)
		outdir = sprintf("%s\\SHIP", dir)
	else
		outdir = dir

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
	local		opt
	local		dTarget
	local		file
	local		dstdir
	local		sreplace

	InitCopy()

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
		printf("      Options:\n")
		printf("        -?:         display this message\n")
		printf("        -alogfile:  append output to logfile (keeps old)\n")
		printf("        -c:         clear target directories before copy\n")
		printf("        -llogfile:  record output to logfile (destroys old)\n")
		printf("        -d:         copy debug build files (external)\n")
		printf("        -s:         copy ship build files (external)\n")
		}

	else if (!fError)
		{
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

		while (getline < szListFile)
			{
			if ($0 == "" || substr($0, 1, 1) == "#")
				;
			else if ($2 == "" || dTargetMap[toupper($1)] == "")
				printf("!!Invalid line in %s:  \"%s\"\n", szListFile, $0)
			else
				{
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
