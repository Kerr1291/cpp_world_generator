#include <Map.h>

#include <helpers.h>

#include <rng.h>
#include <Utils.h>

#include <EntMaker.h>

#include <DataFile.h>
#include <DataFileIterator.h>

const unsigned k_min_dimension = 1;

namespace RMap
{

///Cell definition ====================================================
///Cell definition ====================================================
///Cell definition ====================================================

Cell::Cell()
:_index(unsigned(-1))
,_parent(0)
{
}

Cell::Cell(const Cell& other)
:_index(other._index)
,_parent(other._parent)
{
  _data = other._data;
}

Cell::~Cell()
{
}

unsigned Cell::x() const
{
  return (_index % _parent->w());
}

unsigned Cell::y() const
{
  return (_index / _parent->w());
}

unsigned Cell::GetMapIndex() const
{
  return _index;
}

void Cell::CreateEnt(unsigned type_id)
{
  RType::Ent ent;
  RMaker::MakeNewMapEnt(ent, type_id);
  AddEnt( ent );
}

void Cell::LoadEnt(unsigned type_id, unsigned game_id)
{
  RType::Ent ent;
  RMaker::LoadMapEnt(ent, type_id, game_id);
  AddEnt( ent );
}

bool Cell::AddEnt(RType::Ent ent)
{
  if( ent._type_id < 0 )
    return false;
  
  RMaker::SetMapEntIndex(ent, _index);
  AddEntToStorage( ent );
  return true;
}

void Cell::RemoveEntAt(unsigned index)
{
  if( index >= GetNumEnts() )
    return;
  
  RMaker::SetMapEntIndex(*GetEnt(index), -1);
  RemoveEntFromStorage( index );
}

void Cell::RemoveEnt(unsigned game_id)
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_game_id == static_cast<int>(game_id) )
    {
      RMaker::SetMapEntIndex(*GetEnt(i), -1);
      RemoveEntFromStorage( i );
      return;
    }
  }
}

void Cell::ClearEnts()
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
      RMaker::SetMapEntIndex(*GetEnt(i), -1);
  FreeEntStorage();
}


bool Cell::HasTypeInMask( const std::set<unsigned>& mask ) const
{
  if( mask.empty() )
    return false;

  for(unsigned k = 0; k < GetNumEnts(); ++k)
  {
    const unsigned ent_type_id = GetEnt(k)->_type_id;
    if( mask.count( ent_type_id ) )
      return true;
  }
  return false;
}

unsigned Cell::GetNumTypesInMask( const std::set<unsigned>& mask ) const
{
  unsigned count = 0;
  if( mask.empty() )
    return count;

  for(unsigned k = 0; k < GetNumEnts(); ++k)
  {
    const unsigned ent_type_id = GetEnt(k)->_type_id;
    if( mask.count( ent_type_id ) )
      count++;
  }
  return count;
}


int Cell::HasEntType(unsigned type_id) const
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_type_id == static_cast<int>(type_id) )
      return i;
  }
  return -1;
}

int Cell::HasEntGame(unsigned game_id) const
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_game_id == static_cast<int>(game_id) )
      return i;
  }
  return -1;
}

RType::Ent* Cell::GetEnt(unsigned index)
{
  return GetEntFromStorage(index);
}

RType::Ent* Cell::GetEntGame(unsigned game_id)
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_game_id == static_cast<int>(game_id) )
      return GetEnt(i);
  }
  return 0;
}

const RType::Ent* Cell::GetEnt(unsigned index) const
{
  return GetEntFromStorage(index);
}

const RType::Ent* Cell::GetEntGame(unsigned game_id) const
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_game_id == static_cast<int>(game_id) )
      return GetEnt(i);
  }
  return 0;
}

unsigned Cell::GetNumEnts() const
{
  return GetEntStorageCount();
}

unsigned Cell::GetNumEntsType(unsigned type_id) const
{
  unsigned count = 0;
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_type_id == static_cast<int>(type_id) )
      count+=1;
  }
  return count;
}

void Cell::GetEnts(std::vector<RType::Ent*>& out_ents)
{
  for(unsigned i = 0; i < GetNumEnts(); ++i)
    out_ents.push_back( GetEnt(i) );
}

void Cell::GetEntsOfType(std::vector<RType::Ent*>& out_ents, unsigned type_id)
{
  if( type_id < 0 )
    return;
  
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( GetEnt(i)->_type_id == static_cast<int>(type_id) )
      out_ents.push_back( GetEnt(i) );
  }
}

void Cell::GetEntsWithMask(std::vector<RType::Ent*>& out_ents, const std::set<unsigned>& mask)
{
  if( mask.empty() )
    return;
  
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( mask.count( GetEnt(i)->_type_id ) == 0 )
      out_ents.push_back( GetEnt(i) );
  }
}

void Cell::GetEntsInMask(std::vector<RType::Ent*>& out_ents, const std::set<unsigned>& mask)
{
  if( mask.empty() )
    return;
  
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( mask.count( GetEnt(i)->_type_id ) )
      out_ents.push_back( GetEnt(i) );
  }
}

float Cell::GetMoveExitCost(int type_id, const std::set<unsigned>& mask) const
{
  float exit_cost = 0.0f;
  if( mask.empty() )
    return exit_cost;
  
  std::vector<const RType::Ent*> ents;
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( mask.count( GetEnt(i)->_type_id ) )
      ents.push_back( GetEnt(i) );
  }
  
  for(unsigned i = 0; i < ents.size(); ++i)
    exit_cost += RMaker::GetCellExitCostEffect( type_id, *ents[i] );
  return exit_cost;
}

float Cell::GetMoveEnterCost(int type_id, const std::set<unsigned>& mask) const
{
  float enter_cost = 0.0f;
  if( mask.empty() )
    return enter_cost;
  
  std::vector<const RType::Ent*> ents;
  for(unsigned i = 0; i < GetNumEnts(); ++i)
  {
    if( mask.count( GetEnt(i)->_type_id ) )
      ents.push_back( GetEnt(i) );
  }
  
  for(unsigned i = 0; i < ents.size(); ++i)
    enter_cost += RMaker::GetCellEnterCostEffect( type_id, *ents[i] );
  return enter_cost;
}


