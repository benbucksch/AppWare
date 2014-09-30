# MAKEOBJ
#
# Makes all object DLLs
#

global  objectProjFile        = "objproj.lst"
global  objectExportList      = "objexprt.lst"      # files to release
global  gAlmPathFile          = "almpath.lst"
global  dTargetDir            

global  apiProjects                                 # projects that export a .LIB (read from objectExportList)
global  objectProjects                              # valid object projects to make (read from objectProjFile)
global  dTargetMap                                  # (ary) target directory map

global  fOfficial                                   # official make
global  projToBuild                                 # (ary) list of projects to build
global  szLogFile
global  fAppend
global  fLogCleared
global  fMakeAll
global  fUpdateDeps
global  dHomeDir
global  fIgnoreErrors
global  fDebugBuild
global  fShipBuild
global  fDoRemove
global  fCopyAllDlls          = 1
global  fCopySpecified
global  fBuildLibs            = 1
global  fCopyApis             = 1
global  fMakeDirs
global  dBaseDir              = "O:"
global  gSSProject            = "appware32"
global  fCopyConfigs          = 1


################################################################
#
#
################################################################
function ChangeDir(dir)
	{
	LogMessage("Changing directory to: " dir);
	chdir(dir)
	}

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
# Must be called after init make but before anything else
#
#
################################################################

function InitArys()
	{
	dTargetMap["BIN"]     = sprintf("%s\\%s", dTargetDir, "BIN")
	dTargetMap["INCLUDE"] = sprintf("%s\\%s", dTargetDir, "INCLUDE")
	dTargetMap["LIB"]     = sprintf("%s\\%s", dTargetDir, "LIB")  
	dTargetMap["CONFIGS"] = sprintf("%s\\%s", dTargetDir, "CONFIG")
	}

################################################################
# Call InitArys as well
#
#
################################################################

