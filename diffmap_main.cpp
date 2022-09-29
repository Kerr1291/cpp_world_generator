#include <iostream>
#include <Map.h>
#include <rng.h>
#include <Utils.h>
#include <helpers.h>

using namespace RMap;
using namespace std;


void ParseRegions( std::map<int, std::vector<Map*> >& out_regions, Map& difficulty_map );
void ParseRegions( std::map<int, std::vector<Map*> >& out_regions, Map& difficulty_map )
{
  Map counted( difficulty_map.w(), difficulty_map.h() );
  for(unsigned cy = 0; cy < difficulty_map.h(); cy++)
  {
    for(unsigned cx = 0; cx < difficulty_map.w(); cx++)
    {
      Cell* counted_check = counted.GetCell( cx, cy );
      if(!counted_check || counted_check->GetEnt( 0 ))
        continue;

      Cell* c = difficulty_map.GetCell( cx, cy );

      int type = c->GetEnt( 0 )->_type_id;
      Map* region = new Map();

      int rx = 0;
      int ry = 0;
      difficulty_map.GetSubMapInFloodFillMatch( *region, type, cx, cy );
      region->GetRoot( rx, ry );
      counted.LoadEntDataAndReplace( *region, 9 + 24, rx, ry );

      out_regions[ type ].push_back( region );
    }
  }
}

void FillEmptyCells( Map& difficulty_map, unsigned sx, unsigned sy, unsigned ex, unsigned ey )
{
  int dx = (ex - sx >= 0 ? 1 : -1);
  int dy = (ey - sy >= 0 ? 1 : -1);

  for(unsigned cy = sy; cy < ey; cy += dy)
  {
    for(unsigned cx = sx; cx < ex; cx += dx)
    {
      Cell* c = difficulty_map.GetCell( cx, cy );

      if(c->GetEnt( 0 ))
        continue;

      //get all the non-empty cells
      std::vector<Cell*> cells_to_average;
      difficulty_map.GetAdjacentNonEmptyCells( cells_to_average, cx, cy );

      if(cells_to_average.empty())
        continue;

      int average = 1;
      for(unsigned i = 0; i < cells_to_average.size(); ++i)
      {
        average += cells_to_average[ i ]->GetEnt( 0 )->_type_id;
      }

      if(!cells_to_average.empty())
        average = average / cells_to_average.size();

      average = std::max( average, 1 );

      c->CreateEnt( average );
    }
  }
}

//Go over the map and fill any empty cells found by averaging their neighbors
void FillEmptyCells( Map& difficulty_map )
{
  for(unsigned cy = 0; cy < difficulty_map.h(); cy++)
  {
    for(unsigned cx = 0; cx < difficulty_map.w(); cx++)
    {
      Cell* c = difficulty_map.GetCell( cx, cy );

      if(c->GetEnt( 0 ))
        continue;
            
      //get all the non-empty cells
      std::vector<Cell*> cells_to_average;
      difficulty_map.GetAdjacentNonEmptyCells( cells_to_average, cx, cy );

      int average = 1;
      for(unsigned i = 0; i < cells_to_average.size(); ++i)
      {
        average += cells_to_average[ i ]->GetEnt( 0 )->_type_id;
      }

      if(!cells_to_average.empty())
        average = average / cells_to_average.size();

      average = std::max( average, 1 );

      c->CreateEnt( average );
    }
  }

  //final safety check to make sure there's no empty cells
  Cell* empty_check = difficulty_map.GetRandomEmptyCell();
  while(empty_check)
  {
    empty_check->CreateEnt( 1 );
    empty_check = difficulty_map.GetRandomEmptyCell();
  }
}