///Private Cell:: methods

RType::Ent* Cell::GetEntFromStorage(unsigned index)
{
  if( index >= GetNumEnts() )
    return 0;
  return &_data[ index ];
}

const RType::Ent* Cell::GetEntFromStorage(unsigned index) const
{
  if( index >= GetNumEnts() )
    return 0;
  return &_data[ index ];
}

unsigned Cell::GetEntStorageCount() const
{
  return _data.size();
}

void Cell::FreeEntStorage()
{
  _data.clear();
}

void Cell::AddEntToStorage(const RType::Ent& ent)
{
  _data.push_back( ent );
}

void Cell::RemoveEntFromStorage(unsigned index)
{
  _data.erase( _data.begin() + index );
}




  
  
  
  
  
///Map definition ====================================================
///Map definition ====================================================
///Map definition ====================================================

Map::Map()
{
  SetRoot( 0, 0 );
  SetSize(0,0);//set to min size
}

Map::Map(unsigned w, unsigned h)
{
  SetRoot( 0, 0 );
  SetSize(w,h);
}


Map::Map(const Map& other)
{
  _rootx = other._rootx;
  _rooty = other._rooty;
  _w = other._w;
  _h = other._h;
  _layout = other._layout;
}

Map::~Map()
{
  FreeAllCells();
}

namespace DataSaveLoadKeys
{
  //only ever increment
  const unsigned k_version_number = 2;
  
  const std::string k_map_data = "MapDesc";
  
  const std::string k_map_w = "SizeWidth";
  const std::string k_map_h = "SizeHeight";
  const std::string k_version_data = "Version";
  
  const std::string k_cell_data = "MapLayout";
  
  const std::string k_cell_format = "Cell_";
  const std::string k_cell_x = "CellX";
  const std::string k_cell_y = "CellY";
  
  
  const std::string k_ent_format = "Ent_";
  
  const std::string k_typeid = "TypeID";
  const std::string k_gameid = "GameID";
}

void Map::SaveToFile(const std::string& filename)
{
  DataFile f_data;
  std::stringstream err_msg;
  
  SerializedObject& map_data = f_data.CreateOrGetObject( DataSaveLoadKeys::k_map_data );
  SerializedObject& cell_data = f_data.CreateOrGetObject( DataSaveLoadKeys::k_cell_data );
  
  map_data.SetPODValue( DataSaveLoadKeys::k_map_w, w() );
  map_data.SetPODValue( DataSaveLoadKeys::k_map_h, h() );
  
  map_data.SetPODValue( DataSaveLoadKeys::k_version_data, DataSaveLoadKeys::k_version_number );
  
  
  const unsigned max_index = GetMaxIndex();
  unsigned save_index = 0;
  for(unsigned i = 0; i < max_index; ++i)
  {
    Cell* c = GetCell(i);
    
    //don't save null cells or cells with nothing in them
    if( !c )
    {
      //should never happen, means something went really wrong
      err_msg <<"Attempted to save a null cell. Cell index: " <<i <<"\n";
      KC_WARNING( err_msg.str() );
      err_msg.str("");
      continue;
    }
    
    if( !c->GetEnt(0) )
      continue;
    
    std::stringstream cell_id_name;
    cell_id_name << DataSaveLoadKeys::k_cell_format <<save_index;
    
    SerializedObject& current_cell = cell_data.CreateOrGetObject( cell_id_name.str() );
    current_cell.SetPODValue( DataSaveLoadKeys::k_cell_x, c->x() );
    current_cell.SetPODValue( DataSaveLoadKeys::k_cell_y, c->y() );
    
    unsigned k = 0;
    RType::Ent* ent_to_save = c->GetEnt(k);
    while(ent_to_save)
    {       
      std::stringstream ent_id_name;
      ent_id_name << DataSaveLoadKeys::k_ent_format <<k;
    
      SerializedObject& ent_data = current_cell.CreateOrGetObject( ent_id_name.str() );
      
      ent_data.SetPODValue( DataSaveLoadKeys::k_typeid, ent_to_save->_type_id );
      ent_data.SetPODValue( DataSaveLoadKeys::k_gameid, ent_to_save->_game_id );
      
      //try next ent
      ++k;
      ent_to_save = c->GetEnt(k);
    }
    
    ++save_index;
  }
  
  if( f_data.Save(filename) )
  {
    err_msg <<"Successfully saved file "<<filename <<"\n";
    KC_MSG( err_msg.str() );
  }
}

