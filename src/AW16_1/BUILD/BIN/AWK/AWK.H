/* This source code is Copyright 1992, Thompson Automation, Inc.,
 * 5616 SW Jefferson, Portland, OR  97221.
 */

#ifndef string
#define string struct _string

/* String data type.
 */
struct _string {
	short reserved;
	short slen;
	char sdata[2];
	};
#endif

/* Routines defined by awk for string manipulation.
 */
extern string * awk_string_new(char *);
extern string * awk_string_init(unsigned);
extern string * awk_string_new1(char *,unsigned);
extern string * awk_string_dup(string *);
extern string * awk_string_copy(string *);
extern void awk_string_free(string *);
extern int awk_string_cmp(string *, string *);
