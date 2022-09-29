#include <FOV.h>

#include <helpers.h>

#include <Map.h>
#include <Utils.h>

#include <permissive-fov-cpp.h>
#include <tcod_fov.h>

namespace RMap
{
  

/* isBlockedFunction() may be called even if a square will not be
   visited. isBlockedFunction() may be called more than once.  */
//typedef int (*isBlockedFunction)(short destX, short destY, void * context);

int IsBlockingCell(short x, short y, void* rfov)
{
  const RMap::Rfov* fov_type = (const RMap::Rfov*)rfov;
  const std::set<unsigned>& mask = fov_type->_mask;
  if( mask.empty() )
    return 0;
  
  const RMap::Cell* c = fov_type->_m.GetCell(x,y);
  if(!c)
    return 1;
  
  
  //faster than counting all ents, just check if first ent in the array exists
  if( !c->GetEnt(0) )
    return 0;
  
  if( c->HasTypeInMask( mask ) )
    return 1;
  
  
  return 0;
}

/* visitFunction() will be called at most one time. visitFunction()
   will only be called if a mask allows visitation for that square. */
//typedef void (*visitFunction)(short destX, short destY, void * context);

void VisibleCell(short x, short y, void* rfov)
{
  RMap::Rfov* fov_type = (Rfov*)rfov;
  RMap::Cell* c = fov_type->_m.GetCell(x,y);
  if(!c)
    return;
  
  //TODO: this could be used for a lighting algorithm maybe? (a cell visted multiple times is brighter)
  if( fov_type->_already_added.count( c ) )
    return;
  
  fov_type->_in_fov.push_back(c);
  fov_type->_already_added.insert( c );
}

Rfov::Rfov(Map& m)
:_m(m)
{
}

void Rfov::SetMask(const std::set<unsigned>& mask)
{
  _mask = mask;
}

void Rfov::Calculate(unsigned x, unsigned y, unsigned radius)
{
  _already_added.clear();
  _in_fov.clear();
  if( !_m.ValidCell(x,y) )
    return;
  
  if( radius == 0 )
    return;
 
  //Calculate(x,y,radius,radius,radius,radius);
  
  permissiveSquareFov(x,y,radius,IsBlockingCell, VisibleCell, this);
}

void Rfov::Calculate(unsigned x, unsigned y, unsigned radius, int type, bool see_walls)
{
  _already_added.clear();
  _in_fov.clear();
  if( !_m.ValidCell(x,y) )
    return;
  
  if( radius == 0 )
    return;
  
  map_data m_data = { IsBlockingCell, VisibleCell, this, see_walls, _m.w(), _m.h() };
  
  TCOD_compute_fov_permissive2(m_data, x, y, radius, type);
}

//TODO: fix this algorithm
/*
void Rfov::Calculate(unsigned x, unsigned y, unsigned north, unsigned east, unsigned south, unsigned west)
{
  _in_fov.clear();
  if( !_m.ValidCell(x,y) )
    return;
  
  unsigned tlx = std::max<int>(0,static_cast<int>(x) - west);
  unsigned tly = std::max<int>(0,static_cast<int>(y) - north);
  unsigned brx = x + east;
  unsigned bry = y + south;
  
  CalculateArea(x,y, tlx, tly, brx, bry);
}

void Rfov::CalculateArea(unsigned x, unsigned y, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  _in_fov.clear();
  if( !_m.ValidCell(x,y) )
    return;
  
  //invalid fov if we're outside our region
  if( x < tlx || x > brx )
    return;
  
  if( y < tly || y > bry )
    return;
  
  RUtils::FitArea(static_cast<int>(_m.w()) - 1, static_cast<int>(_m.h()) - 1, tlx, tly, brx, bry);
  
  KC_PRINT( tlx );
  KC_PRINT( tly );
  KC_PRINT( brx );
  KC_PRINT( bry );
  
  KC_PRINT( x );
  KC_PRINT( y );
  
  int north = y - tly;
  int south = bry - y;
  int east  = brx - x;
  int west  = x - tlx;
  
  KC_PRINT( north );
  KC_PRINT( south );
  KC_PRINT( east );
  KC_PRINT( west );
  
  permissiveMaskT pfov_mask;
  
  pfov_mask.north = north;
  pfov_mask.south = south;
  pfov_mask.east = east;
  pfov_mask.west = west;
  pfov_mask.width = pfov_mask.west + 1 + pfov_mask.east;
  pfov_mask.height = pfov_mask.south + 1 + pfov_mask.north;
  pfov_mask.mask = 0;
  
  permissiveFov(x,y, &pfov_mask, IsBlockingCell, VisibleCell, this);
}
*/


void Rfov::GetFOV(std::vector<Cell*>& out_cells) const
{
  out_cells = _in_fov;
}
  
  
}






