void GenerateDeltaMap( Map& out_m, Map& m )
{
  //out_m.SetSize( m.w() / 2 + 1, m.h() / 2 + 1 );
  out_m.SetSize( m.w() * 2 - 1, m.h() * 2 - 1 );

  //do dx first
  for(unsigned cy = 0; cy < m.h(); cy++)
  {
    for(unsigned cx = 0; cx < m.w(); cx++)
    {
      Cell* c0 = m.GetCell( cx, cy );
      Cell* c1 = m.GetCell( cx + 1, cy );

      int t0 = c0->GetEnt( 0 )->_type_id;
      int t1 = t0;
      if(c1)
        t1 = c1->GetEnt( 0 )->_type_id;

      int dx = abs( t1 - t0 );

      out_m.NewEnt( dx, cx * 2 + 1, cy * 2 );
    }
  }

  //do dy next
  for(unsigned cx = 0; cx < m.w(); cx++)
  {
    for(unsigned cy = 0; cy < m.h(); cy++)
    {
      Cell* c0 = m.GetCell( cx, cy );
      Cell* c1 = m.GetCell( cx, cy + 1 );

      int t0 = c0->GetEnt( 0 )->_type_id;
      int t1 = t0;
      if(c1)
        t1 = c1->GetEnt( 0 )->_type_id;

      int dy = abs( t1 - t0 );

      out_m.NewEnt( dy, cx * 2, cy * 2 + 1 );
    }
  }

  //now fill corners
  for(unsigned cy = 1; cy < out_m.h(); cy += 2)
  {
    for(unsigned cx = 1; cx < out_m.w(); cx += 2)
    {
      Cell* c0 = m.GetCell( cx, cy );

      std::vector<Cell*> out_cells;
      out_m.GetAdjacentNonEmptyCells( out_cells, cx, cy, false );

      if(out_cells.empty())
        continue;

      int average = 0;
      for(unsigned i = 0; i < out_cells.size(); ++i)
      {
        average += out_cells[ i ]->GetEnt( 0 )->_type_id;
      }

      average /= out_cells.size();

      out_m.NewEnt( average, cx, cy );
    }
  }
}



static int debug_ids = 0;

struct god_worm
{
  int px;
  int py;

  int dx;
  int dy;

  int cate;

  int d_id;
};

void init( Map& m );

void render( Map& m, const std::string& name = "" )
{
  if(!name.empty())
    std::cout << "World: " << name << "\n";

  int w = m.w();
  int h = m.h();
  for(int i = 0; i < w+2; ++i)
    cout << "=";
  cout << endl;

  for(int j = 0; j < h; ++j)
  {
    cout << "=" ;
    for(int i = 0; i < w; ++i)
    {
      if(m.GetCell( i, j )->GetEnt( 0 ))
      {
        int out = m.GetCell( i, j )->GetEnt( 0 )->_type_id;
        char out2 = out + 55;
        if(out < 10)
          cout << out;
        else
          cout << out2;
      }
      else
        cout << " ";
    }
    cout << "=" << endl;
  }

  for(int i = 0; i < w+2; ++i)
    cout << "=";
  cout << endl;
}

void render_range( Map& m, int r0, int r1, const std::string& name = "" )
{
  if(!name.empty())
    std::cout << "World: " << name << "\n";

  int w = m.w();
  int h = m.h();
  for(int i = 0; i < w + 2; ++i)
    cout << "=";
  cout << endl;

  for(int j = 0; j < h; ++j)
  {
    cout << "=";
    for(int i = 0; i < w; ++i)
    {
      if(m.GetCell( i, j )->GetEnt( 0 ))
      {
        int out = m.GetCell( i, j )->GetEnt( 0 )->_type_id;

        if(out < r0) {
          cout << " ";
          continue;
        }
        if(out > r1) {
          cout << " ";
          continue;
        }

        char out2 = out + 55;
        if(out < 10)
          cout << out;
        else
          cout << out2;
      }
      else
        cout << " ";
    }
    cout << "=" << endl;
  }

  for(int i = 0; i < w + 2; ++i)
    cout << "=";
  cout << endl;
}













////////////////////////////////////////////////
////
////////////////////////////////////////////////
////
////////////////////////////////////////////////
////

