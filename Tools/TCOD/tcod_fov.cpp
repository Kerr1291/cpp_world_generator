#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libtcod.h>
#include <libtcod_int.h>
#include <tcod_list.h>
#include <tcod_fov.h>











/* The size of each square in units */
#define STEP_SIZE 16

/* Jonathon Duerig enhanced permissive FOV */
typedef struct {
        int xi,yi,xf,yf;
} line_t;

/* Defines the parameters of the permissiveness */
/* Derived values defining the actual part of the square used as a range. */
static int offset;
static int limit;

typedef struct _viewbump_t {
        int x,y;
        int refcount;
        struct _viewbump_t *parent;
} viewbump_t;

typedef struct {
        line_t shallow_line;
        line_t steep_line;
        viewbump_t *shallow_bump;
        viewbump_t *steep_bump;
} view_t;

static view_t **current_view=NULL;
static view_t *views=NULL;
static viewbump_t *bumps=NULL;
static int bumpidx=0;

#define RELATIVE_SLOPE(l,x,y) (((l)->yf-(l)->yi)*((l)->xf-(x)) - ((l)->xf-(l)->xi)*((l)->yf-(y)))
#define BELOW(l,x,y) (RELATIVE_SLOPE(l,x,y) > 0)
#define BELOW_OR_COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) >= 0)
#define ABOVE(l,x,y) (RELATIVE_SLOPE(l,x,y) < 0)
#define ABOVE_OR_COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) <= 0)
#define COLINEAR(l,x,y) (RELATIVE_SLOPE(l,x,y) == 0)
#define LINE_COLINEAR(l,l2) (COLINEAR(l,(l2)->xi,(l2)->yi) && COLINEAR(l,(l2)->xf,(l2)->yf))


    
    
static bool is_blocked(map_data& m_data, int startX, int startY, int x, int y, int dx, int dy) 
{
  int posx=x*dx/STEP_SIZE+startX;
  int posy=y*dy/STEP_SIZE+startY;
  bool blocked = m_data.isBlocked(posx, posy, m_data.userdata);
  
  if( blocked && m_data.see_blocking_objects )
    m_data.visit(posx, posy, m_data.userdata);
  else
    m_data.visit(posx, posy, m_data.userdata);
  
  return blocked;
}

static void add_shallow_bump(int x, int y, view_t *view) 
{
  viewbump_t *shallow, *curbump;
  view->shallow_line.xf=x;
  view->shallow_line.yf=y;
  shallow= &bumps[bumpidx++];
  shallow->x=x;
  shallow->y=y;
  shallow->parent=view->shallow_bump;
  view->shallow_bump=shallow;
  curbump=view->steep_bump;
  while ( curbump ) {
          if ( ABOVE(&view->shallow_line,curbump->x,curbump->y)) {
                  view->shallow_line.xi=curbump->x;
                  view->shallow_line.yi=curbump->y;
          }
          curbump=curbump->parent;
  }
}

static void add_steep_bump(int x, int y, view_t *view) 
{
  viewbump_t *steep, *curbump;
  view->steep_line.xf=x;
  view->steep_line.yf=y;
  steep=&bumps[bumpidx++];
  steep->x=x;
  steep->y=y;
  steep->parent=view->steep_bump;
  view->steep_bump=steep;
  curbump=view->shallow_bump;
  while ( curbump ) {
          if ( BELOW(&view->steep_line,curbump->x,curbump->y)) {
                  view->steep_line.xi=curbump->x;
                  view->steep_line.yi=curbump->y;
          }
          curbump=curbump->parent;
  }
}  

static bool check_view(TCOD_list_t active_views, view_t **it) 
{
        view_t *view=*it;
        line_t *shallow_line=&view->shallow_line;
        line_t *steep_line=&view->steep_line;
        if (LINE_COLINEAR(shallow_line,steep_line)
                && (COLINEAR(shallow_line,offset,limit) 
                || COLINEAR(shallow_line,limit,offset)) ){
/*printf ("deleting view %x\n",it); */
                /* slow ! */
                TCOD_list_remove_iterator(active_views,(void **)it);
                return false;
        }
        return true;
}
    

