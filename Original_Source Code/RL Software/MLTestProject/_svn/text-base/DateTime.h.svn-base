/////////////////////////////////////////////////////////
//DateTime.h
//Includes the necessary headers to use the timeval struct
//and the gettimeofday function.
//
//Manuel Madera
//10/31/09
/////////////////////////////////////////////////////////

#ifndef DateTime_h
#define DateTime_h

#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#define ssize_t int
#include <io.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <winsock.h>
void gettimeofday(struct timeval* t,void* timezone);
#endif

#endif	//DateTime_h