int main()
{
  RNG::SetSeed( 123 );
  {
    Map world(100,100);
    //init( world );
    //render( world, "Test 1" );
    while(true)
    {
      int i0 = 0;
      int i1 = 0;

      cout << "enter r0 (use -1 if this is the first map): "; cin >> i0;
      cout << "enter r1: "; cin >> i1;

      if(i0 == -1)
      {
        RNG::SetSeed( i1 );
        init( world );
        render( world, "Test Custom" );
        continue;
      }

      if(i1 == 0)
        break;
      render_range( world, i0, i1, "Filtered Results" );
    }
  }


  {
    Map world;
    init( world );
    render( world, "Test 2" );
  }

  {
    Map world;
    init( world );
    render( world, "Test 3" );
  }

  {
    Map world;
    init( world );
    render( world, "Test 4" );
  }

  {
    Map world;
    init( world );
    render( world, "Test 5" );
  }

  return 0;
}

////////////////////////////////////////////////
////
////////////////////////////////////////////////
////
////////////////////////////////////////////////
////

//world gen data

const int k_origin_type = 0;
const int k_map_size_x = 90;
const int k_map_size_y = 100;
const bool k_debug_render_steps = false;

std::vector< god_worm > gods;

//World gen functions
void MakeStart( Map& m );
void MakeFirstGodWorms( Map& m );
void RunGods( Map& m );
bool CheckDone( Map& m );

int Eval_NewCate( god_worm& g );
void Eval_Move( Map& m, god_worm& out_g, int dx1, int dy1, int dx2, int dy2, int pleftx, int plefty, int prightx, int prighty );
int Eval_NumSpawns( god_worm& g );

void God_New( int cate, int px, int py, int dx, int dy );
void God_Visit( Map& m, god_worm& g );
void God_Kill( god_worm g );
void God_Act( Map& m, god_worm& g );
void God_Eval( Map& m, god_worm& g );
void God_Move( Map& m, god_worm& g );
void God_Spawn( Map& m, god_worm& g );

////////////////////////////////////////////////
////
void init( Map& m )
{
  m.SetSize( k_map_size_x, k_map_size_y );

  MakeStart( m );
  MakeFirstGodWorms( m );

  //this generates a difficulty map
  int get = 0;
  while(!CheckDone( m ))
  {
    RunGods( m );

    if(k_debug_render_steps)
    {
      if(get != 53)
      {
        render( m, "building... (5 to skip)" );

        get = cin.get();
      }
    }
  }

  //this fills any empty cells (1 quadrent at a time)
  FillEmptyCells( m, m.w() / 2, m.h() / 2, 0, 0 );
  FillEmptyCells( m, m.w() / 2, m.h() / 2, m.w(), m.h() );
  FillEmptyCells( m, m.w() / 2, m.h() / 2, m.w(), 0 );
  FillEmptyCells( m, m.w() / 2, m.h() / 2, 0, m.h() );
  //final pass
  FillEmptyCells( m );

  //Render all the regions (test) //////
  std::map<int, std::vector<Map*> > regions;
  ParseRegions( regions, m );

  //debug printing
  for(auto rs = regions.begin(); rs != regions.end(); ++rs)
  {
    std::cerr << rs->second.size() << " regions of type " << rs->first << "\n";

    if(rs->first > 15)
      continue;

    for(unsigned i = 0; i < rs->second.size(); ++i)
    {
      //don't show tiny maps
      if( (*rs->second[i]).w() * (*rs->second[ i ]).h() > 16 )
        render( *rs->second[ i ], "Region" );
    }
  }
  /////////

  std::cerr << debug_ids << " gods were used to create this world.\n";
  debug_ids = 0;


  ////////////////////////////////////////////////
  ////delta map
  Map delta;
  GenerateDeltaMap( delta, m );
  render( delta, "Delta map" );
}


////////////////////////////////////////////////
////
void MakeStart( Map& m )
{
  int sx = m.w() / 2;
  int sy = m.h() / 2;
  m.NewEnt( k_origin_type, sx, sy );
}