bool Map::LoadFromFile(const std::string& filename)
{
  //get all the currently loaded map ents in each cell
  std::vector<RType::Ent*> current_ents;
  for( unsigned i = 0; i < GetMaxIndex(); ++i )
    GetCell(i)->GetEnts( current_ents );
  
  //now clear the map ent ids
  RMaker::UnloadAllMapEnt();
    
  DataFile f_data;
  std::stringstream err_msg;
  
  err_msg <<"Loading text data from file  "<<filename <<" ...\n";
  KC_MSG( err_msg.str() );
  err_msg.str("");
  
  if( !f_data.Load( filename ) )
  {
    err_msg <<"Failed to open file: "<<filename;
    KC_ERROR( err_msg.str() );
    return false;
  }
  
  err_msg <<"Importing loaded map data... \n";
  KC_MSG( err_msg.str() );
  err_msg.str("");
  
  SerializedObject* map_data = 0; 
  if( !f_data.GetFileObject( DataSaveLoadKeys::k_map_data, &map_data ) )
  {
    err_msg <<"Failed to find data group "<< DataSaveLoadKeys::k_map_data <<" in "<<filename;
    KC_ERROR( err_msg.str() );
    return false;
  }
  
  unsigned version = 0;
  unsigned w = 0;
  unsigned h = 0;
  SERIALIZE_VARIABLE( (*map_data), DataSaveLoadKeys::k_map_w, w );
  SERIALIZE_VARIABLE( (*map_data), DataSaveLoadKeys::k_map_h, h );
  
  SERIALIZE_VARIABLE( (*map_data), DataSaveLoadKeys::k_version_data, version );
  
  if( version < DataSaveLoadKeys::k_version_number )
  {
    err_msg <<"Saved map version in "<<filename <<" is "<<version <<" current version is "<<DataSaveLoadKeys::k_version_number;
    KC_WARNING( err_msg.str() );
    err_msg.str("");
  }
  
  if( w == 0 || h == 0 )
  {
    err_msg <<"Width or Height is 0 in "<<filename;
    KC_ERROR( err_msg.str() );
    return false;
  }
  
  SetSize(w,h);
  //const unsigned max_index = w*h;
  
  
  SerializedObject* cell_data = 0; 
  if( !f_data.GetFileObject( DataSaveLoadKeys::k_cell_data, &cell_data ) )
  {
    err_msg <<"Failed to find cell data group "<< DataSaveLoadKeys::k_cell_data <<" in "<<filename;
    KC_ERROR( err_msg.str() );
    return false;
  }
  
  std::stringstream cell_id_tostr; 
  
  ObjectIterator<SerializedObject> cell_iter( *cell_data );
  for( ; cell_iter.Good(); cell_iter.Next() )
  {
    SerializedObject& cur_cell = cell_iter.Get();
    
    /* //old way
    
    //this code parses the index out of the cell's name
    //TODO: maybe it would be faster to store the cell index as a cell member? and write it to a file
    size_t pindex_s = cur_cell.GetName().find( "_" );
    if( pindex_s == std::string::npos )
    {
      err_msg <<"Syntax error in cell name "<< cur_cell.GetName() <<" in "<<filename;
      KC_ERROR( err_msg.str() );
      return false;
    }
    
    int len = (int)cur_cell.GetName().size() - (int)(pindex_s + 1);
    std::string cell_id_str = cur_cell.GetName().substr( pindex_s + 1, (size_t)len );
    
    cell_id_tostr.str("");
    cell_id_tostr << cell_id_str;
    
    unsigned cell_index = 0;
    cell_id_tostr >> cell_index;
    
    Cell* c = GetCell(cell_index);
    
    */
    
    unsigned x = (unsigned)-1;
    unsigned y = (unsigned)-1;
  
    SERIALIZE_VARIABLE( cur_cell, DataSaveLoadKeys::k_cell_x, x )
    SERIALIZE_VARIABLE( cur_cell, DataSaveLoadKeys::k_cell_y, y )
    
    Cell* c = GetCell(x,y);
    
    if( !c )
    {
      err_msg <<"Load aborted. Cell index error. Cell "<<cur_cell.GetName() <<" at " <<x <<"," <<y <<" is invalid/outside map: "<<filename;
      KC_ERROR( err_msg.str() );
      std::cerr<<"Dumping raw cell contents from parsed data...\n";
      cur_cell.DebugPrintValues();
      SetSize(0,0);
      return false;
    }
    
    ObjectIterator<SerializedObject> ent_iter( cur_cell );
    for( ; ent_iter.Good(); ent_iter.Next() )
    {
      SerializedObject& ent_data = ent_iter.Get();
    
      int type_id = -1;
      int game_id = -1;
      
      SERIALIZE_VARIABLE( ent_data, DataSaveLoadKeys::k_typeid, type_id )
      SERIALIZE_VARIABLE( ent_data, DataSaveLoadKeys::k_gameid, game_id )
      
      c->LoadEnt( type_id, game_id );
    }
  }
  
  /* //old slow way
  
  for(unsigned cell_index = 0; cell_index < max_index; cell_index++)
  {
    Cell* c = GetCell(cell_index);
    
    SerializedObject current_cell_object;
    std::stringstream cell_id_name;
    cell_id_name << DataSaveLoadKeys::k_cell_format <<cell_index;
    
    if( !cell_data.GetInnerObject( cell_id_name.str(), current_cell_object ) )
    {
      //err_msg <<"Data not found for cell "<<cell_id_name.str() <<". Skipping load for this cell.\n";
      //KC_WARNING( err_msg.str() );
      //err_msg.str("");
      continue;
    }
    
    if( !c )
    {
      err_msg <<"Load aborted. Map dimension error. Does not match number of cells in "<<filename;
      KC_ERROR( err_msg.str() );
      SetSize(0,0);
      return false;
    }
    
    ObjectIterator<SerializedObject> ent_iter( current_cell_object );
    for( ; ent_iter.Good(); ent_iter.Next() )
    {
      SerializedObject& ent_data = ent_iter.Get();
    
      int type_id = -1;
      int game_id = -1;
      
      SERIALIZE_VARIABLE( ent_data, DataSaveLoadKeys::k_typeid, type_id )
      SERIALIZE_VARIABLE( ent_data, DataSaveLoadKeys::k_gameid, game_id )
      
      c->LoadEnt( type_id, game_id );
    }
  }
  
  */
  
  //get new unique game_ids for all the previously loaded ents
  for( unsigned i = 0; i < current_ents.size(); ++i )
  {
    RMaker::GetNewID( *current_ents[i], true );
  }
  
  err_msg <<"Successfully loaded file "<<filename <<"\n";
  KC_MSG( err_msg.str() );
  return true;
}

unsigned Map::w() const
{
  return _w;
}

unsigned Map::h() const
{
  return _h;
}

void Map::SetRoot( int x, int y )
{
  _rootx = x;
  _rooty = y;
}

void Map::GetRoot( int& out_rx, int& out_ry )
{
  out_rx = _rootx;
  out_ry = _rooty;
}

int Map::GetRootX()
{
  return _rooty;
}

int Map::GetRootY()
{
  return _rootx;
}
  
void Map::SetSize(unsigned w, unsigned h)
{  
  FreeAllCells();
  
  _w = std::max(w,k_min_dimension);
  _h = std::max(h,k_min_dimension);
  
  unsigned size = _w * _h;
  
  //uses member variables _w and _h to resize the underlying storage container
  ResizeCells();
  
  for(unsigned i = 0; i < size; ++i)
  {
    GetCell( i )->_index = i;
    GetCell( i )->_parent = this;
  }
}
  

void Map::GetAllCells(std::vector<Cell*>& out_cells)
{
  const unsigned size = GetMaxIndex();
  out_cells = std::vector<Cell*>( size );
  for(unsigned i = 0; i < size; ++i)
    out_cells[i] = GetCell(i);
}

