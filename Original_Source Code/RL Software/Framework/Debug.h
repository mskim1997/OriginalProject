/////////////////////////////////////////////////////////
//Debug.h
//Defines the Debug directive
//
//Manuel Madera
//7/7/09
/////////////////////////////////////////////////////////

#ifndef Debug_h
#define Debug_h

namespace Framework
{
    //#define Debug
	//#define DEBUG	//Turn on debug statements from Learning algorithm only
	//#define PrintAgentPosition
	//#define DebugLinkedAgent
	//#define DebugMLNode
	//#define DebugMLAgent
	//#define DebugMessageDistributor
	//#define PrintMemoryLeak	
	//#define NullPointerVerbose
}

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <malloc.h>
#include <stdio.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define DEBUG_CALLOC(count, size) _calloc_dbg(count, size, _CLIENT_BLOCK, __FILE__, __LINE__)
#define DEBUG_FREE(p) _free_dbg(p, _NORMAL_BLOCK); *(int*)&p = 0x666;

//Must include windows.h for this macro to work correctly
#define CHECK_HEAP() \
{ \
switch( _heapchk() ) \
{ \
case _HEAPEMPTY: \
case _HEAPOK: \
	break; \
case _HEAPBADBEGIN: \
	printf( "ERROR - bad start of heap\n" ); \
	break; \
case _HEAPBADNODE: \
	printf( "ERROR - bad node in heap\n" ); \
	break; \
case _HEAPBADPTR: \
	printf( "Error - bad heap pointer\n" ); \
	break; \
default: \
	printf( "Unknown heap code\n"); \
	break; \
} \
}
#endif
#endif
#endif

#endif  //Debug_h