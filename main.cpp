#include <iostream>
#include <Map.h>
#include <rng.h>
#include <Utils.h>
#include <helpers.h>

using namespace RMap;
using namespace std;


void init( Map& m );

void render( Map& m, const std::string& name = "" )
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


void make_test( Map& m )
{
  const int tdata[] =
  {
    5,4,3,1,3, 3,5,5,5,6,
    4,3,1,2,2, 2,4,4,5,5,
    1,1,2,1,2, 2,4,4,4,4,
    5,1,3,1,1, 1,1,2,3,3,
    5,2,2,2,1, 0,1,1,4,3,

    4,4,3,2,1, 1,1,2,2,3,
    4,5,4,2,2, 2,2,2,4,5,
    5,6,7,2,2, 2,2,3,2,6,
    5,6,7,3,3, 3,3,3,6,5,
    5,5,5,4,4, 2,3,4,5,5,
  };

  m.SetSize( 10, 10 );
  m.LoadEntData( tdata, 0, 0, 10, 10 );
}




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
    Map world;
    make_test( world );
    init( world );
    render( world, "Step 2 test" );
  }

  return 0;
}



////////////////////////////////////////////////
////prototypes

void GenerateDeltaMap( Map& out_m, Map& m )
{
  //out_m.SetSize( m.w() / 2 + 1, m.h() / 2 + 1 );
  out_m.SetSize( m.w()*2-1, m.h()*2-1 );

  //do dx first
  for(unsigned cy = 0; cy < m.h(); cy++)
  {
    for(unsigned cx = 0; cx < m.w(); cx++)
    {
      Cell* c0 = m.GetCell( cx, cy );
      Cell* c1 = m.GetCell( cx+1, cy );

      int t0 = c0->GetEnt( 0 )->_type_id;
      int t1 = t0;
      if(c1)
        t1 = c1->GetEnt( 0 )->_type_id;

      int dx = abs( t1 - t0 );

      out_m.NewEnt( dx, cx*2 + 1, cy*2 );
    }
  }

  //do dy next
  for(unsigned cx = 0; cx < m.w(); cx++)
  {
    for(unsigned cy = 0; cy < m.h(); cy++)
    {
      Cell* c0 = m.GetCell( cx, cy );
      Cell* c1 = m.GetCell( cx, cy+1 );

      int t0 = c0->GetEnt( 0 )->_type_id;
      int t1 = t0;
      if(c1)
        t1 = c1->GetEnt( 0 )->_type_id;

      int dy = abs( t1 - t0 );

      out_m.NewEnt( dy, cx*2, cy*2 + 1 );
    }
  }

  //now fill corners
  for(unsigned cy = 1; cy < out_m.h(); cy+=2)
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
        average += out_cells[i]->GetEnt( 0 )->_type_id;
      }

      average /= out_cells.size();

      out_m.NewEnt( average, cx, cy );
    }
  }
}



////////////////////////////////////////////////
////
////////////////////////////////////////////////
////

void init( Map& m )
{
  std::map<int, std::vector<Map*> > regions;
  ParseRegions( regions, m );

  Map delta;
  GenerateDeltaMap( delta, m );
  render(delta, "Delta map");
  /*
  for(auto rs = regions.begin(); rs != regions.end(); ++rs)
  {
    for(unsigned i = 0; i < rs->second.size(); ++i)
    {
      render( *rs->second[ i ], "Regions..." );
    }
  }
  */

  /*
  render( counted, "Counted" );

  for(auto rs = regions.begin(); rs != regions.end(); ++rs)
  {
    for(unsigned i = 0; i < rs->second.size(); ++i)
    {
      render( *rs->second[ i ] , "Regions..." );
    }
  }
  */
  
}



////////////////////////////////////////////////
////defintions
