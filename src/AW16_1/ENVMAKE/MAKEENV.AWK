# MAKEENV.AWK
#
# Performs a make of the environment modules.
#
#
#

global  fNetwork                                        # compiling on network:  -n option
global  fUpdateDeps                             # update dependencies:  -u option
global  nomakeProjects          # projects that should be retrieved only
global  dllProjects                             # exeProjects that export a .LIB
global  exeProjects                             # exeProjects that don't export a .LIB
global  projectsToBuild         # projects to build specified by user
global  dHomeDir                                        # home directory above all directories
global  szLogFile
global  fLogCleared
global  fAppend
global  fGetFiles                                       # perform an "ss get" before compiling:  -g
global  fMakeAll                                        # force a remake on all targets:  -m
global  fIgnoreErrors                   # ignore errors during make
global  fDebugBuild                             # add DEBUG=1 to command line
global  fShipBuild                              # add SHIP=1 to command line
global  fDontMake                                       # skip the make itself
################################################################
#
#
#
################################################################

function InitArys()
	{
	nomakeProjects["ENVMAKE"]                       # strange syntax declares an array element
	nomakeProjects["MEMIO"]
	nomakeProjects["UPSF"]

	dllProjects["OIP"]
	dllProjects["CFGUPDAT"]

	exeProjects["DEV"]
	exeProjects["PRO"]
	exeProjects["SHELL"]
	}

################################################################
#
#
#
################################################################

function InitMake()
	{
	local           fOk             = 1

	if (fNetwork)
		{
		if(dHomeDir == "")
			dHomeDir  = "N:"

		ENVIRON["INCLUDE"] = "HDR;RC;..\\DEV\\HDR;..\\MEMIO\\HDR;..\\OIP\\HDR;..\\PRO\\HDR;..\\CFGUPDAT\\HDR;R:\\BORLANDC\\INCLUDE"
		ENVIRON["LIB"]     = "..\\OIP\\OBJ;..\\CFGUPDAT\\OBJ;R:\\BORLANDC\\LIB"
		ENVIRON["PATH"]    = "R:\\APPWARE\\BUILD\\BIN;R:\\BORLANDC\\BIN;R:\\OPTLINK;R:\\SRCSAFE"
		}
	else
		{
		if (ENVIRON["HOMEENV"] != "")
			dHomeDir = ENVIRON["HOMEENV"]
		else
			{
			printf("Non-network make requires a  HOMEENV environment variable\n")
			fOk = 0
			}

		if (ENVIRON["INCLUDE"] == "")
			{
			printf("Non-network make requires an INCLUDE environment variable\n")
			fOk = 0
			}
		
		if (ENVIRON["LIB"] == "")
			{
			printf("Non-network make requires a  LIB environment variable\n")
			fOk = 0
			}
		}

	return fOk
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

BEGIN   {
	local           dCWD
	local           fHelp
	local           fError
	local           fListOfProjects
	local           project
	local           opt
	local           buildFlags
	local           buildOpt1
	local           buildOpt2
	local           buildCmd

	InitArys()

	for (opt = 1; opt < ARGC; opt++)
		{
		if (ARGV[opt] == "-h" || ARGV[opt] == "-?" || ARGV[opt] == "?")
			{
			fHelp = 1
			break
			}
		else if (ARGV[opt] == "-d")
			fDebugBuild = 1
		else if (ARGV[opt] == "-s")
			fShipBuild = 1
		else if (ARGV[opt] == "-n")
			fNetwork = 1
		else if (ARGV[opt] == "-g")
			fGetFiles = 1
		else if (ARGV[opt] == "-i")
			fIgnoreErrors = 1
		else if (ARGV[opt] == "-m")
			fMakeAll = 1
		else if (ARGV[opt] == "-u")
			fUpdateDeps = 1
		else if (ARGV[opt] == "-x")
			fDontMake = 1
		else if (toupper(ARGV[opt]) in nomakeProjects ||
							toupper(ARGV[opt]) in dllProjects   ||
							toupper(ARGV[opt]) in exeProjects)
			{
			projectsToBuild[toupper(ARGV[opt])]
			fListOfProjects = 1
			}
		else if (substr(ARGV[opt], 1, 2) == "-a")
			{
			szLogFile = substr(ARGV[opt], 3)
			fAppend = 1
			}
		else if (substr(ARGV[opt], 1, 2) == "-l")
			szLogFile = substr(ARGV[opt], 3)

		else if (substr(ARGV[opt], 1, 2) == "-h")
			dHomeDir = substr(ARGV[opt], 3)
		else
			{
			printf("Unknown option %s\n", ARGV[opt])
			fError = 1
			}
		}

	if (fHelp)
		{
		printf("Usage:  makeenv [-?] [options] [project] ...\n\n")
		printf("      Options:\n")
		printf("        -?:         display this message\n")
		printf("        -alogfile:  append output to logfile (keeps old)\n")
		printf("        -llogfile:  record output to logfile (destroys old)\n")
		printf("        -d:         perform a DEBUG build (external)\n")
		printf("        -g:         perform SS GET prior to make\n")
		printf("        -i:         ignore errors in make\n")
		printf("        -m:         force rebuild of all\n")
		printf("        -n:         perform network make\n")
		printf("        -s:         perform a SHIP build (external)\n")
		printf("        -u:         update dependencies before make\n")
		printf("        -x:         don't perform make (useful with -g)\n")
		printf("        -h[directory]:          specify home directory\n")
		}

	else if (!fError && InitMake())
		{
		dCWD = getcwd()

		if (!fListOfProjects)
			{
			for (project in nomakeProjects)
				projectsToBuild[project]
			for (project in dllProjects)
				projectsToBuild[project]
			for (project in exeProjects)
				projectsToBuild[project]
			}

		if (fGetFiles)
			{
			for (project in projectsToBuild)
				{
				LogMessage(sprintf("Getting files from SourceSafe in project %s", project))
				ExecuteCmd(sprintf("ss cd $/appware/appbuild/%s", project))
				chdir(sprintf("%s\\%s", dHomeDir, project))
				ExecuteCmd("ss get * -r")
				}
			}

		LogMessage("Starting make of system\n")

		if (fDebugBuild)
			buildFlags = "DEBUG=1"
		else if (fShipBuild)
			buildFlags = "SHIP=1"
		else
			buildFlags = "INTERNAL=1"

		if (fIgnoreErrors)
			buildOpt1 = "-i"
		else
			buildOpt1 = ""

		if (fMakeAll)
			buildOpt2 = "-B"
		else
			buildOpt2 = ""

		buildCmd = sprintf("make %s %s %s", buildOpt1, buildOpt2, buildFlags)

		for (project in projectsToBuild)
			{
			if (!(project in nomakeProjects))
				{
				LogMessage("********************************")
				LogMessage(sprintf("Starting make of %s", project))
				chdir(sprintf("%s\\%s", dHomeDir, project))

				if (fUpdateDeps)
					{
					LogMessage(sprintf("--> Updating dependencies"))
					ExecuteCmd("updeps")
					}

				if (!fDontMake)
					{
					if (fMakeAll)
						LogMessage(sprintf("--> Forcing rebuild of all"))
					else
						LogMessage(sprintf("--> Performing make"))

					LogMessage(buildCmd)
					ExecuteCmd(buildCmd)
					}
				}
			}

		chdir(dCWD)
		}
	}
