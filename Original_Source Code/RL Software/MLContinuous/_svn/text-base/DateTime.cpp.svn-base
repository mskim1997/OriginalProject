/////////////////////////////////////////////////////////
//DateTime.cpp
//Includes the necessary headers to use the timeval struct
//and the gettimeofday function.
//
//Manuel Madera
//10/31/09
/////////////////////////////////////////////////////////

#include "DateTime.h"

void gettimeofday(struct timeval* t,void* timezone)
{       struct _timeb timebuffer;
_ftime( &timebuffer );
t->tv_sec=timebuffer.time;
t->tv_usec=1000*timebuffer.millitm;
}