Cell* Map::GetCell(unsigned index)
{
  return GetCellFromStorage(index);
}

Cell* Map::GetCell(unsigned x, unsigned y)
{
  if( !ValidCell(x,y) )
    return 0;
  
  const unsigned index = GetIndex(x,y);
  return GetCell(index);
}

void Map::ClearCell(unsigned index)
{
  if( !ValidCell(index) )
    return;
  GetCell( index )->ClearEnts();
}

void Map::ClearCell(unsigned x, unsigned y)
{
  const unsigned index = GetIndex(x,y);
  ClearCell(index);
}

bool Map::ValidCell(unsigned index) const
{
  if( index >= GetMaxIndex() ) 
    return false;
  return true;
}

bool Map::ValidCell(unsigned x, unsigned y) const
{
  if( x >= w() )
    return false;
  
  if( y >= h() )
    return false;
  
  return true;
}

void Map::LoadEntData(const int* data, unsigned x, unsigned y, unsigned w, unsigned h)
{
  std::set<unsigned> mask;
  LoadEntDataWithMask(data, mask, x, y, w, h);
}

void Map::LoadEntData( Map & m, unsigned x, unsigned y )
{
  std::set<unsigned> mask;
  LoadEntDataWithMask( m, mask, x, y );
}

void Map::LoadEntDataAndReplace( Map& m, unsigned type_id, unsigned x, unsigned y )
{
  if(m.w() == 0 || m.h() == 0)
    return;

  unsigned w = m.w();
  unsigned h = m.h();

  const unsigned max_y = y + h;
  const unsigned max_x = x + w;

  unsigned row = 0;
  for(unsigned j = y; j < max_y; ++j, ++row)
  {
    unsigned col = 0;
    for(unsigned i = x; i < max_x; ++i, ++col)
    {
      //look up dest cell to make sure it's valid
      Cell* c = GetCell( i, j );
      if(!c)
        continue;

      //look up source cell to make sure it's valid
      Cell* cell_to_copy = m.GetCell( col, row );
      if(!cell_to_copy)
        return;

      //if this source cell has an entity, place our replace type at dest
      if(cell_to_copy->GetEnt( 0 ))
        c->CreateEnt( type_id );
    }
  }
}


void Map::LoadEntDataWithMask(const int* data, const std::set<unsigned>& mask, unsigned x, unsigned y, unsigned w, unsigned h)
{
  if(!data)
    return;
  
  const unsigned max_y = y + h;
  const unsigned max_x = x + w;
  
  unsigned row = 0;
  for(unsigned j = y; j < max_y; ++j, ++row)
  {
    unsigned col = 0;
    for(unsigned i = x; i < max_x; ++i, ++col)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      unsigned data_index = row * w + col;
      if( data[data_index] == -1 )
        continue;
      
      if( !c->HasTypeInMask( mask ) )
        c->CreateEnt( data[data_index] );
    }
  }
}

void Map::LoadEntDataWithMask( Map& m, const std::set<unsigned>& mask, unsigned x, unsigned y )
{
  if(m.w() == 0 || m.h() == 0)
    return;

  unsigned w = m.w();
  unsigned h = m.h();

  const unsigned max_y = y + h;
  const unsigned max_x = x + w;

  unsigned row = 0;
  for(unsigned j = y; j < max_y; ++j, ++row)
  {
    unsigned col = 0;
    for(unsigned i = x; i < max_x; ++i, ++col)
    {
      //look up dest cell to make sure it's valid
      Cell* c = GetCell( i, j );
      if(!c)
        continue;

      //look up source cell to make sure it's valid
      Cell* cell_to_copy = m.GetCell( col, row );
      if(!cell_to_copy)
        return;

      //check the dest against the mask
      if(c->HasTypeInMask( mask ))
        continue;

      //copy all ents from source cell to dest cell
      for(unsigned count = 0; count < cell_to_copy->GetNumEnts(); ++count)
      {
        c->CreateEnt( cell_to_copy->GetEnt(count)->_type_id );
      }
    }
  }
}

void Map::NewEnt(unsigned type_id, Cell& c)
{
  NewEnt(type_id, c.x(), c.y());
}

void Map::NewEnt(unsigned type_id, unsigned x, unsigned y)
{
  if( !ValidCell(x,y) )
    return;

  const unsigned index = GetIndex(x,y);  
  GetCell( index )->CreateEnt( type_id );
}

void Map::MoveEnt(unsigned game_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  if( !ValidCell(x0,y0) || !ValidCell(x1,y1) )
    return;
  
  const unsigned index0 = GetIndex(x0,y0);
  const unsigned index1 = GetIndex(x1,y1);
  RType::Ent* ent = GetCell( index0 )->GetEntGame( game_id );
  if(!ent)
    return;
  
  GetCell( index0 )->RemoveEnt( game_id );
  GetCell( index1 )->AddEnt( *ent );
}

void Map::MoveEntdx(unsigned game_id, unsigned x, unsigned y, unsigned dx, unsigned dy)
{
  unsigned x1 = x+dx;
  unsigned y1 = y+dy;
  MoveEnt( game_id, x, y, x1, y1 );
}

void Map::MoveEntsOfType(unsigned type_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  if( !ValidCell(x0,y0) || !ValidCell(x1,y1) )
    return;
  
  const unsigned index0 = GetIndex(x0,y0);
  const unsigned index1 = GetIndex(x1,y1);
  std::vector<RType::Ent*> ents_to_move;
  
  for( unsigned i = 0; i < GetCell( index0 )->GetNumEnts(); ++i )
  {
    RType::Ent* ent = GetCell( index0 )->GetEnt( i );
    unsigned ent_type_id = ent->_type_id;
    if( type_id == ent_type_id )
      ents_to_move.push_back( ent );
  }
  
  for( unsigned i = 0; i < ents_to_move.size(); ++i )
  {
    GetCell( index0 )->RemoveEnt( ents_to_move[i]->_game_id );
    GetCell( index1 )->AddEnt( *ents_to_move[i] );
  }
}

void Map::MoveEntsOfTypedx(unsigned type_id, unsigned x, unsigned y, unsigned dx, unsigned dy)
{
  unsigned x1 = x+dx;
  unsigned y1 = y+dy;
  MoveEntsOfType( type_id, x, y, x1, y1 );
}