////////////////////////////////////////////////
////
void MakeFirstGodWorms( Map& m )
{
  Cell* c = m.GetRandomCellWithEntType( k_origin_type );
  int sx = c->x();
  int sy = c->y();
  std::vector<Cell*> cells;
  m.GetAdjacentEmptyCells( cells, sx, sy, false );

  for(unsigned i = 0; i < cells.size(); ++i)
  {
    int cx = cells[ i ]->x();
    int cy = cells[ i ]->y();

    int dx = cx - sx;
    int dy = cy - sy;

    God_New( 1, cx, cy, dx, dy );
  }
}

////////////////////////////////////////////////
////
void RunGods( Map& m )
{
  const int k_num_to_run = 4;

  for(int i = 0; i < k_num_to_run; ++i)
  {
    int god_to_run = RNG::Rand( gods.size() );
    God_Visit( m, gods[ god_to_run ] );

    if(CheckDone( m ))
      return;
  }
}

////////////////////////////////////////////////
////
bool CheckDone( Map& m )
{
  if(gods.empty())
    return true;
  return false;
}


////////////////////////////////////////////////
////
int Eval_NewCate( god_worm& g )
{
  //TODO: decide if this should increment (better logic here)
  int new_cate = g.cate;
  int roll = RNG::Rand( 100 );
  if(roll < 30)
    new_cate += 1;
  else if(RNG::Rand( 100 ) > 90)
    new_cate -= 1;
  return new_cate;
}

////////////////////////////////////////////////
////
void Eval_Move( Map& m, god_worm& out_g, int dx1, int dy1, int dx2, int dy2, int pleftx, int plefty, int prightx, int prighty )
{
  //TODO: better move decision logic
  int move = RNG::Rand( 100 );
  if(move < 50)
  {
    out_g.px += out_g.dx;
    out_g.py += out_g.dy;
  }
  else if(move < 65)
  {
    out_g.px += dx1;
    out_g.py += dy1;
  }
  else if(move < 80)
  {
    out_g.px += dx2;
    out_g.py += dy2;
  }
  else if(move < 90)
  {
    out_g.px += pleftx;
    out_g.py += plefty;
  }
  else if(move < 100)
  {
    out_g.px += prightx;
    out_g.py += prighty;
  }
  else
  {
  }

  RUtils::Clamp<int>( out_g.dx, -1, 1 );
  RUtils::Clamp<int>( out_g.dy, -1, 1 );
}

////////////////////////////////////////////////
////
int Eval_NumSpawns( god_worm& g )
{
  //TODO: better spawn children logic
  int spawns = 0;
  int rspawns = RNG::Rand( 100 );
  if(rspawns < 50)
  {
    spawns = 0;
  }
  else if(rspawns < 65)
  {
    spawns = 1;
  }
  else if(rspawns < 85)
  {
    spawns = 2;
  }
  else
  {
    spawns = 3;
  }
  return spawns;
}


////////////////////////////////////////////////
////
void God_New( int cate, int px, int py, int dx, int dy )
{
  god_worm worm;
  worm.px = px;
  worm.py = py;
  worm.dx = dx;
  worm.dy = dy;
  worm.cate = cate;

  RUtils::Clamp( worm.dx, -1, 1 );
  RUtils::Clamp( worm.dy, -1, 1 );

  worm.d_id = debug_ids++;
  //std::cerr << "new god " << worm.d_id << " at " << px << "," << py << " with dir " << dx << "," << dy << "\n";

  gods.emplace_back( worm );
}

////////////////////////////////////////////////
////
void God_Visit( Map& m, god_worm& g )
{
  //std::cerr << "god: " << g.d_id << " is visiting "<<g.px <<","<<g.py <<"\n";

  Cell* c = m.GetCell( g.px, g.py );
  if(!c)
  {
    God_Kill( g );
    return;
  }

  if(c->GetNumEnts() != 0)
  {
    God_Kill( g );
    return;
  }

  God_Act( m, g );

  God_Eval( m, g );

  //std::cerr << "god: " << g.d_id << " is done visiting and is now at " << g.px << "," << g.py << "\n";
}

