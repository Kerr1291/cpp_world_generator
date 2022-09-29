#pragma once
#include <set>
#include <vector>

namespace RMap
{

class Map;
class Cell;

class Rfov
{
  friend int IsBlockingCell(short x, short y, void* rfov);
  friend void VisibleCell(short x, short y, void* rfov);
public:
  
  Rfov(Map& m);
  
  //type ids inside mask are treated as things that block fov
  void SetMask(const std::set<unsigned>& mask);
  
  //sees origin
  void Calculate(unsigned x, unsigned y, unsigned radius);
  
  //does not see origin + option to see walls
  //type [0,8] --> granularity of FOV detection, 1 = more like a normal raycast while 8 = very powerful (can see around corners)
  void Calculate(unsigned x, unsigned y, unsigned radius, int type, bool see_walls = true);
  
  //TODO: fix the algorithm for these later
  /*
  void Calculate(unsigned x, unsigned y, unsigned north, unsigned east, unsigned south, unsigned west);
  void CalculateArea(unsigned x, unsigned y, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
  */
  void GetFOV(std::vector<Cell*>& out_cells) const;
  
private:
  Map& _m;
  std::set<unsigned> _mask;
  std::vector<Cell*> _in_fov;
  std::set<Cell*>    _already_added;
};


}



















