#include <libtcod_int.h>
#include <libtcod.h>
#include <stdio.h>

#if defined( TCOD_VISUAL_STUDIO )
static const char *version_string = TCOD_STRVERSIONL;
#else
static const char *version_string __attribute__((unused)) ="libtcod "TCOD_STRVERSION;
#endif


void TCOD_fatal(const char *fmt, ...) {
        va_list ap;
        printf("%s\n",version_string);
        va_start(ap,fmt);
        vprintf(fmt,ap);
        va_end(ap);
        printf ("\n");
        assert(0);
}

void TCOD_fatal_nopar(const char *msg) {
        printf("%s\n%s\n",version_string,msg);
        assert(0);
}