#include <Pathfind.h>
#include <Map.h>
#include <path.hpp>
#include <helpers.h>

///Local file variables
const float k_default_diagonal_path_cost = 1.41f;

const float k_bad_map_cost = 0.0f;
const float k_bad_cell_cost = 0.0f;

const float k_min_move_enter_cost = 1.0f;
const float k_min_move_exit_cost = 0.0f;

const float k_max_move_enter_cost = 100.0f;

namespace RMap
{

    //a zero cost = non-movable
float GetMoveCost( int type_id, const std::set<unsigned>& mask, RMap::Cell* source_cell, RMap::Cell* dest_cell )
{
  float exit_cost = k_min_move_exit_cost;
  float enter_cost = k_min_move_enter_cost;
  
  if(source_cell)
    exit_cost = source_cell->GetMoveExitCost( type_id, mask );
  
  //null dest forces us to assume this is a wall
  if(!dest_cell)
    return k_bad_cell_cost;
  else
    enter_cost = dest_cell->GetMoveEnterCost( type_id, mask );
  
  //if we're moving into a walkable space, then consider the exit cost
  float move_cost = enter_cost;
  if( move_cost < k_max_move_enter_cost )
    move_cost += exit_cost;
  
  //if the cost is now >= max, then mark as non-walkable
  if( move_cost >= k_max_move_enter_cost )
    move_cost = 0;
  //if cost is zero here, that means that the cell is 100% walkable, set to minimum walk cost
  else if( move_cost <= k_min_move_enter_cost )
    move_cost = k_min_move_enter_cost;
  
  return move_cost;
}

}


///=//////////////////////////////////////////////////
/// AStar pathfinding interface types
///
#ifdef _WIN32
class AStarPathCallback : public ::ITCODPathCallback
#else
class AStarPathCallback : public ITCODPathCallback
#endif
{
public:
  AStarPathCallback(RMap::Map** m)
  :_m(m)
  {}
  
  virtual ~AStarPathCallback() {}
  virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *rpath ) const
  {
    if( !*_m )
      return k_bad_map_cost;
    
    RMap::RPath* rp = (RMap::RPath*)rpath;
    
    const int ent_type = rp->GetWalkerType();
    const std::set<unsigned>& mask = rp->GetMask();
    
    RMap::Cell* source_cell = (*_m)->GetCell(xFrom,yFrom);
    RMap::Cell* dest_cell   = (*_m)->GetCell(xTo,yTo);
    
    if( !source_cell || !dest_cell )
      return k_bad_cell_cost;
    
    float move_cost = RMap::GetMoveCost( ent_type, mask, source_cell, dest_cell );
    return move_cost;
  }
  
  RMap::Map** _m;
};

namespace RMap
{

struct PathTypeA
{
  PathTypeA(RMap::Map** m, RMap::RPath* p, float diagonal_cost)
  :_path_cb(m)
  ,_astar_path( (*m)->w(), (*m)->h(), &_path_cb, p, diagonal_cost )
  {}
  
  AStarPathCallback _path_cb;
  TCODPath          _astar_path;
};

}


///=//////////////////////////////////////////////////
/// Dijkstra pathfinding interface types
///
class DijkstraPathCallback : public ITCODPathCallback
{
public:
  DijkstraPathCallback(RMap::Map** m)
  :_m(m)
  {}
  
  virtual ~DijkstraPathCallback() {}
  virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *rpath ) const
  {
    if( !*_m )
      return k_bad_map_cost;
    
    RMap::RPath* rp = (RMap::RPath*)rpath;
    
    const int ent_type = rp->GetWalkerType();
    const std::set<unsigned>& mask = rp->GetMask();
    
    RMap::Cell* source_cell = (*_m)->GetCell(xFrom,yFrom);
    RMap::Cell* dest_cell   = (*_m)->GetCell(xTo,yTo);
    
    if( !source_cell || !dest_cell )
      return k_bad_cell_cost;
    
    float move_cost = RMap::GetMoveCost( ent_type, mask, source_cell, dest_cell );
    return move_cost;
  }
  
  RMap::Map** _m;
};

namespace RMap
{
  
struct PathTypeD
{
  PathTypeD(RMap::Map** m, RMap::RPath* p, float diagonal_cost)
  :_path_cb(m)
  ,_dijkstra_path( (*m)->w(), (*m)->h(), &_path_cb, p, diagonal_cost )
  {}
  
  DijkstraPathCallback _path_cb;
  TCODDijkstra         _dijkstra_path;
};

}




///=//////////////////////////////////////////////////
/// RPath definition
///