void Map::MoveEntsNotType(unsigned type_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  if( !ValidCell(x0,y0) || !ValidCell(x1,y1) )
    return;
  
  const unsigned index0 = GetIndex(x0,y0);
  const unsigned index1 = GetIndex(x1,y1);
  std::vector<RType::Ent*> ents_to_move;
  
  for( unsigned i = 0; i < GetCell( index0 )->GetNumEnts(); ++i )
  {
    RType::Ent* ent = GetCell( index0 )->GetEnt( i );
    unsigned ent_type_id = ent->_type_id;
    if( type_id != ent_type_id )
      ents_to_move.push_back( ent );
  }
  
  for( unsigned i = 0; i < ents_to_move.size(); ++i )
  {
    GetCell( index0 )->RemoveEnt( ents_to_move[i]->_game_id );
    GetCell( index1 )->AddEnt( *ents_to_move[i] );
  }
}

void Map::MoveEntsNotTypedx(unsigned type_id, unsigned x, unsigned y, unsigned dx, unsigned dy)
{
  unsigned x1 = x+dx;
  unsigned y1 = y+dy;
  MoveEntsNotType( type_id, x, y, x1, y1 );
}

void Map::MoveEnts(unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  if( !ValidCell(x0,y0) || !ValidCell(x1,y1) )
    return;
  
  const unsigned index0 = GetIndex(x0,y0);
  const unsigned index1 = GetIndex(x1,y1);
  std::vector<RType::Ent*> ents_to_move;
  
  for( unsigned i = 0; i < GetCell( index0 )->GetNumEnts(); ++i )
  {
    RType::Ent* ent = GetCell( index0 )->GetEnt( i );
    ents_to_move.push_back( ent );
  }
  
  for( unsigned i = 0; i < ents_to_move.size(); ++i )
  {
    GetCell( index0 )->RemoveEnt( ents_to_move[i]->_game_id );
    GetCell( index1 )->AddEnt( *ents_to_move[i] );
  }
}

void Map::MoveEntsdx(unsigned x, unsigned y, unsigned dx, unsigned dy)
{
  unsigned x1 = x+dx;
  unsigned y1 = y+dy;
  MoveEnts( x, y, x1, y1 );
}









void Map::GetCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id)
{
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell(i,j);
      
      if( c->HasEntType(type_id) != -1 )
        out_cells.push_back( c );
    }
  }
}

void Map::GetCellsWithMask(std::vector<Cell*>& out_cells, const std::set<unsigned>& mask)
{
  GetCellsInAreaWithMask(out_cells, mask, 0,0, w(), h());
}

void Map::GetCellsInArea(std::vector<Cell*>& out_cells, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  //force brx to be bottom right
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  int r = static_cast<int>(brx);
  int b = static_cast<int>(bry);
  
  for(int j = tly; j <= b; ++j)
  {
    for(int i = tlx; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      out_cells.push_back( c );
    }
  }
}

void Map::GetCellsInAreaWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  //force brx to be bottom right
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  int r = static_cast<int>(brx);
  int b = static_cast<int>(bry);
  
  for(int j = tly; j <= b; ++j)
  {
    for(int i = tlx; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      if( c->HasEntType(type_id) != -1  )
        out_cells.push_back( c );
    }
  }
}

void Map::GetCellsInAreaWithMask(std::vector<Cell*>& out_cells, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  int r = static_cast<int>(brx);
  int b = static_cast<int>(bry);
  
  for(int j = tly; j <= b; ++j)
  {
    for(int i = tlx; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      if( !c->HasTypeInMask( mask ) )
        out_cells.push_back( c );
    }
  }
}

void Map::GetCellsInFloodFill( std::vector<Cell*>& out_cells, unsigned type_id, unsigned x, unsigned y, const std::set<unsigned>& boundry_type_ids, bool search_diagonal )
{
  if(!ValidCell( x, y ))
    return;

  Cell& start = *GetCell( x, y );
  if(start.HasEntType( type_id ) != -1)
    return;

#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
     && visited_cells.count( c ) == 0 \
     && cells_to_fill.count( c ) == 0 ) \
     { \
       Cell& cl = *GetCell(c.first,c.second); \
       bool boundry = false; \
       for(unsigned i = 0; i < cl.GetNumEnts(); ++i) \
       { \
         unsigned type_id = cl.GetEnt(i)->_type_id; \
         if( boundry_type_ids.count( type_id ) != 0 ) { \
           boundry = true; \
           break; \
         } \
       } \
       if( !boundry ) \
         cells_to_fill.insert( c ); \
     }

  typedef std::pair<unsigned, unsigned> cell_pos;

  std::set< cell_pos > visited_cells;
  std::set< cell_pos > cells_to_fill;

  cells_to_fill.insert( cell_pos( x, y ) );

  while(!cells_to_fill.empty())
  {
    cell_pos p = *cells_to_fill.begin();

    cell_pos left = cell_pos( p.first - 1, p.second );
    cell_pos right = cell_pos( p.first + 1, p.second );
    cell_pos up = cell_pos( p.first, p.second - 1 );
    cell_pos down = cell_pos( p.first, p.second + 1 );

    M_CHECK_AND_ADD( left );
    M_CHECK_AND_ADD( right );
    M_CHECK_AND_ADD( up );
    M_CHECK_AND_ADD( down );

    if(search_diagonal)
    {
      cell_pos top_left = cell_pos( p.first - 1, p.second - 1 );
      cell_pos top_right = cell_pos( p.first + 1, p.second - 1 );
      cell_pos bot_left = cell_pos( p.first - 1, p.second + 1 );
      cell_pos bot_right = cell_pos( p.first + 1, p.second + 1 );

      M_CHECK_AND_ADD( top_left );
      M_CHECK_AND_ADD( top_right );
      M_CHECK_AND_ADD( bot_left );
      M_CHECK_AND_ADD( bot_right );
    }

    cells_to_fill.erase( p );
    visited_cells.insert( p );

    out_cells.push_back( GetCell( p.first, p.second ) );
  }

#undef M_CHECK_AND_ADD
}

