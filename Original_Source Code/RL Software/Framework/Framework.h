// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FRAMEWORK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FRAMEWORK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif

// This class is exported from the delete.dll
//class FRAMEWORK_API Cdelete {
//public:
//	Cdelete(void);
//	// TODO: add your methods here.
//};

//extern FRAMEWORK_API int ndelete;

//FRAMEWORK_API int fndelete(void);