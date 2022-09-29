#pragma once



/* isBlockedFunction() may be called even if a square will not be
   visited. isBlockedFunction() may be called more than once.  */
typedef int (*isBlockedFunction)(short destX, short destY, void* userdata);

/* visitFunction() will be called at most one time. visitFunction()
   will only be called if a mask allows visitation for that square. */
typedef void (*visitFunction)(short destX, short destY, void* userdata);

struct map_data
{
  isBlockedFunction isBlocked;
  visitFunction visit;
  void* userdata;
  
  bool see_blocking_objects;
  
  int w;
  int h;
};

void TCOD_compute_fov_permissive2(map_data& m_data, int player_x, int player_y, int max_radius, int fovType);