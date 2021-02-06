#ifndef MYPROJECT_H_
#define MYPROJECT_H_

#include <stdio.h>

#define DEBUG_FUNC OK

#ifdef DEBUG_FUNC 
	#define print_debug(...) printf(__VA_ARGS__)
#else
	#define print_debug(...) 
#endif

#define DEBUG_ERROR OK

#ifdef DEBUG_ERROR
	#define print_debug_error(...) fprintf(stderr, __VA_ARGS__)
#else
	#define print_debug_error(...) 
#endif



#endif
