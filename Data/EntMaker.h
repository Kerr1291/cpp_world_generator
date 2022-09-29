#pragma once
namespace RType
{
  struct Ent;
}

namespace RMaker
{
  ///Private
  unsigned GetUniqueID();
  void FreeUniqueID(unsigned id);
  bool AddGameID(unsigned id);
  void ClearUniqueIDMap();
  ///
  
  ///Public
  void MakeNewMapEnt(RType::Ent& out, unsigned type_id);
  void LoadMapEnt(RType::Ent& out, unsigned type_id, unsigned game_id);
  
  void GetNewID(RType::Ent& out, bool ignore_old_id=false);
  
  void SetMapEntIndex(RType::Ent& out, int index);
 
  void UnloadEnt(RType::Ent& out);
  void UnloadAllMapEnt();
  
  float GetCellExitCostEffect(int type_id, const RType::Ent& ent);
  float GetCellEnterCostEffect(int type_id, const RType::Ent& ent);
  ///
}