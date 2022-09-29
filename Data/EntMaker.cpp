#include <EntMaker.h>
#include <Types.h>
#include <set>
namespace RMaker
{
  
std::set<unsigned>& UniqueIDMap()
{
  static std::set<unsigned> guidmap;
  return guidmap;
}

unsigned GetUniqueID()
{
  unsigned next = 1;
  
  if( !UniqueIDMap().empty() )
  {
    next = *UniqueIDMap().rbegin();
    next += 1;
  }
  
  UniqueIDMap().insert( next );
  return next;
}

void FreeUniqueID(unsigned id)
{
  if( UniqueIDMap().count( id ) )
  {
    UniqueIDMap().erase( UniqueIDMap().find(id) );
  }
}

bool AddGameID(unsigned id)
{
  if( UniqueIDMap().count( id ) )
    return false;
  UniqueIDMap().insert( id );
  return true;
}

void ClearUniqueIDMap()
{
  UniqueIDMap().clear();
}


void MakeNewMapEnt(RType::Ent& out, unsigned type_id)
{
  out._type_id = type_id;
  out._game_id = GetUniqueID();
  out._map_index = -1;
}

void LoadMapEnt(RType::Ent& out, unsigned type_id, unsigned game_id)
{
  out._type_id = type_id;
  out._game_id = game_id;
  
  //TODO: what to do when we try to load a game id that already exists?
  AddGameID( game_id );
  
  out._map_index = -1;
}

void GetNewID(RType::Ent& out, bool ignore_old_id)
{
  if( out._type_id < 0 )
    return;
  
  if( !ignore_old_id && out._game_id >= 0 )
    FreeUniqueID( out._game_id );
  
  out._game_id = GetUniqueID();
}

void SetMapEntIndex(RType::Ent& out, int index)
{
  out._map_index = index;
}

void UnloadEnt(RType::Ent& out)
{
  FreeUniqueID( out._game_id );
  out._game_id = -1;
  out._type_id = -1;
  out._map_index = -1;
}

void UnloadAllMapEnt()
{
  ClearUniqueIDMap();
  
  //TODO: force all non-map ents to request new unique ids
}

//zero on exit means no cost to exit
float GetCellExitCostEffect(int type_id, const RType::Ent& ent)
{
  //if the entity is -1 then it's invalid, so ignore it
  if( ent._type_id < 0 )
    return 0.0f;
  
  //TODO: request the exit cost between type ids
  
  //by default all cells are free to walk out of
  return 0.0f;
}

//zero on exit means no effect to cost to enter
float GetCellEnterCostEffect(int type_id, const RType::Ent& ent)
{
  //if the entity is -1 then it's invalid, so ignore it
  if( ent._type_id < 0 )
    return 0.0f;
  
  //TODO: request the enter cost between type ids
    
    //NOTE: hard coded for testing
  if( ent._type_id == 2 )
    return 5.0f;
  
  //by default, non-empty cells are walls
  return 100.0f;
}

}
