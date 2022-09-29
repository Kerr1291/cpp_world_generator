#pragma once
#include <set>
#include <vector>

namespace RMap
{

class Map;
class Cell;

struct PathTypeA;
struct PathTypeD;

    //a zero cost = non-movable
  float GetMoveCost( int type_id, const std::set<unsigned>& mask, RMap::Cell* source_cell, RMap::Cell* dest_cell );

class RPath
{
public:
  
  RPath(Map* m);
  RPath(const RPath& other);
  ~RPath();
  
  void SetMap(Map* m);
  
  //type ids inside mask are treated as things that block the path
  void SetMask(const std::set<unsigned>& mask);
  const std::set<unsigned>& GetMask() const;
  
  //get the last successfully calculated path
  void GetPath(std::vector<Cell*>& out_cells) const;
  
  //check to see if there's a new path
  //if false after a calculate call then there's no path
  bool HasNewPath() const;
  
  bool IsAStar() const;
  bool IsDijkstra() const;
  
  //set the type of pathfinding calculate will use, disables the other type
  void EnableAStar();
  void EnableDijkstra();
  
  void SetDiagonalCost(float cost);
  float GetDiagonalCost() const;
  
  //returns false if it failed to calculate any new path
  bool Calculate(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
  
  void ReversePath();
  
  void GetStart(unsigned &out_x, unsigned &out_y) const;
  void GetEnd(unsigned &out_x, unsigned &out_y) const;
  
  unsigned GetNewPathSize() const;
  
  void SetWalkerType(int type_id);
  int  GetWalkerType() const;
  
  //if we're using a dijkstra path type then this is essentially free after a Calculate() call
  //this does not work is using A* type
  //note that this will return -1.0f if the destination is unreachable
  float GetDijkstraDistance(unsigned x, unsigned y);
  
private:
  Map* _m;
  std::set<unsigned> _mask;
  std::vector<Cell*> _path;
  
  PathTypeA* _astar;
  PathTypeD* _dijkstra;
  
  float _diagonal_path_cost;
  bool  _has_new_path;
  int   _type_walking;
};


}



















