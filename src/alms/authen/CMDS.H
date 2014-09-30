/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		*************** PROTOTYPES ********************
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef __CMDS_H_
#define __CMDS_H_

#ifdef __cplusplus
extern "C" {
#endif
void CheckMailbox(PSPACE *p);
#ifdef __cplusplus
}
#endif

typedef struct ExpiredPwd
{
	WORD 	connectionID;
	char 	*password;
	int	DSbinderyServer;		/* ds server attached bindery mode SPD 47924 */
	struct ExpiredPwd *next;
} ExpiredPwd;


#ifdef DOS
	void 		*SecureMalloc(unsigned int size);
	char 	*SecureStrdup(const char *s);
#endif
int		AttachCommand(char *server, char *user, char *pwd, PSPACE *p);
int		HomeServer(char *server, char *user, PSPACE *p);
int		ChangeContext(char *contextString, PSPACE *p);
int		ComSpecCommand(char *spec, WORD drive, char *originalLine, PSPACE *p);
char 	*GetComspecPtr(void);
int 		DisplayFileCommand(char *name, int binaryMode, char *originalLine, PSPACE *p);
int 		ShowLastLoginTime(PSPACE *p);
void 		UpdateLoginTime(WORD connID, char *name);
void 		DOSBreakCommand(int on);
int 		DOSSetCommand(char *name, char *value, int masterEnvFlag, PSPACE *p);
int 		DriveCommand(int letter, PSPACE *p);
void 		ExitCommand(char *string,PSPACE *p);
int 		ExternalCommand(char *name, char *args, PSPACE *p);
void 		FirePhasersCommand(int count);
int 		MachineNameCommand(char *name, PSPACE *p);
int 		PauseCommand(PSPACE *p);
void 		LoadExitString(void);
void 		PointToCommandShell(void);
int MapCommand(char *cmdString,PSPACE *p);
void BreakCommand(int onOff);

#endif