void Map::GetSubMapInFloodFillMatch( Map& out_map, unsigned type_id, unsigned x, unsigned y, bool search_diagonal)
{
  if(!ValidCell( x, y ))
    return;

  Cell& start = *GetCell( x, y );
  if(start.HasEntType( type_id ) == -1)
    return;

#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
     && visited_cells.count( c ) == 0 \
     && cells_to_fill.count( c ) == 0 ) \
     { \
       Cell& cl = *GetCell(c.first,c.second); \
       bool boundry = true; \
       for(unsigned i = 0; i < cl.GetNumEnts(); ++i) \
       { \
         unsigned cell_type_id = cl.GetEnt(i)->_type_id; \
         if( type_id == cell_type_id ) { \
           boundry = false; \
           break; \
         } \
       } \
       if( !boundry ) \
         cells_to_fill.insert( c ); \
     }

  typedef std::pair<unsigned, unsigned> cell_pos;

  std::set< cell_pos > visited_cells;
  std::set< cell_pos > cells_to_fill;

  cells_to_fill.insert( cell_pos( x, y ) );

  cell_pos max = cell_pos(0,0);
  cell_pos min = cell_pos(w()-1,h()-1);

  while(!cells_to_fill.empty())
  {
    cell_pos p = *cells_to_fill.begin();

    cell_pos left = cell_pos( p.first - 1, p.second );
    cell_pos right = cell_pos( p.first + 1, p.second );
    cell_pos up = cell_pos( p.first, p.second - 1 );
    cell_pos down = cell_pos( p.first, p.second + 1 );

    M_CHECK_AND_ADD( left );
    M_CHECK_AND_ADD( right );
    M_CHECK_AND_ADD( up );
    M_CHECK_AND_ADD( down );

    if(search_diagonal)
    {
      cell_pos top_left = cell_pos( p.first - 1, p.second - 1 );
      cell_pos top_right = cell_pos( p.first + 1, p.second - 1 );
      cell_pos bot_left = cell_pos( p.first - 1, p.second + 1 );
      cell_pos bot_right = cell_pos( p.first + 1, p.second + 1 );

      M_CHECK_AND_ADD( top_left );
      M_CHECK_AND_ADD( top_right );
      M_CHECK_AND_ADD( bot_left );
      M_CHECK_AND_ADD( bot_right );
    }

    cells_to_fill.erase( p );
    visited_cells.insert( p );

    if(p.first < min.first)
      min.first = p.first;
    if(p.second < min.second)
      min.second = p.second;

    if(p.first > max.first)
      max.first = p.first;
    if(p.second > max.second)
      max.second = p.second;
  }

  int shiftx = min.first;
  int shifty = min.second;
  int sizew = max.first - min.first;
  int sizeh = max.second - min.second;

  out_map.SetSize( sizew + 1, sizeh + 1 );

  out_map.SetRoot( shiftx, shifty );

  for(auto iter = visited_cells.begin(); iter != visited_cells.end(); ++iter)
  {
    out_map.NewEnt( type_id, iter->first - shiftx, iter->second - shifty );
  }

#undef M_CHECK_AND_ADD
}

void Map::FloodFillEntType(unsigned type_id, unsigned x, unsigned y, const std::set<unsigned>& boundry_type_ids, bool search_diagonal)
{
  if( !ValidCell(x,y) )
    return;
  
  Cell& start = *GetCell(x,y);
  if( start.HasEntType( type_id ) != -1 )
    return;
  
#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
     && visited_cells.count( c ) == 0 \
     && cells_to_fill.count( c ) == 0 ) \
     { \
       Cell& cl = *GetCell(c.first,c.second); \
       bool boundry = false; \
       for(unsigned i = 0; i < cl.GetNumEnts(); ++i) \
       { \
         unsigned type_id = cl.GetEnt(i)->_type_id; \
         if( boundry_type_ids.count( type_id ) != 0 ) { \
           boundry = true; \
           break; \
         } \
       } \
       if( !boundry ) \
         cells_to_fill.insert( c ); \
     }
  
  typedef std::pair<unsigned,unsigned> cell_pos;
    
  std::set< cell_pos > visited_cells;
  std::set< cell_pos > cells_to_fill;
  
  cells_to_fill.insert( cell_pos(x,y) );
  
  while( !cells_to_fill.empty() )
  {
    cell_pos p = *cells_to_fill.begin();
    
    cell_pos left   = cell_pos(p.first-1,p.second);
    cell_pos right  = cell_pos(p.first+1,p.second);
    cell_pos up     = cell_pos(p.first,p.second-1);
    cell_pos down   = cell_pos(p.first,p.second+1);
    
    M_CHECK_AND_ADD( left  );
    M_CHECK_AND_ADD( right );
    M_CHECK_AND_ADD( up    );
    M_CHECK_AND_ADD( down  );
    
    if( search_diagonal )
    {
      cell_pos top_left   = cell_pos(p.first-1,p.second-1);
      cell_pos top_right  = cell_pos(p.first+1,p.second-1);
      cell_pos bot_left   = cell_pos(p.first-1,p.second+1);
      cell_pos bot_right  = cell_pos(p.first+1,p.second+1);
        
      M_CHECK_AND_ADD( top_left  );
      M_CHECK_AND_ADD( top_right );
      M_CHECK_AND_ADD( bot_left  );
      M_CHECK_AND_ADD( bot_right );
    }
    
    NewEnt( type_id, p.first, p.second ); 
    
    cells_to_fill.erase( p );
    visited_cells.insert( p );
  }
    
#undef M_CHECK_AND_ADD
}

void Map::AreaFillEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  int r = static_cast<int>(brx);
  int b = static_cast<int>(bry);
  
  for(int j = tly; j <= b; ++j)
  {
    for(int i = tlx; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      c->CreateEnt( type_id );
    }
  }
}

void Map::AreaFillRectEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  const int r = static_cast<int>(brx);
  const int b = static_cast<int>(bry);
  
  const int t = static_cast<int>(tly);
  const int l = static_cast<int>(tlx);
  
  for(int j = t; j <= b; ++j)
  {
    for(int i = l; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      if(  i == l
        || i == r 
        || j == t
        || j == b )
        c->CreateEnt( type_id );
    }
  }
}

