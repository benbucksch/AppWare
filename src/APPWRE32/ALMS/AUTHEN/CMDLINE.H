#ifndef __CMDLINE_H_
#define __CMDLINE_H_

#ifdef __cplusplus
extern "C" {
#endif
extern WORD	SaveCmdLineArg(char *arg, int argNumber, PSPACE *p);
extern void	ExpandCmdLineArgs(char *dest, char *src, PSPACE *p);
#ifdef __cplusplus
}
#endif

#endif
