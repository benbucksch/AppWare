#ifndef __FEATURES_H__
#define __FEATURES_H__

#define IS_ENABLED	== 1)
#define IS_DISABLED	== 0)
#define ENABLED		(1
#define DISABLED		(0


#ifdef LOGIN
#undef LOGIN
#define LOGIN			ENABLED
#else
#define LOGIN			DISABLED
#endif

#ifdef LOGOUT
#undef LOGOUT
#define LOGOUT		ENABLED
#else
#define LOGOUT		DISABLED
#endif

#if LOGOUT IS_ENABLED && LOGIN IS_ENABLED
COMPILE ERROR!!!
Both logout and login should never both be enabled.
#endif

#endif