void Map::AreaFillEntTypeWithMask(unsigned type_id, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  int r = static_cast<int>(brx);
  int b = static_cast<int>(bry);
  
  for(int j = tly; j <= b; ++j)
  {
    for(int i = tlx; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      if( !c->HasTypeInMask( mask ) )
        c->CreateEnt( type_id );
    }
  }
}

void Map::AreaFillRectEntTypeWithMask(unsigned type_id, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  const int r = static_cast<int>(brx);
  const int b = static_cast<int>(bry);
  
  const int t = static_cast<int>(tly);
  const int l = static_cast<int>(tlx);
  
  for(int j = t; j <= b; ++j)
  {
    for(int i = l; i <= r; ++i)
    {
      Cell* c = GetCell(i,j);
      if( !c )
        continue;
      
      if(  i == l
        || i == r 
        || j == t
        || j == b )
      {
        if( !c->HasTypeInMask( mask ) )
          c->CreateEnt( type_id );
      }
    }
  }
}

Cell& Map::GetRandomCell()
{
  const unsigned max = w() * h();
  unsigned index = RNG::Rand(max);
  
  return *GetCell( index );
}

Cell* Map::GetRandomCellWithEntType(unsigned type_id)
{
  std::vector<Cell*> cells;
  GetCellsWithEntType(cells, type_id);
  
  if( cells.empty() )
    return 0;
  
  const unsigned max = cells.size();
  unsigned index = RNG::Rand(max);
  
  return cells[index];
}

Cell* Map::GetRandomCellInArea(unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  unsigned x = RNG::Rand(tlx,brx);
  unsigned y = RNG::Rand(tly,bry);
  
  return GetCell(x,y);
}

Cell* Map::GetRandomCellInAreaWithEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  std::vector<Cell*> cells;
  GetCellsInAreaWithEntType( cells, type_id, tlx, tly, brx, bry );
  
  if( cells.empty() )
    return 0;
  
  const unsigned max = cells.size();
  unsigned index = RNG::Rand(max);
  
  return cells[index];
}

Cell* Map::GetRandomCellInAreaWithMask(const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry)
{
  RUtils::FitArea(static_cast<int>(w())-1, static_cast<int>(h())-1, tlx, tly, brx, bry);
  
  std::vector<Cell*> cells;
  GetCellsInAreaWithMask( cells, mask, tlx, tly, brx, bry );
  
  if( cells.empty() )
    return 0;
  
  const unsigned max = cells.size();
  unsigned index = RNG::Rand(max);
  
  return cells[index];
}

Cell* Map::GetRandomEmptyCell()
{
  std::vector<Cell*> cells;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell( i, j );

      if(c->GetNumEnts() == 0)
        cells.push_back( c );
    }
  }

  if(cells.empty())
    return 0;

  int cell_index = RNG::Rand( cells.size() );
  return cells[ cell_index ];
}


unsigned Map::GetNumEnts()
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell(i,j);
      count += c->GetNumEnts();
    }
  }
  return count;
}

unsigned Map::GetNumEntsOfType(unsigned type_id)
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell(i,j);      
      count += c->GetNumEntsType(type_id);
    }
  }
  return count;
}


unsigned Map::GetNumCellsWithEntType(unsigned type_id)
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell(i,j);      
      if( c->HasEntType(type_id) != -1 )
        count += 1;
    }
  }
  return count;
}
  
unsigned Map::GetNumCellsWithMask(const std::set<unsigned>& mask)
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell(i,j);      
      if( !c->HasTypeInMask( mask ) )
        count += 1;
    }
  }
  return count;
}

unsigned Map::GetNumEmptyCells()
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell( i, j );
      if(c->GetEnt(0) == 0)
        count += 1;
    }
  }
  return 0;
}

unsigned Map::GetNumNonEmptyCells()
{
  unsigned count = 0;
  for(unsigned j = 0; j < h(); ++j)
  {
    for(unsigned i = 0; i < w(); ++i)
    {
      Cell* c = GetCell( i, j );
      if(c->GetEnt( 0 ))
        count += 1;
    }
  }
  return 0;
}

void Map::GetAdjacentCells(std::vector<Cell*>& out_cells, const Cell& c, bool search_diagonal)
{
  GetAdjacentCells( out_cells, c.x(), c.y(), search_diagonal );
}

void Map::GetAdjacentCells(std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal)
{
  if( !ValidCell(x,y) )
    return;
  
#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) )\
     { \
       out_cells.push_back( GetCell(c.first,c.second) ); \
     }
  
  typedef std::pair<unsigned,unsigned> cell_pos;
  
  cell_pos p = cell_pos(x,y);
  
  cell_pos left   = cell_pos(p.first-1,p.second);
  cell_pos right  = cell_pos(p.first+1,p.second);
  cell_pos up     = cell_pos(p.first,p.second-1);
  cell_pos down   = cell_pos(p.first,p.second+1);
  
  M_CHECK_AND_ADD( left  );
  M_CHECK_AND_ADD( right );
  M_CHECK_AND_ADD( up    );
  M_CHECK_AND_ADD( down  );
  
  if( search_diagonal )
  {
    cell_pos top_left   = cell_pos(p.first-1,p.second-1);
    cell_pos top_right  = cell_pos(p.first+1,p.second-1);
    cell_pos bot_left   = cell_pos(p.first-1,p.second+1);
    cell_pos bot_right  = cell_pos(p.first+1,p.second+1);
      
    M_CHECK_AND_ADD( top_left  );
    M_CHECK_AND_ADD( top_right );
    M_CHECK_AND_ADD( bot_left  );
    M_CHECK_AND_ADD( bot_right );
  }
    
#undef M_CHECK_AND_ADD
}

void Map::GetAdjacentCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, const Cell& c, bool search_diagonal)
{
  GetAdjacentCellsWithEntType( out_cells, type_id, c.x(), c.y(), search_diagonal );
}

