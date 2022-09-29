/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCODLIB_INT_H
#define _TCODLIB_INT_H
#include <stdarg.h>
#include <assert.h>
/* tcodlib internal stuff */
#ifdef __cplusplus
extern "C" {
#endif


/* fov internal stuff */
typedef struct {
	bool transparent:1;
	bool walkable:1;
	bool fov:1;
} cell_t;
typedef struct {
	int width;
	int height;
	int nbcells;
	cell_t *cells;
} map_t;

/* fatal errors */
void TCOD_fatal(const char *fmt, ...);
void TCOD_fatal_nopar(const char *msg);

#if defined(__ANDROID__) && !defined(NDEBUG)
#include <android/log.h>
#ifdef printf
#undef printf
#endif
#ifdef vprintf
#undef vprintf
#endif
#define printf(args...) __android_log_print(ANDROID_LOG_INFO, "libtcod", ## args)
#define vprintf(args...) __android_log_vprint(ANDROID_LOG_INFO, "libtcod", ## args)

#ifdef assert
#undef assert
#endif
#define assert(cond) if(!(cond)) __android_log_assert(#cond, "libtcod", "assertion failed: %s", #cond)
#endif

#ifdef NDEBUG
#define TCOD_IF(x) if (x)
#define TCOD_IFNOT(x) if (!(x))
#define TCOD_ASSERT(x)
#define TCOD_LOG(x)
#else
#define TCOD_IF(x) assert(x);
#define TCOD_IFNOT(x) assert(x); if (0)
#define TCOD_ASSERT(x) assert(x)
#define TCOD_LOG(x) printf x
#endif

/* UTF-8 stuff */
//#ifndef NO_UNICODE
//wchar_t *TCOD_console_vsprint_utf(const wchar_t *fmt, va_list ap);
//int TCOD_console_print_internal_utf(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
//	TCOD_alignment_t align, wchar_t *msg, bool can_split, bool count_only);
//#endif;

#ifdef __cplusplus
}
#endif
#endif

