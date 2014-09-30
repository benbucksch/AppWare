# Release.AWK
#

global  dHomeDir
global  dReleaseDir			

global  copyList

global	szLogFile				# logfile to record to:  -l or -a option
global  szReleaseFile
global  szFilesFile
global	fLogAppend			# append to logfile:  -a option
global	fLogCleared
global	fAppend
global  buildNumList
global  gBuildNumber            = "BUILD_NUMBER"
global  latestBuild;
global  archiveList;
global  numArchives             = 0;
global  APIList;
global  LIBList;
global  CONFIGList;
global  EXEList;
global  DLLList;
global  buildPrefix 						= "BLD"
global  maxArchives             = 2;
global  g_latestDir         		= "latest"
global  g_archiveDir            = "archive"					
global  g_latestNum             = -1;
global  g_archiveFlag           = 1;

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

function InitRelease()
	{
  local filePath;
  
  if(dHomeDir == "")
  	dHomeDir = "O:"
    
  if(dReleaseDir == "")
  	dReleaseDir = "O:"
    
	szReleaseFile			= dHomeDir "\\environ\\envmake\\release.lst"
  szFilesFile       = dHomeDir "\\environ\\envmake\\relFiles.lst"
	
  #put all the build numbers is an array
	while ((getline < szReleaseFile)>0)
		{
		if ($0 != "" && substr($0, 1, 1) != "#")
    	{
      if(toupper($1) == "BLDNUM")
      	{
        #replace forward slashes with single back slashes
        gsub(/\//, "\\", $2)
        
        #replace any double slashes with single slashes
        gsub(/\\\\/, "\\", $2);
        
        filePath = dHomeDir "\\" $2;
        
        #this code is also in makeenv.awk
			  while((getline < filePath) > 0)
			  	{
					if (toupper($1) == "#DEFINE")
						{
			    	if(toupper($2) == toupper(gBuildNumber))
			      	{
			      	#retrieve the number which is contained inside the parenthesies        
			        buildNumList[filePath] = substr($3, 2, length($3)-2);
			        }        
			    	}
			    }
      
        close(filePath)
        }
      else if(toupper($1) == "LATEST")
      	latestBuild = toupper($2);
      else if(toupper($1) == "ARCHIVE")
      	{
        #sort by the build number which follows the buildPrefix
        #i.e. BLD###
        archiveList[substr($2, length(buildPrefix)+1)] = toupper($2);
        numArchives++;
        }
      }
    }
    
  close(szReleaseFile);
	}


################################################################
#
#
#
################################################################

function VerifyRelease()
	{
  local bldFile;
  local ok = 1;
  
  for(bldFile in buildNumList)
  	{
    if(g_latestNum < 0)
    	g_latestNum = buildNumList[bldFile];
      
    if(g_latestNum != buildNumList[bldFile])
    	{
      ok = 0;
      LogMessage("Release Error: The build numbers do not match!!!");
      break;
      }
    }
  

	return ok;
	}

################################################################
#
#
#
################################################################

function BuildFilesList()
	{
  local ok = 1;
  
  while(ok && (getline < szFilesFile) > 0)
  	{	
		if ($0 != "" && substr($0, 1, 1) != "#")
    	{
    	if(toupper($1) == "API")
      	{
        if(toupper($2))
        	{
          if($3)
          	{
            APIList[$2][toupper($3)];
            
            if($4)
              APIList[$2][toupper($4)];
            	 
            }
          else
          	{
            ok = 0;
            LogMessage("Release Error: No destination listed for API " $2 "in " szFilesFile);
        
            }
          }
        else
        	{
          ok = 0;
          LogMessage("Release Error: Bad API filename in " szFilesFile);
          }
        }
        
    	else if(toupper($1) == "LIB")
      	{
        if(toupper($2))
        	{
          if($3)
          	{
            LIBList[$2][toupper($3)];
            
            if($4)
              LIBList[$2][toupper($4)];
            	 
            }
          else
          	{
            ok = 0;
            LogMessage("Release Error: No destination listed for LIB " $2 "in " szFilesFile);
        
            }
          }
        else
        	{
          ok = 0;
          LogMessage("Release Error: Bad LIB filename in " szFilesFile);
          }
        }
    	
    	else if(toupper($1) == "CONFIG")
      	{
        if(toupper($2))
        	{
          if($3)
          	{
            CONFIGList[$2][toupper($3)];
            
            if($4)
              CONFIGList[$2][toupper($4)];
            	 
            }
          else
          	{
            ok = 0;
            LogMessage("Release Error: No destination listed for CONFIG " $2 "in " szFilesFile);
        
            }
          }
        else
        	{
          ok = 0;
          LogMessage("Release Error: Bad CONFIG filename in " szFilesFile);
          }
        }
    	
    	else if(toupper($1) == "EXE")
      	{
        if(toupper($2))
        	{
          if($3)
          	{
            EXEList[$2][toupper($3)];
            
            if($4)
              EXEList[$2][toupper($4)];
            	 
            }
          else
          	{
            ok = 0;
            LogMessage("Release Error: No destination listed for EXE " $2 "in " szFilesFile);
        
            }
          }
        else
        	{
          ok = 0;
          LogMessage("Release Error: Bad EXE filename in " szFilesFile);
          }
        }
    	
    	else if(toupper($1) == "DLL")
      	{
        if(toupper($2))
        	{
          if($3)
          	{
            DLLList[$2][toupper($3)];
            
            if($4)
              DLLList[$2][toupper($4)];
            	 
            }
          else
          	{
            ok = 0;
            LogMessage("Release Error: No destination listed for DLL " $2 "in " szFilesFile);
        
            }
          }
        else
        	{
          ok = 0;
          LogMessage("Release Error: Bad DLL filename in " szFilesFile);
          }
        }
      else
      	{
        ok=0;
        LogMessage("Release Error: Bad type name listed in " szFilesFile);
        }
    	}    	
    }
  
  return ok
  }
################################################################
#
#
#
################################################################
function MakeDir(dir)
	{
  local mode;
  
  mode = filemode(dir)
  
  #if the directory doesn't exist, make it
	if(index(mode, "d") == 0)
  	ExecuteCmd("md " dir);
	}
  
################################################################
#
#
#
################################################################
function PurgeDir(dir)
	{
  MakeDir(dir)
  ExecuteCmd("attrib -r " dir "\\*.*");
  ExecuteCmd("echo y | del " dir "\\*.*");
  }

################################################################
#
#
#
################################################################
function CleanArchive()
	{
  local num;
  local dir;
  
  MakeDir(dReleaseDir "\\" g_archiveDir)
  
  if(numArchives >= maxArchives)
  	{
    for(num in archiveList)	#get the first in the sorted list
    	{
      dir = dReleaseDir "\\" g_archiveDir "\\" archiveList[num];
      ExecuteCmd("deltree /Y " dir);
      delete archiveList[num];
      break; 
      }
    }
  }
  
################################################################
#
#
#
################################################################

function LatestToArchive()
	{
	local latDir;
  local archiveDir;
  
  if(latestBuild != "")
  	{
    latDir = dReleaseDir "\\" g_latestDir;
    archiveDir = dReleaseDir "\\" g_archiveDir "\\" latestBuild;
    MakeDir(dReleaseDir "\\" g_archiveDir)
    ExecuteCmd("move " latDir " " archiveDir);
    
    archiveList[ substr(latestBuild, length(buildPrefix)+1)] = latestBuild;
    latestBuild = 0;
    }
	}

################################################################
#
#
#
################################################################

function ReleaseToLatest()
	{
  local intSrcDir;
  local intDstDir;
  local shipSrcDir;
  local shipDstDir;
  local debugSrcDir;
  local debugDstDir;
  local fileName;
  local type;
  local srcFile;
  local dstFile;
  
  MakeDir(dReleaseDir "\\" g_latestDir);
  latestBuild = buildPrefix g_latestNum;
  
  dReleaseDir = dReleaseDir "\\latest";
  MakeDir(dReleaseDir);
  MakeDir(dReleaseDir "\\ship")
  MakeDir(dReleaseDir "\\release")
  
	#copy APIs
  intSrcDir = dHomeDir "\\internal\\include"
  intDstDir = dReleaseDir "\\release\\include"
	shipSrcDir = dHomeDir "\\internal\\include"
  shipDstDir = dReleaseDir "\\ship\\include"
  
  PurgeDir(intDstDir)
  PurgeDir(shipDstDir)
  
  for(fileName in APIList)
  	{
  	for(type in APIList[fileName])
    	{
      if(type == "INTERNAL")
      	{
        srcFile = intSrcDir "\\" fileName
        dstFile = intDstDir
        }
      else if(type == "SHIP")
      	{
        srcFile = shipSrcDir "\\" fileName
        dstFile = shipDstDir;
        }
      else
      	LogMessage("Release Error: Bad file type for APIs.")
        
      ExecuteCmd("copy " srcFile " " dstFile);
      }
  	}
	
	ExecuteCmd("attrib +r " intDstDir "\\*.*")
	ExecuteCmd("attrib +r " shipDstDir "\\*.*")

  #copy LIBs
  intSrcDir = dHomeDir "\\internal\\lib"
  intDstDir = dReleaseDir "\\release\\lib"
	shipSrcDir = dHomeDir "\\internal\\lib\\ship"
  shipDstDir = dReleaseDir "\\ship\\lib"
  
  PurgeDir(intDstDir)
  PurgeDir(shipDstDir)
  
  for(fileName in LIBList)
  	{
  	for(type in LIBList[fileName])
    	{
      if(type == "INTERNAL")
      	{
        srcFile = intSrcDir "\\" fileName
        dstFile = intDstDir
        }
      else if(type=="SHIP")
      	{
        srcFile = shipSrcDir "\\" fileName
        dstFile = shipDstDir;
        }
      else
      	LogMessage("Release Error: Bad file type for LIBs.")
      
        
      ExecuteCmd("copy " srcFile " " dstFile);
      }
  	}
	
	ExecuteCmd("attrib +r " intDstDir "\\*.*")
	ExecuteCmd("attrib +r " shipDstDir "\\*.*")

	#copy CONFIGs
  intSrcDir = dHomeDir "\\internal\\config"
  intDstDir = dReleaseDir "\\release\\config"
	shipSrcDir = dHomeDir "\\internal\\config"
  shipDstDir = dReleaseDir "\\ship\\config"
  
  PurgeDir(intDstDir)
  PurgeDir(shipDstDir)
  
  for(fileName in CONFIGList)
  	{
  	for(type in CONFIGList[fileName])
    	{
      if(type == "INTERNAL")
      	{
        srcFile = intSrcDir "\\" fileName
        dstFile = intDstDir
        }
      else if(type=="SHIP")
      	{
        srcFile = shipSrcDir "\\" fileName
        dstFile = shipDstDir;
        }
      else
      	LogMessage("Release Error: Bad file type for CONFIGs.")
        
      ExecuteCmd("copy " srcFile " " dstFile);
      }
  	}
	
	ExecuteCmd("attrib +r " intDstDir "\\*.*")
	ExecuteCmd("attrib +r " shipDstDir "\\*.*")
	
	#copy ENVIRONMENT
  intSrcDir = dHomeDir "\\internal\\bin"
  intDstDir = dReleaseDir "\\release\\bin"
	shipSrcDir = dHomeDir "\\internal\\bin\\ship"
  shipDstDir = dReleaseDir "\\ship\\bin"
  
  PurgeDir(intDstDir)
  PurgeDir(shipDstDir)
  
  for(fileName in EXEList)
  	{
  	for(type in EXEList[fileName])
    	{
      if(type == "INTERNAL")
      	{
        srcFile = intSrcDir "\\" fileName
        dstFile = intDstDir
        }
      else if(type=="SHIP")
      	{
        srcFile = shipSrcDir "\\" fileName
        dstFile = shipDstDir;
        }
      else
      	LogMessage("Release Error: Bad file type for EXEs.")
        
      ExecuteCmd("copy " srcFile " " dstFile);
      }
  	}
	
	ExecuteCmd("attrib +r " intDstDir "\\*.*")
	ExecuteCmd("attrib +r " shipDstDir "\\*.*")
	
	#copy DLLs
  intSrcDir = dHomeDir "\\internal\\bin"
  intDstDir = dReleaseDir "\\release\\bin"
	shipSrcDir = dHomeDir "\\internal\\bin\\ship"
  shipDstDir = dReleaseDir "\\ship\\bin\\nodebug"
  debugSrcDir = dHomeDir "\\internal\\bin\\debug"
  debugDstDir = dReleaseDir "\\ship\\bin\\debug"
  
  #Don't purge the internal destination since it was purged when
  #we copied the EXEList.
  PurgeDir(shipDstDir)
  PurgeDir(debugDstDir)
  
  for(fileName in DLLList)
  	{
  	for(type in DLLList[fileName])
    	{
      if(type == "INTERNAL")
      	{
        srcFile = intSrcDir "\\" fileName
        dstFile = intDstDir
     		ExecuteCmd("copy " srcFile " " dstFile);
        }
      else if(type=="SHIP")
      	{
        srcFile = shipSrcDir "\\" fileName
        dstFile = shipDstDir;
     		ExecuteCmd("copy " srcFile " " dstFile);
     		
     		srcFile = debugSrcDir "\\" fileName;
        dstFile = debugDstDir;
     		ExecuteCmd("copy " srcFile " " dstFile);
        }
      else
      	LogMessage("Release Error: Bad file type for DLLs.")
        
      }
  	}
	
	ExecuteCmd("attrib +r " intDstDir "\\*.*")
	ExecuteCmd("attrib +r " shipDstDir "\\*.*")
	}
		  
################################################################
#
#
#
################################################################

function RebuildReleaseFile()
	{
  local path;
  local bld;
  local currentDir;
  
  LogMessage("*** Rebuilding " szReleaseFile);
  ExecuteCmd("attrib -r "szReleaseFile);
  fopen(szReleaseFile, "w")
  
  #write the build number files
  for(path in buildNumList)
  	{
    #remove the home directory and backslash from the path
  	path = 	substr(path, length(dHomeDir) + 2)
  	fwrite(sprintf("%s\t%s\n", "BLDNUM", path), szReleaseFile)
  	}
  
  #write the latest build
  fwrite(sprintf("%s\t%s\n", "LATEST", latestBuild), szReleaseFile) 
  
  #write the archive
  for(bld in archiveList)
  	{
    fwrite(sprintf("%s\t%s\n", "ARCHIVE", archiveList[bld]), szReleaseFile) 
    }
  
  close(szReleaseFile)
  ExecuteCmd("attrib +r "szReleaseFile)  
  }

################################################################
#
#
#
################################################################

function ReleaseStuff()
	{
  if(g_archiveFlag)
  	{
  	CleanArchive();
		LatestToArchive();
    }
    
	ReleaseToLatest();
	
  if(g_archiveFlag)
  	{
		RebuildReleaseFile(); 
		}   
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
    else if (substr(ARGV[opt], 1, 3) == "-c-")
    	g_archiveFlag = 0;
    else if (substr(ARGV[opt], 1, 2) == "-h")
    	dHomeDir = substr(ARGV[opt], 3)
    else if (substr(ARGV[opt], 1, 2) == "-r")
    	dReleaseDir = substr(ARGV[opt], 3)
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
			fHelp = 1
			}
		}

	if (fHelp)
		{
		printf("Usage:  copyenv [-?] [options]\n\n")
		printf("    Options:\n")
		printf("      -?:           display this message\n")
		printf("      -alogfile:    append output to logfile (keeps old)\n")
    printf("      -hdirectory:  home directory\n")
		printf("      -llogfile:    record output to logfile (destroys old)\n")
    printf("      -rdirectory:  release directory\n")
    printf("      -c-:          don't copy to archive\n")
		}

	else if (!fError)
		{
		InitRelease()
    
    if(VerifyRelease())
    	{
      if(BuildFilesList())
      	{
        ReleaseStuff()
        }
      }    
		}
	}
