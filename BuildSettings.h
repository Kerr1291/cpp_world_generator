#pragma once


///enable if building a dll, note that this will comment out all of main.cpp
//#define LIBRARY_EXPORTS

///Server/RakNet settings

#ifdef _WIN32
#    ifdef LIBRARY_EXPORTS
#        define RSERVER_API __declspec(dllexport)
#    else
#    define RSERVER_API
//#        define RSERVER_API __declspec(dllimport)
#    endif
#else
#    define RSERVER_API
#endif

///TCOD settings

#ifdef _WIN32

	/* DLL export */
	#ifdef _WIN32
	#ifdef LIBRARY_EXPORTS
	#define TCODLIB_API __declspec(dllexport)
	#else
	#define TCODLIB_API 
	//#define TCODLIB_API __declspec(dllimport)
	#endif
	#else
	#define TCODLIB_API
	#endif

#endif