namespace RMap
{
 
RPath::RPath(Map* m)
:_m(m)
,_astar(0)
,_dijkstra(0)
,_diagonal_path_cost(k_default_diagonal_path_cost)
,_has_new_path(false)
,_type_walking(-1)
{
  //default pathfinding type
  EnableAStar();
}

RPath::RPath(const RPath& other)
:_m(other._m)
,_astar(0)
,_dijkstra(0)
,_diagonal_path_cost(other._diagonal_path_cost)
,_has_new_path(false)
,_type_walking(other._type_walking)
{
  if( other._astar )
    EnableAStar();
  else if( other._dijkstra )
    EnableDijkstra();
}

RPath::~RPath()
{
  delete _astar;
  delete _dijkstra;
}

void RPath::SetMap(Map* m)
{
  _m = m;
  if( _astar && *_astar->_path_cb._m != _m )
  {
    delete _astar;
    if( _m )
      _astar = new PathTypeA( &_m, this, _diagonal_path_cost );
    else
      _astar = 0;
    _has_new_path = false;
  }
  else if( _dijkstra && *_dijkstra->_path_cb._m != _m )
  {
    delete _dijkstra;
    if( _m )
      _dijkstra = new PathTypeD( &_m, this, _diagonal_path_cost );
    else
      _dijkstra = 0;
    _has_new_path = false;
  }
}

//type ids inside mask are treated as things that block the path
void RPath::SetMask(const std::set<unsigned>& mask)
{
  _mask = mask;
}

const std::set<unsigned>& RPath::GetMask() const
{
  return _mask;
}

void RPath::GetPath(std::vector<Cell*>& out_cells) const
{ 
  out_cells = _path;
} 

bool RPath::HasNewPath() const
{
  return _has_new_path;
}

bool RPath::IsAStar() const
{
  return ( _astar != 0 );
}

bool RPath::IsDijkstra() const
{
  return ( _dijkstra != 0 );
}

void RPath::EnableAStar()
{  
  if( _astar )
    return;
  
  if( !_m )
    return;
  
  if( _dijkstra )
    delete _dijkstra;
  _dijkstra = 0;
  
  _astar = new PathTypeA( &_m, this, _diagonal_path_cost );
  _has_new_path = false;
}

void RPath::EnableDijkstra()
{
  if( _dijkstra )
    return;
  
  if( !_m )
    return;
  
  if( _astar )
    delete _astar;
  _astar = 0;
  
  _dijkstra = new PathTypeD( &_m, this, _diagonal_path_cost );
  _has_new_path = false;
}

void RPath::SetDiagonalCost(float cost)
{
  if( _diagonal_path_cost < 0.0f )
    return;
  
  _diagonal_path_cost = cost;
  
  if( !_m )
    return;
  
  if( _astar )
  {
    delete _astar;
    _astar = new PathTypeA( &_m, this, _diagonal_path_cost );
    _has_new_path = false;
  }
  else if( _dijkstra )
  {
    delete _dijkstra;
    _dijkstra = new PathTypeD( &_m, this, _diagonal_path_cost );
    _has_new_path = false;
  }
}

float RPath::GetDiagonalCost() const
{
  return _diagonal_path_cost;
}

bool RPath::Calculate(unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  _has_new_path = false;
  if( !_m )
    return false;
  
  if( !_m->ValidCell(x0,y0) )
    return false;
  
  if( !_m->ValidCell(x1,y1) )
    return false;
  
  //cannot calculate a zero path (makes no sense)
  if( x0 == x1 && y0 == y1 )
    return false;
  
  if( _astar )
  {
    _has_new_path = _astar->_astar_path.compute(x0,y0, x1,y1);
    if( _has_new_path )
    {
      _path.clear();
      for (int i=0; i < _astar->_astar_path.size(); ++i )
      {
        int x,y;
        _astar->_astar_path.get(i,&x,&y);
        
        Cell* c = _m->GetCell(x,y);
        if(!c)
        {
          KC_LABEL("Invalid null cell encountered in calculated a star path at path index ",i);
          continue;
        }
        
        _path.push_back( c );
      }
    }
  }
  else if( _dijkstra )
  {
    _dijkstra->_dijkstra_path.compute(x0,y0);
    _has_new_path = _dijkstra->_dijkstra_path.setPath(x1,y1);
    if( _has_new_path )
    {
      _path.clear();
      for (int i=0; i < _dijkstra->_dijkstra_path.size(); ++i )
      {
        int x,y;
        _dijkstra->_dijkstra_path.get(i,&x,&y);
        
        Cell* c = _m->GetCell(x,y);
        if(!c)
        {
          KC_LABEL("Invalid null cell encountered in calculated dijkstra path at path index ",i);
          continue;
        }
        
        _path.push_back( c );
      }
    }
  }
  return _has_new_path;
}

void RPath::ReversePath()
{
  if( !_has_new_path )
    return;
  
  if( _astar )
    _astar->_astar_path.reverse();
  else if( _dijkstra )
    _dijkstra->_dijkstra_path.reverse();
  
  //reverse locally stored path too
  std::vector<Cell*> r_path;
  for(unsigned i = _path.size()-1; i >= 0; --i)
    r_path.push_back( _path[i] );
  _path = r_path;
}

void RPath::GetStart(unsigned &out_x, unsigned &out_y) const
{
  if( !_has_new_path )
    return;
  
  int x = 0;
  int y = 0;
  
  if( _astar )
  {
    _astar->_astar_path.getOrigin(&x, &y);
  }
  else if( _dijkstra && !_path.empty() )
  {
    out_x = _path[0]->x();
    out_y = _path[0]->y();
  }
  
  out_x = x;
  out_y = y;
}

void RPath::GetEnd(unsigned &out_x, unsigned &out_y) const
{
  if( !_has_new_path )
    return;
  
  int x = 0;
  int y = 0;
  
  if( _astar )
  {
    _astar->_astar_path.getDestination(&x, &y);
  }
  else if( _dijkstra && !_path.empty() )
  {
    const int end_index = _path.size()-1;
    out_x = _path[end_index]->x();
    out_y = _path[end_index]->y();
  }
  
  out_x = x;
  out_y = y;
}

unsigned RPath::GetNewPathSize() const
{
  if( !_has_new_path )
    return 0;
  
  if( _astar )
  {
    return _astar->_astar_path.size();
  }
  else if( _dijkstra )
  {
    return _dijkstra->_dijkstra_path.size();
  }
  return 0;
}

void RPath::SetWalkerType(int type_id)
{
  _type_walking = type_id;
}

int  RPath::GetWalkerType() const
{
  return _type_walking;
}

float RPath::GetDijkstraDistance(unsigned x, unsigned y)
{
  if( !_has_new_path )
    return -1.0f;
  
  if( _dijkstra )
    return _dijkstra->_dijkstra_path.getDistance(x,y);
  
  return -1.0f;
}
  
}