# MAKEENV.AWK
#
# Performs a make of the environment modules.
#
#
#

global  fOfficial               #performing an official make
global  fUseBuildPaths          # use build paths, lib, include
global  fUpdateDeps             # update dependencies:  -u option
global  nomakeProjects          # projects that should be retrieved only
global  dllProjects             # exeProjects that export a .LIB
global  exeProjects             # exeProjects that don't export a .LIB
global  projectsToBuild         # will contain projects to build
global  projectsToGet           # will contain projects to get
global  dHomeDir                # home directory above all directories
global  szLogFile
global  fLogCleared
global  fAppend
global  fGetFiles               # perform an "ss get" before compiling:  -g
global  fMakeAll                # force a remake on all targets:  -m
global  fIgnoreErrors           # ignore errors during make
global  fDebugBuild             # add DEBUG=1 to command line
global  fShipBuild              # add SHIP=1 to command line
global  fDontMake               # skip the make itself
global  fMakeDirs
global  fDeleteStuff            # only valid if we're also getting files
global  gEnvironProjFile         = "envproj.lst"
global  gEnvironPathFile        = "envpath.lst"
global  gEnvironDir             = "environ"
global  possibleBuildProjects   #projects specified by the user
global  gSrcSafeBase            = "$/appware32"
global  gBuildNumber            = "BUILD_NUMBER"
global  gBuildDate              = "BUILD_DATE"
global  buildNumberFiles
global  gDate                   = ""
global  gNewLineFile                                            = "enter.key"
global  gIncBuildNumbers         = 0
################################################################
#
#
#
################################################################

function VerifyDirectory(dir)
	{
	ExecuteCmd(sprintf("IF NOT EXIST %s MKDIR %s", dir, dir))
	}


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
#Build the project arrays
#
################################################################

function InitArys()
	{
  local prjList;
  local ok = 1;
  
  prjList = sprintf("%s\\envmake\\%s", dHomeDir, gEnvironProjFile);
  
  while((getline < prjList) > 0)
	{
		if ($0 != "" && substr($0, 1, 1) != "#")
			{
			if($2 != "")
	{       
				$1 = toupper($1)
      
		if($1 == "EXE")
		exeProjects[$2]
	else if($1 == "DLL")
		dllProjects[$2]
	else if($1 == "NOMAKE")
		nomakeProjects[$2]
	else
		ok = 0
	
	if(ok && $3 != "")
		buildNumberFiles[$2] = $3
	  
	if(ok && fGetFiles)
		projectsToGet[$2]
		
	}
      }
      
    if(!ok)
			{
			LogMessage(sprintf("Error: Invalid Project line: [ %s ] in file: %s", $0, prjList))
			break;
	}
    }
  
  close(prjList);
  
  return ok
	}



################################################################
#
#
#
################################################################

function InitMake()
	{
	local           fok  = 1;
  local           pathList;

	if (fUseBuildPaths)
		{
		if(dHomeDir == "")
			dHomeDir  = "O:"
      
    
		dHomeDir = dHomeDir "\\" gEnvironDir
    
    #eliminate possible double slash from the above statement
		gsub(/\\\\/, "\\", dHomeDir);  

    pathList = sprintf("%s\\envmake\\%s", dHomeDir, gEnvironPathFile);
  
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
			if (ENVIRON["HOMEENV"] != "")
				dHomeDir = ENVIRON["HOMEENV"]
			else
				{
				printf("Non-network make requires a  HOMEENV environment variable\n")
				fok = 0
				}
      }


	  if(fok)
	{
			dHomeDir = dHomeDir "\\" gEnvironDir
    
	#eliminate possible double slash from the above statement
			gsub(/\\\\/, "\\", dHomeDir);        
      }
      
		if (ENVIRON["INCLUDE"] == "")
			{
			printf("Non-network make requires an INCLUDE environment variable\n")
			fok = 0
			}
		
		if (ENVIRON["LIB"] == "")
			{
			printf("Non-network make requires a  LIB environment variable\n")
			fok = 0
			}
		}

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