void Map::GetAdjacentCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, unsigned x, unsigned y, bool search_diagonal)
{
  if( !ValidCell(x,y) )
    return;
  
#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
      && GetCell(c.first,c.second)->HasEntType( type_id ) != -1 )\
     { \
       out_cells.push_back( GetCell(c.first,c.second) ); \
     }
  
  typedef std::pair<unsigned,unsigned> cell_pos;
  
  cell_pos p = cell_pos(x,y);
  
  cell_pos left   = cell_pos(p.first-1,p.second);
  cell_pos right  = cell_pos(p.first+1,p.second);
  cell_pos up     = cell_pos(p.first,p.second-1);
  cell_pos down   = cell_pos(p.first,p.second+1);
  
  M_CHECK_AND_ADD( left  );
  M_CHECK_AND_ADD( right );
  M_CHECK_AND_ADD( up    );
  M_CHECK_AND_ADD( down  );
  
  if( search_diagonal )
  {
    cell_pos top_left   = cell_pos(p.first-1,p.second-1);
    cell_pos top_right  = cell_pos(p.first+1,p.second-1);
    cell_pos bot_left   = cell_pos(p.first-1,p.second+1);
    cell_pos bot_right  = cell_pos(p.first+1,p.second+1);
      
    M_CHECK_AND_ADD( top_left  );
    M_CHECK_AND_ADD( top_right );
    M_CHECK_AND_ADD( bot_left  );
    M_CHECK_AND_ADD( bot_right );
  }
    
#undef M_CHECK_AND_ADD
}

void Map::GetAdjacentEmptyCells( std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal )
{
  if(!ValidCell( x, y ))
    return;

#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
      && GetCell(c.first,c.second)->GetNumEnts() == 0 )\
     { \
       out_cells.push_back( GetCell(c.first,c.second) ); \
     }

  typedef std::pair<unsigned, unsigned> cell_pos;

  cell_pos p = cell_pos( x, y );

  cell_pos left = cell_pos( p.first - 1, p.second );
  cell_pos right = cell_pos( p.first + 1, p.second );
  cell_pos up = cell_pos( p.first, p.second - 1 );
  cell_pos down = cell_pos( p.first, p.second + 1 );

  M_CHECK_AND_ADD( left );
  M_CHECK_AND_ADD( right );
  M_CHECK_AND_ADD( up );
  M_CHECK_AND_ADD( down );

  if(search_diagonal)
  {
    cell_pos top_left = cell_pos( p.first - 1, p.second - 1 );
    cell_pos top_right = cell_pos( p.first + 1, p.second - 1 );
    cell_pos bot_left = cell_pos( p.first - 1, p.second + 1 );
    cell_pos bot_right = cell_pos( p.first + 1, p.second + 1 );

    M_CHECK_AND_ADD( top_left );
    M_CHECK_AND_ADD( top_right );
    M_CHECK_AND_ADD( bot_left );
    M_CHECK_AND_ADD( bot_right );
  }

#undef M_CHECK_AND_ADD
}

void Map::GetAdjacentNonEmptyCells( std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal )
{
  if(!ValidCell( x, y ))
    return;

#define M_CHECK_AND_ADD( c ) \
    if( ValidCell(c.first,c.second) \
      && GetCell(c.first,c.second)->GetNumEnts() != 0 )\
     { \
       out_cells.push_back( GetCell(c.first,c.second) ); \
     }

  typedef std::pair<unsigned, unsigned> cell_pos;

  cell_pos p = cell_pos( x, y );

  cell_pos left = cell_pos( p.first - 1, p.second );
  cell_pos right = cell_pos( p.first + 1, p.second );
  cell_pos up = cell_pos( p.first, p.second - 1 );
  cell_pos down = cell_pos( p.first, p.second + 1 );

  M_CHECK_AND_ADD( left );
  M_CHECK_AND_ADD( right );
  M_CHECK_AND_ADD( up );
  M_CHECK_AND_ADD( down );

  if(search_diagonal)
  {
    cell_pos top_left = cell_pos( p.first - 1, p.second - 1 );
    cell_pos top_right = cell_pos( p.first + 1, p.second - 1 );
    cell_pos bot_left = cell_pos( p.first - 1, p.second + 1 );
    cell_pos bot_right = cell_pos( p.first + 1, p.second + 1 );

    M_CHECK_AND_ADD( top_left );
    M_CHECK_AND_ADD( top_right );
    M_CHECK_AND_ADD( bot_left );
    M_CHECK_AND_ADD( bot_right );
  }

#undef M_CHECK_AND_ADD
}

unsigned Map::GetNumAdjacentCellsWithEntType(unsigned type_id, const Cell& c, bool search_diagonal)
{
  std::vector<Cell*> cells;
  GetAdjacentCellsWithEntType( cells, type_id, c, search_diagonal ); 
  return cells.size();
}

unsigned Map::GetNumAdjacentCellsWithEntType(unsigned type_id, unsigned x, unsigned y, bool search_diagonal)
{
  std::vector<Cell*> cells;
  GetAdjacentCellsWithEntType( cells, type_id, x, y, search_diagonal );
  return cells.size();
}




///Private methods


Cell* Map::GetCellFromStorage(unsigned index)
{
  if( !ValidCell(index) )
    return 0;
  return &_layout[ index ];
}

void Map::FreeAllCells()
{
  RMaker::UnloadAllMapEnt();
  
  _layout.clear();
}

void Map::ResizeCells()
{
  unsigned size = _w * _h;
  
  _layout.resize(size);
}

unsigned Map::GetIndex(unsigned x, unsigned y) const
{
  if( x >= w() || y >= h() )
    return unsigned(-1);
  
  const unsigned index = x + _w * y;
  return index;
}

unsigned Map::GetMaxIndex() const
{
  return _layout.size();
}




///Functions that act on collections

void FillCellsWithEntType( unsigned type_id, std::vector<Cell*>& cells )
{
  for(unsigned i = 0; i < cells.size(); ++i)
  {
    if(cells[ i ])
      cells[ i ]->CreateEnt( type_id );
  }
}

void FillEmptyCellsWithEntType( unsigned type_id, std::vector<Cell*>& cells )
{
  for(unsigned i = 0; i < cells.size(); ++i)
  {
    if(cells[ i ] && cells[i]->GetNumEnts() == 0 )
      cells[ i ]->CreateEnt( type_id );
  }
}







  
  
  
  
}