function InitMake()                             
	{
	local           fok                     = 1
	local           prjList
	local           pathList
	
	if (fOfficial)
		{
		dHomeDir = dBaseDir "\\ALMS"
		dTargetDir = dBaseDir "\\INTERNAL"
	 
		#eliminate possible double slash from the above statement
		gsub(/\\\\/, "\\", dHomeDir);  
	
		pathList = sprintf("%s\\make\\%s", dHomeDir, gAlmPathFile);
	
		while((getline < pathList) > 0)
			{
			if ($0 != "" && substr($0, 1, 1) != "#")
				{
				if(($2 != "") && ($3 == ""))
					{
					$1 = toupper($1)
					
					#replace with single back slashes
					gsub(/\//, "\\", $2)
					
					#replace any double slashes with single slashes
					gsub(/\\\\/, "\\", $2);

					if($1 == "INCLUDE")
						{
						ENVIRON[$1] = $2
						}
					else if($1 == "LIB")
						{
						ENVIRON[$1] = $2
						}
					else if($1 == "PATH")
						{
						ENVIRON[$1] = $2
						}
					else
						{
						fok = 0
						}
					}
				else
					{
					fok = 0
					}      
				}
	
			if(!fok)
				{
				LogMessage(sprintf("Error: Invalid line: [ %s ] in file: %s", $0, pathList))
				break;        
				}
			}
			
		close(pathList)

		}
	else
		{
		if(dHomeDir == "")
			{
			#no home directory specified
			if (ENVIRON["HOMEOBJ"] != "")
				{
				dHomeDir = ENVIRON["HOMEOBJ"]
				}
			else
				{
				printf("Non-network make requires a  HOMEOBJ environment variable\n")
				fok = 0
				}
			}
			
		if (ENVIRON["INCLUDE"] == "")
			{
			printf("Non-network make requires an INCLUDE environment variable or a home directory\n")
			fok = 0
			}
		
		if (ENVIRON["LIB"] == "")
			{
			printf("Non-network make requires a  LIB environment variable or a home directroy\n")
			fok = 0
			}
		}

	if(fok)
		InitArys()
	
	return fok
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

function ValidateProjList()
	{
	local           fOk                             = 1
	local           project

	for (project in projToBuild)
		{
		if (!(project in objectProjects))
			{
			fOk = 0
			LogMessage(sprintf("Invalid project %s\n", project))
			}
		}

	return fOk
	}

################################################################
#
#
#
################################################################

function SetupProjInfo()
	{
	local           fOk                             = 1
	local           apiList
	local           prjList
	local           proj

	prjList = sprintf("%s\\make\\%s", dHomeDir, objectProjFile)

	while ((getline < prjList)>0)
		{
		if ($0 != "" && substr($0, 1, 1) != "#")
			objectProjects[toupper($1)] = $2
		}

	close(prjList)

	apiList = sprintf("%s\\make\\%s", dHomeDir, objectExportList)

	while ((getline < apiList)>0)
		{
		if ($0 != "" && substr($0, 1, 1) != "#")
			{
			$1 = toupper($1)

			if ($2 == "" || dTargetMap[$1] == "")
				{
				LogMessage(sprintf("!!Invalid line in %s:  \"%s\"", apiList, $0))
				fOk = 0
				}
			else
				{
				gsub(/\//, "\\", $2)
				
				proj = substr(toupper($2), 1, index($2, "\\") - 1)

				if (!(proj in objectProjects))
					{
					if (proj != "INCLUDE" && proj != "CONFIGS")
						{
						LogMessage(sprintf("Project %s is not a valid project", proj))
						fOk = 1
						break;
						}
					}
				else if ($1 == "LIB")
					apiProjects[proj]
				}
			}
		}

	close(apiList)


	return fOk
	}


################################################################
#
#
#
################################################################

function ValidateGetProjects()
	{
	local           apiList
	local           prjList
	local           proj

	prjList = sprintf("%s\\make\\%s", dHomeDir, objectProjFile)

	while ((getline < prjList)>0)
		{
		if ($0 != "" && substr($0, 1, 1) != "#")
			{
			if(toupper($3) == "ALWAYSGET")
				{
				if((toupper($1) in objectProjects) && (!(toupper($1) in projToBuild)))
					{
					projToBuild[toupper($1)]
					LogMessage(sprintf("Adding %s to list for getting.", $1));
					}
				}
			 }
		}
					
	close(prjList)
	}
				
################################################################
#
#
#
################################################################

function GetDstDirectory(tag)
	{
	local           dstdir

	tag = dTargetMap[tag]

	if(tag != "CONFIGS")
		{
		if (fDebugBuild)
			dstdir = sprintf("%s\\DEBUG", tag)
		else if (fShipBuild)
			dstdir = sprintf("%s\\SHIP", tag)
		else
			dstdir = tag
		}
	else
		dstdir = tag

	return dstdir
	}

################################################################
#
#
#
################################################################

function CopyObjectAPIFiles(tag)
	{
	local           file
	local           apiList
	local           sreplace="";
	local           dstdir

	tag = toupper(tag)

	if(tag == "BIN")
		{
		if (fDebugBuild)
			sreplace = "\\DOBJ\\"
		else if (fShipBuild)
			sreplace = "\\SOBJ\\"
		}
		

	apiList = sprintf("%s\\make\\%s", dHomeDir, objectExportList)

	while ((getline < apiList)>0)
		{
		if ($0 != "" && substr($0, 1, 1) != "#")
			{
			$1 = toupper($1)
			if ($1 == tag)
				{
				#copy if not a bin file or we're copying all dlls or the dll is
				#a specified dll.
				if(($1 != "BIN") || 
					fCopyAllDlls || (substr(toupper($2), 1, index($2, "\\") - 1) in projToBuild))  
					{
						 
					gsub(/\//, "\\", $2)

					# Now make path substitutions for DEBUG/SHIP
					if (sreplace != "")                                    
						gsub(/\\[oO][bB][jJ]\\/, sreplace, $2)

					dstdir = GetDstDirectory($1)

					file = GetFileName($2)

					VerifyDirectory(dstdir)
						
					ExecuteCmd(sprintf("attrib -r %s\\%s", dstdir, file))
					ExecuteCmd(sprintf("copy %s\\%s\t%s", dHomeDir, $2, dstdir))
					ExecuteCmd(sprintf("attrib +r %s\\%s", dstdir, file))
					}
				}
			}
		}

	close(apiList)
	}

################################################################
#
#
#
################################################################

function MakeProject(proj, op)
	{
	local           buildCmd
	local           buildFlags
	local           buildOpt1
	local           buildOpt2
	local           buildWhat
	local           buildMsg
	local           onlyLib = 0

	LogMessage("********************************")
	LogMessage(sprintf("Starting make of %s", proj))
	
	VerifyDirectory(sprintf("%s\\%s", dHomeDir, proj))
		
	ChangeDir(sprintf("%s\\%s", dHomeDir, proj))

	if(toupper(op) == "LIB")
		{
		buildWhat = "MAKELIB=1"
		onlyLib = 1
		buildMsg = "Building .LIB only"
		}
	else if(toupper(op) == "DLL")
		{
		buildWhat = "MAKEDLL=1"
		buildMsg = "Building .DLL only"
		}
	else 
		{
		buildWhat = "MAKEALL=1" 
		buildMsg = "Building both .DLL and .LIB"
		}
		
	if (fIgnoreErrors)
                buildOpt1 = "-I"
	else
		buildOpt1 = ""

        if (fMakeAll && ((fBuildLibs && onlyLib) || (!fBuildLibs)))
                buildOpt2 = "-A"
	else
		buildOpt2 = ""
		
	if (fDebugBuild)
		buildFlags = "DEBUG=1"
	else if (fShipBuild)
		buildFlags = "SHIP=1"
	else
		buildFlags = "INTERNAL=1"

        buildCmd = sprintf("nmake %s %s %s %s", buildOpt1, buildOpt2, buildFlags, buildWhat)

	if (fUpdateDeps)
		{
		LogMessage(sprintf("--> Updating dependencies"))
		ExecuteCmd("updeps")
		}

	if (onlyLib)
		LogMessage(sprintf("--> %s", buildMsg))
	else
		{
		if (fMakeAll)
			LogMessage(sprintf("--> %s --> Forcing rebuild of all", buildMsg))
		else
			LogMessage(sprintf("--> %s --> Performing make", buildMsg))
		}
		
	ExecuteCmd(buildCmd);
	}                       

################################################################
#
#
#
################################################################

BEGIN{
	local           dCWD
	local           fHelp
	local           fError
	local           fListOfProj
	local           opt
	local           project
	local           fGetFiles
	local           holdMakeDirs
	
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
		else if (ARGV[opt] == "-o")
			fOfficial = 1
		else if (ARGV[opt] == "-i")
			fIgnoreErrors = 1
		else if (ARGV[opt] == "-g")
			fGetFiles = 1
		else if (ARGV[opt] == "-m")
			fMakeAll = 1
		else if (ARGV[opt] == "-u")
			fUpdateDeps = 1
		else if (ARGV[opt] == "-r")
			fDoRemove = 1
		else if (ARGV[opt] == "-md")
			fMakeDirs = 1
		else if(ARGV[opt] == "-cd-")
			fCopyAllDlls = 0
		else if(ARGV[opt] == "-cl-")
			fBuildLibs = 0
		else if(ARGV[opt] == "-api-")
			fCopyApis = 0
		else if(ARGV[opt] == "-cs")
			fCopySpecified = 1
			
		else if (substr(ARGV[opt], 1, 2) == "-a")
			{
			szLogFile = substr(ARGV[opt], 3);
			fAppend = 1
			}
		else if (substr(ARGV[opt], 1, 2) == "-l")
			szLogFile = substr(ARGV[opt], 3)
		else if(substr(ARGV[opt],1,4) == "-cc-")
			fCopyConfigs = 0
		else if(substr(ARGV[opt], 1, 2)  == "-h")
			{
			dBaseDir = substr(ARGV[opt], 3)
			dHomeDir = dBaseDir "\\alms"
			dTargetDir = dBaseDir "\\internal"
			}
			
		else if (substr(ARGV[opt], 1, 1) == "-")
			{
			printf("Unknown option %s\n", ARGV[opt])
			fError = 1
			}
		else
			{
			projToBuild[toupper(ARGV[opt])]
			fListOfProj = 1
			}
		}
	

	if(!fOfficial)
		{
		#don't know where to copy
		fCopyAllDlls = 0
		fCopySpecified = 0
		fCopyApis = 0
		fBuildLibs = 0
		}
		
	#can't remove files if we're not doing a get
	if(fGetFiles != 1)
		{
		if(fDoRemove)
			{
			LogMessage("Can't delete files since -g was not also specified.")
			fDoRemove = 0
			}
		}
	
	if(fCopySpecified)
		{
		if(!fCopyAllDlls)
			LogMessage("Copying specified dlls takes precedence over suppressing copy")
		else
			fCopyAllDlls = 0
		}
			
	if (fHelp)
		{
		printf("Usage:  makeobj [-?] [project] ...\n\n")
		printf("    -?:           display this message\n")
		printf("    -alog:        append output to logfile (keeps old)\n")
		printf("    -cs:          copy specified dlls\n")
		printf("    -d:           perform DEBUG build (external)\n")
		printf("    -g:           perform SS GET prior to make\n")
		printf("    -hdirectory:  home directory\n")
		printf("    -i:           ignore errors in make\n")
		printf("    -llog:        record output to logfile (destroys old)\n")
		printf("    -m:           force rebuild of all\n")
		printf("    -md:          makes needed directories\n")
		printf("    -o:           official make - copy by default and use default paths\n")
		printf("    -r:           remove all files prior to getting if -g is also specified\n");
		printf("    -s:           perform SHIP build (external)\n")
		printf("    -u:           update dependencies before make\n")
		printf("    -api-:        supress copy of api's prior to make\n")
		printf("    -cd-:         suppress copy of dlls\n")
		printf("    -cl-:         supress create and copy of .lib files\n")
		printf("    -cc-:         supress copying of configs\n")
		}
	else
		{
		if (!fError && InitMake() && SetupProjInfo() && (!fListOfProj || ValidateProjList()))
			{                                                                 
			dCWD = getcwd() 

			if (!fListOfProj)
				{
				for (project in objectProjects)
					projToBuild[project]
				}

			if (fGetFiles)
				{
				ValidateGetProjects();
				
				for (project in projToBuild)                                       
					{
					#set source safe to the appropriate directory
					ExecuteCmd(sprintf("ss cd $/%s/alms/%s", gSSProject, project))
					
					VerifyDirectory(sprintf("%s\\%s", dHomeDir, project))
					
					ChangeDir(sprintf("%s\\%s", dHomeDir, project))
					
					#don't delete the make directory because its caused
					#me too much grief in the past.
					if(fDoRemove == 1 && (toupper(project) != "MAKE"))
						{
						LogMessage(sprintf("Deleting files for project %s", project))
						ExecuteCmd("attrib -r *.* /s")
						ExecuteCmd("echo y | del /s *.*")
						}
						
					LogMessage(sprintf("Getting files from SourceSafe in project %s", project))
					
					ExecuteCmd("ss get * -r")
					}
					
				}

			if (fCopyApis)
				{
				LogMessage("Copying files to common include")
				CopyObjectAPIFiles("INCLUDE")
				}
				
			LogMessage("Starting make of system\n")

			#first make necessary .LIBs
			if(fBuildLibs)
				{
				for (project in apiProjects)
					{
					if(toupper(objectProjects[project]) != "NOMAKE")
						MakeProject(project, "LIB")
					}
					
				CopyObjectAPIFiles("LIB")
				}
			
			# Make all projects
			
			holdMakeDirs = fMakeDirs
			
			if(fGetFiles)
				fMakeDirs = 0   #directories were made during getting
				
			for (project in projToBuild)
				if(toupper(objectProjects[project]) != "NOMAKE")
					MakeProject(project, "DLL")

			fMakeDirs = holdMakeDirs  #restore  for copying dlls
			
			# Copy to internal directory if network
			if (fCopyAllDlls || fCopySpecified)
				{
				if(fCopyAllDlls)
					LogMessage("Copying all .DLL files to common bin")
				else
					LogMessage("copying specified .DLL files to common bin")
					
				CopyObjectAPIFiles("BIN")
				}

			if (fCopyConfigs)
				{
				LogMessage("Copying files to common config")
				CopyObjectAPIFiles("CONFIGS")
				}
				
			ChangeDir(dCWD)
			}
		}
	}