////////////////////////////////////////////////
////
void God_Kill( god_worm g )
{
  //std::cerr << "killing " << g.d_id << " at " <<g.px <<"," <<g.py <<"\n";

  for(unsigned i = 0; i < gods.size(); ++i)
  {
    if(gods[ i ].px == g.px && gods[ i ].py == g.py)
    {
      gods.erase( gods.begin() + i );
      i--;
      
      if(i < 0)
        i = 0;
      if(gods.empty())
        return;
    }
  }
}

////////////////////////////////////////////////
////
void God_Act( Map& m, god_worm& g )
{
  m.NewEnt( g.cate, g.px, g.py );
}

////////////////////////////////////////////////
////
void God_Eval( Map& m, god_worm& g )
{
  int new_cate = Eval_NewCate( g );
  if(new_cate < 1)
    new_cate = 1;

  God_Move( m, g );

  int spawns = Eval_NumSpawns( g );
  for(int i = 0; i < spawns; ++i)
  {
    God_Spawn( m, g );
  }

  g.cate = new_cate;
}

////////////////////////////////////////////////
////
void God_Move( Map& m, god_worm& g )
{
  bool is_card = (g.dx == 0 || g.dy == 0);

  int dx1 = 0;
  int dy1 = 0;
  int dx2 = 0;
  int dy2 = 0;

  //is moving n/s/e/w
  if(is_card)
  {
    if(g.dx == 0)
    {
      dx1 = -1;
      dy1 = g.dy;

      dx2 = 1;
      dy2 = g.dy;
    }
    else//dy==0
    {
      dx1 = g.dx;
      dy1 = -1;

      dx2 = g.dy;
      dy2 = 1;
    }
  }
  else//moving diagonally
  {
    dx1 = g.dx;
    dy1 = 0;

    dx2 = 0;
    dy2 = g.dy;
  }
  
  //perpendicular left/right
  int pleft_x = -g.dy;
  int pleft_y =  g.dx;

  int pright_x = -pleft_x;
  int pright_y = -pleft_y;

  //now that we have our 5 possible moves, choose one
  Eval_Move( m, g, dx1, dy1, dx2, dy2, pleft_x, pleft_y, pright_x, pright_y );
}

////////////////////////////////////////////////
////
void God_Spawn( Map& m, god_worm& g )
{
  Cell* c = m.GetCell( g.px - g.dx, g.py - g.dy );
  if(!c)
    return;

  int sx = c->x();
  int sy = c->y();

  std::vector<Cell*> cells;
  m.GetAdjacentEmptyCells( cells, sx, sy );

  if(cells.empty())
    return;

  for(int i = 0; i < 5; ++i)
  {
    int cell_to_use = RNG::Rand( cells.size() );
    Cell* c = cells[ cell_to_use ];
    if(c->x() == g.px && c->y() == g.py)
      continue;

    std::vector<Cell*> cells_spawn;
    m.GetAdjacentEmptyCells( cells_spawn, c->x(), c->y() );

    if(cells_spawn.empty())
      continue;

    int dir_to_go = RNG::Rand( cells_spawn.size() );
    Cell* dir_c = cells_spawn[ dir_to_go ];
    if(dir_c->x() == g.px && dir_c->y() == g.py)
      continue;
    
    int dx = dir_c->x() - c->x();
    int dy = dir_c->y() - c->y();

    God_New( g.cate, c->x(), c->y(), dx, dy );
    break;
  }
}

/*

  int r = 20;
  for(int i = 0; i < r; ++i)
  {
    Cell& c = m.GetRandomCell();
    int sx = c.x();
    int sy = c.y();
    std::vector<Cell*> cells;
    m.GetAdjacentEmptyCells( cells, sx, sy, false );
    static int in = 1;
    RMap::FillEmptyCellsWithEntType( in++, cells );
  }
*/