static void visit_coords(map_data& m_data,int startX, int startY, int x, int y, int dx, int dy, TCOD_list_t active_views ) 
{
        /* top left */
        int tlx=x, tly=y+STEP_SIZE;
        /* bottom right */
        int brx=x+STEP_SIZE, bry=y;
        view_t *view=NULL;
        while (current_view != (view_t **)TCOD_list_end(active_views)) {
                view=*current_view;
                if ( ! BELOW_OR_COLINEAR(&view->steep_line,brx,bry) ) {
                        break;
                }
                current_view++;
        }
        if ( current_view == (view_t **)TCOD_list_end(active_views) || ABOVE_OR_COLINEAR(&view->shallow_line,tlx,tly)) {
                /* no more active view */
                return;
        }
        
        if( !is_blocked(m_data,startX,startY,x,y,dx,dy) ) 
        {
          return;
        }
        
        if (  ABOVE(&view->shallow_line,brx,bry) 
                && BELOW(&view->steep_line,tlx,tly)) {
                /* view blocked */
                /* slow ! */
                TCOD_list_remove_iterator(active_views,(void **)current_view);
        } else if ( ABOVE(&view->shallow_line,brx,bry)) {
                /* shallow bump */                
                add_shallow_bump(tlx,tly,view);
                check_view(active_views,current_view);
        } else if (BELOW(&view->steep_line,tlx,tly)) {
                /* steep bump */
                add_steep_bump(brx,bry,view);
                check_view(active_views,current_view);
        } else {
                /* view splitted */
                int offset=startX+x*dx/STEP_SIZE + (startY+y*dy/STEP_SIZE)*m_data.w;
                view_t *shallower_view= & views[offset];
                int view_index=current_view - (view_t **)TCOD_list_begin(active_views);
                view_t **shallower_view_it;
                view_t **steeper_view_it;
                *shallower_view=**current_view;
                /* slow ! */
                shallower_view_it = (view_t **)TCOD_list_insert_before(active_views,shallower_view,view_index);
                steeper_view_it=shallower_view_it+1;
                current_view=shallower_view_it;
                add_steep_bump(brx,bry,shallower_view);
                if (!check_view(active_views,shallower_view_it)) steeper_view_it--;
                add_shallow_bump(tlx,tly,*steeper_view_it);
                check_view(active_views,steeper_view_it);
                if ( view_index > TCOD_list_size(active_views)) current_view=(view_t **)TCOD_list_end(active_views);
        }
}

static void check_quadrant(map_data& m_data,int startX,int startY,int dx, int dy, int extentX,int extentY) 
{
        TCOD_list_t active_views=TCOD_list_new();
        line_t shallow_line={offset,limit,extentX*STEP_SIZE,0};
        line_t steep_line={limit,offset,0,extentY*STEP_SIZE};
        int maxI=extentX+extentY,i=1;
        view_t *view= &views[startX+startY*m_data.w];

        view->shallow_line=shallow_line;
        view->steep_line=steep_line;
        view->shallow_bump=NULL;
        view->steep_bump=NULL;
        TCOD_list_push(active_views,view);
        current_view=(view_t **)TCOD_list_begin(active_views);
        while ( i  != maxI+1 && ! TCOD_list_is_empty(active_views) ) {
                int startJ=MAX(i-extentX,0);
                int maxJ=MIN(i,extentY);
                int j=startJ;
                while ( j != maxJ+1 && ! TCOD_list_is_empty(active_views) && current_view != (view_t **)TCOD_list_end(active_views) ) {
                        int x=(i - j)*STEP_SIZE;
                        int y=j*STEP_SIZE;
                        visit_coords(m_data,startX,startY,x,y,dx,dy,active_views);
                        j++;
                }
                i++;
                current_view=(view_t **)TCOD_list_begin(active_views);
        }
        TCOD_list_delete(active_views);
}


void TCOD_compute_fov_permissive2(map_data& m_data, int player_x, int player_y, int max_radius, int fovType) 
{
        int minx,maxx,miny,maxy;
        if ( (unsigned)fovType>8 ) fovType = 8;//TCOD_fatal("Bad permissiveness %d for FOV_PERMISSIVE. Accepted range is [0,8].\n",fovType);
        offset=8-fovType;
        limit=8+fovType;
        /* preallocate views and bumps */
        views=(view_t *)calloc(sizeof(view_t),m_data.w*m_data.h);
        bumps=(viewbump_t *)calloc(sizeof(viewbump_t),m_data.w*m_data.h);
        /* set the fov range */
        if ( max_radius > 0 ) {
                minx=MIN(player_x,max_radius);
                maxx=MIN(m_data.w-player_x-1,max_radius);
                miny=MIN(player_y,max_radius);
                maxy=MIN(m_data.h-player_y-1,max_radius);
        } else {
                minx=player_x;
                maxx=m_data.w-player_x-1;
                miny=player_y;
                maxy=m_data.h-player_y-1;
        }
        /* calculate fov. precise permissive field of view */
        bumpidx=0;
        check_quadrant(m_data,player_x,player_y,1,1,maxx,maxy);
        bumpidx=0;
        check_quadrant(m_data,player_x,player_y,1,-1,maxx,miny);
        bumpidx=0;
        check_quadrant(m_data,player_x,player_y,-1,-1,minx,miny);
        bumpidx=0;
        check_quadrant(m_data,player_x,player_y,-1,1,minx,maxy);
        free(bumps);
        free(views);
}
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    