function HandleBuildNumberFile(project, fileName)
	{
  local lDir = getcwd()
  local num=0
  
  if(gIncBuildNumbers)
  	{
	  if(gDate == "")
		{
		"type "sprintf("%s\\envmake\\%s", dHomeDir, gNewLineFile) "| date" | getline #get the date
		gDate = $NF
		}

		LogMessage(sprintf("Incrementing build info in file: %s of project: %s\n", project, fileName))
	  
	  ExecuteCmd(sprintf("ss cp %s/%s/%s/hdr",gSrcSafeBase,gEnvironDir,project))
	  ChangeDir(sprintf("%s\\%s\\hdr", dHomeDir, project))
		ExecuteCmd(sprintf("ss che %s", fileName))
		
	  #this code is also in release.awk
	  while((getline < fileName) > 0)
		{
			if (toupper($1) == "#DEFINE")
				{
		if(toupper($2) == toupper(gBuildNumber))
		{
		#retrieve the number which is contained inside the parenthesies        
		num = substr($3, 2, length($3)-2);
		}        
		}
	    }
	  
	  close(fileName)
	  
	  num = num+1
	  
	  #rebuild the file
	  fopen(fileName, "w")
	  
	  fwrite(sprintf("#define\t%s\t\"%s\"\n", gBuildNumber, num), fileName)
	  fwrite(sprintf("#define\t%s\t\"%s\"\n", gBuildDate, gDate), fileName) 
	  
	  close(fileName)
	  
		sprintf("ss dir %s", fileName) | getline
	  
	  if(toupper($1) != toupper(fileName))
		ExecuteCmd(sprintf("ss add %s \"-C %s\"", fileName, "Initial Seed file for build number")) 
		else
			ExecuteCmd(sprintf("ss update %s \"-C %s\"", fileName, "Bumped up build number"))   
		
	  #purge the buffer from the above date call
	  "ECHO \r | date" | getline
		ChangeDir(sprintf("%s",lDir))
    }
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
  local                                         buildMsg
  local           buildOpt3
  local           buildOpt4

	for (opt = 1; opt < ARGC; opt++)
		{
		if (ARGV[opt] == "-h" || ARGV[opt] == "-?" || ARGV[opt] == "?")
			{
			fHelp = 1
			break
			}
    else if (ARGV[opt] == "-b")
    	gIncBuildNumbers = 1
		else if (ARGV[opt] == "-d")
			fDebugBuild = 1
		else if (ARGV[opt] == "-s")
			fShipBuild = 1
		else if (ARGV[opt] == "-o")
			fOfficial = 1
		else if (ARGV[opt] == "-g")
			fGetFiles = 1
		else if (ARGV[opt] == "-i")
			fIgnoreErrors = 1
		else if (ARGV[opt] == "-m")
			fMakeAll = 1
    else if (ARGV[opt] == "-p")
	fUseBuildPaths = 1
		else if (ARGV[opt] == "-u")
			fUpdateDeps = 1
    else if (ARGV[opt] == "-r")
	fDeleteStuff=1
		else if (ARGV[opt] == "-x")
			fDontMake = 1
    else if (ARGV[opt] == "-md")
	fMakeDirs = 1
		else if (substr(ARGV[opt], 1, 1) != "-")
			{
			possibleBuildProjects[toupper(ARGV[opt])]
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

	if(fDeleteStuff && !fGetFiles)
	fDeleteStuff = 0;

	if (fHelp)
		{
		printf("Usage:  makeenv [-?] [options] [project] ...\n\n")
		printf("    Options:\n")
		printf("      -?:           display this message\n")
		printf("      -alogfile:    append output to logfile (keeps old)\n")
		printf("      -llogfile:    record output to logfile (destroys old)\n")
    printf("      -b            increment build numbers\n")
		printf("      -d:           perform a DEBUG build (external)\n")
		printf("      -g:           perform SS GET prior to make\n")
		printf("      -i:           ignore errors in make\n")
		printf("      -m:           force rebuild of all\n")
    printf("      -md:          makes needed directories\n")
		printf("      -o:           perform official make\n")
    printf("      -p:           Use default build paths\n")
    printf("      -r:           remove files if -g also specified\n")
		printf("      -s:           perform a SHIP build (external)\n")
		printf("      -u:           update dependencies before make\n")
		printf("      -x:           don't perform make (useful with -g)\n")
		printf("      -hdirectory:  specify home directory\n")
		}

	else if (!fError && InitMake() && InitArys())
		{
		dCWD = getcwd()

		if (!fListOfProjects)
	{
			for (project in dllProjects)
				projectsToBuild[project]
			for (project in exeProjects)
				projectsToBuild[project]
      }
		else
	{
      for (project in possibleBuildProjects)
	{
	if(project in nomakeProjects || project in dllProjects ||
		project in exeProjects)
	  {
	  projectsToBuild[project]
	  }
	else
		{
	  fError = 1
					LogMessage(sprintf("Error: Invalid project: [ %s ] specified on command line.", project))                  
	  }
	}
      
      }

		if(!fError)
	{
			if (fGetFiles)
				{
				for (project in projectsToGet)
					{
					
					if(fMakeDirs)
						VerifyDirectory(sprintf("%s\\%s", dHomeDir, project))
								
					ExecuteCmd(sprintf("ss cp " gSrcSafeBase "/" gEnvironDir "/%s", project))

					ChangeDir(sprintf("%s\\%s", dHomeDir, project))
					
	  if(fDeleteStuff)
		{
		LogMessage(sprintf("Removing all files in %s\\%s and subdirectories.", dHomeDir, project))
		ExecuteCmd(sprintf("attrib -r %s\\%s\\*.* /s", dHomeDir, project))
		ExecuteCmd(sprintf("del %s\\%s\\*.* /Q/S", dHomeDir, project))
		}
	    
					LogMessage(sprintf("Getting files from SourceSafe in project %s", project))
					ExecuteCmd("ss get * -r")                               
					}
	      
	      #don't make directories any more  
	      fMakeDirs = 0
				}

	if(fOfficial)
	{
	buildOpt4 = "OFFICIAL=1"
	
	for(project in buildNumberFiles)
		{
	  if(project in projectsToBuild)
		HandleBuildNumberFile(project, buildNumberFiles[project])
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
                                buildOpt1 = "-I"
			else
				buildOpt1 = ""

			if (fMakeAll)
                                buildOpt2 = "-A"
			else
				buildOpt2 = ""


			#first make the .lib
			#don't need to worry about updating the dependencies for making
      #the .lib files
			for (project in dllProjects)
				{
				LogMessage("********************************")
				LogMessage(sprintf("Starting make of %s", project))
	
				if(fMakeDirs)
					VerifyDirectory(sprintf("%s\\%s", dHomeDir, project))
				
				ChangeDir(sprintf("%s\\%s", dHomeDir, project))


				if (fUpdateDeps)
					{
					LogMessage(sprintf("--> Updating dependencies"))
					ExecuteCmd("updeps")
					}

				LogMessage(sprintf("--> making .LIB file for %s", project))

                ExecuteCmd(sprintf("nmake %s %s MAKELIB=1", buildOpt1, buildOpt2, buildFlags))
				}
		
            buildCmd = sprintf("nmake %s %s %s", buildOpt1, buildOpt4, buildFlags)

			for (project in projectsToBuild)
				{
				LogMessage("********************************")
				LogMessage(sprintf("Starting make of %s", project))
	
				#Only build everything if we haven't already built everything
				#while building the library file.
				if(fMakeAll && (!(project in dllProjects)))
					buildOpt2 = "-A"
				else
					buildOpt2 = ""
				
				if(project in dllProjects)
					{
				  buildOpt3 = "MAKEDLL=1"
				  buildMsg = "--> making DLL only"
				  }
				else 
					{
					if(fMakeDirs)
						VerifyDirectory(sprintf("%s\\%s", dHomeDir, project))
				  
				  buildOpt3 = ""
				  buildMsg = ""
				  }
				
				ChangeDir(sprintf("%s\\%s", dHomeDir, project))

				
				if (fUpdateDeps && !(project in dllProjects))
					{
					LogMessage(sprintf("--> Updating dependencies"))
					ExecuteCmd("updeps")
					}

				
				if (!fDontMake)
					{                                       
					if (fMakeAll)
						LogMessage(sprintf("--> Forcing rebuild of all %s", buildMsg))
					else
						LogMessage(sprintf("--> Performing make %s", buildMsg))

					ExecuteCmd(sprintf("%s %s", buildCmd, buildOpt2, buildOpt3))
					}
				}

			ChangeDir(dCWD)
			}
    }
	}
                                                                 
