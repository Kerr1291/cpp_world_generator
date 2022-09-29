#pragma once
#include <vector>
#include <set>
#include <string>

#include <Types.h>

namespace RMap
{
  class Map;

  class Cell
  {    
  public:
    friend class Map;
    Cell();
    Cell(const Cell& other);
    ~Cell();
    
    unsigned x() const;
    unsigned y() const;
    
    unsigned GetMapIndex() const;
    
    void CreateEnt(unsigned type_id);
    void LoadEnt(unsigned type_id, unsigned game_id);
    
    bool AddEnt(RType::Ent ent);
    
    void RemoveEntAt(unsigned index);
    void RemoveEnt(unsigned game_id);
    void ClearEnts();
    
    bool HasTypeInMask( const std::set<unsigned>& mask ) const;
    unsigned GetNumTypesInMask( const std::set<unsigned>& mask ) const;
    
    int HasEntType(unsigned type_id) const;
    int HasEntGame(unsigned game_id) const;
    
    RType::Ent* GetEnt(unsigned index);
    RType::Ent* GetEntGame(unsigned game_id);
    
    const RType::Ent* GetEnt(unsigned index) const;
    const RType::Ent* GetEntGame(unsigned game_id) const;
    
    unsigned GetNumEnts() const;
    unsigned GetNumEntsType(unsigned type_id) const;
    
    void GetEnts(std::vector<RType::Ent*>& out_ents);
    void GetEntsOfType(std::vector<RType::Ent*>& out_ents, unsigned type_id);
    void GetEntsWithMask(std::vector<RType::Ent*>& out_ents, const std::set<unsigned>& mask);
    void GetEntsInMask(std::vector<RType::Ent*>& out_ents, const std::set<unsigned>& mask);
    
    //get the cost of moving out of or into this cell
    float GetMoveExitCost(int type_id, const std::set<unsigned>& mask) const;
    float GetMoveEnterCost(int type_id, const std::set<unsigned>& mask) const;
    
  private:
    
    RType::Ent* GetEntFromStorage(unsigned index);
    const RType::Ent* GetEntFromStorage(unsigned index) const;
    unsigned GetEntStorageCount() const;
    void FreeEntStorage();
    void AddEntToStorage(const RType::Ent& ent);
    void RemoveEntFromStorage(unsigned index);
    
    std::vector<RType::Ent> _data;
    
    unsigned _index;
    Map*     _parent;
  };
    
  class Map
  {
  public:
    Map();
    Map(unsigned w, unsigned h);
    Map(const Map& other);
    ~Map();
    
    ///Map related methods
    
    void SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename);

    //root has no effect on internal map data at all
    //typically you only set this if you want to have submaps, for example, and keep track of where this data should go in the parent
    void SetRoot( int x, int y );

    void GetRoot( int& out_rx, int& out_ry );

    int GetRootX();
    int GetRootY();
    
    void SetSize(unsigned w, unsigned h);

    unsigned w() const;
    unsigned h() const;
    
    ///Cell-level methods
    
    void GetAllCells(std::vector<Cell*>& out_cells);
    
    Cell* GetCell(unsigned index);
    Cell* GetCell(unsigned x, unsigned y);
    
    void ClearCell(unsigned index);
    void ClearCell(unsigned x, unsigned y);
    
    bool ValidCell(unsigned index) const;
    bool ValidCell(unsigned x, unsigned y) const;
    
    ///Entity-level methods
    
    //given an array of data, load it as an WxH 2D array of typeids located at X/Y (-1 means empty)
    void LoadEntData(const int* data, unsigned x, unsigned y, unsigned w, unsigned h);

    //Load the data from another map into this one
    void LoadEntData(Map& m, unsigned x, unsigned y);

    //A little different, if the input map has a nonempty cell, place the input type in our map instead
    void LoadEntDataAndReplace( Map& m, unsigned type_id, unsigned x, unsigned y );
    
    //same as above, but also don't load input data if the dest cell matches with mask
    void LoadEntDataWithMask(const int* data, const std::set<unsigned>& mask, unsigned x, unsigned y, unsigned w, unsigned h);

    void LoadEntDataWithMask( Map& m, const std::set<unsigned>& mask, unsigned x, unsigned y );
    
    //Generates a new entity of the given type in this cell
    void NewEnt(unsigned type_id, Cell& c);
    void NewEnt(unsigned type_id, unsigned x, unsigned y);
    
    void MoveEnt(unsigned game_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1);
    void MoveEntdx(unsigned game_id, unsigned x, unsigned y, unsigned dx, unsigned dy);
    
    void MoveEntsOfType(unsigned type_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1);
    void MoveEntsOfTypedx(unsigned type_id, unsigned x, unsigned y, unsigned dx, unsigned dy);
    
    void MoveEntsNotType(unsigned type_id, unsigned x0, unsigned y0, unsigned x1, unsigned y1);
    void MoveEntsNotTypedx(unsigned type_id, unsigned x, unsigned y, unsigned dx, unsigned dy);
    
    void MoveEnts(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
    void MoveEntsdx(unsigned x, unsigned y, unsigned dx, unsigned dy);
    
    ///Utility functions
    
    //get ALL cells that contain at least 1 entity of this type
    void GetCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id);
    
    //get ALL cells that don't have a type contained in mask
    void GetCellsWithMask(std::vector<Cell*>& out_cells, const std::set<unsigned>& mask);
    
    //select a group of cells
    void GetCellsInArea(std::vector<Cell*>& out_cells, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    void GetCellsInAreaWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);    
    void GetCellsInAreaWithMask(std::vector<Cell*>& out_cells, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);

    void GetCellsInFloodFill( std::vector<Cell*>& out_cells, unsigned type_id, unsigned x, unsigned y, const std::set<unsigned>& boundry_type_ids, bool search_diagonal = true );

    //Select cells with flood fill and then use them to build a map that is sized exactly to contain these elements
    void GetSubMapInFloodFillMatch( Map& out_map, unsigned type_id, unsigned x, unsigned y, bool search_diagonal = true );

    
    //"draw" filled area boardered by given mask
    void FloodFillEntType(unsigned type_id, unsigned x, unsigned y, const std::set<unsigned>& boundry_type_ids, bool search_diagonal=true);
    
    //"draw" filled box
    void AreaFillEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    
    //"draw" hollow box
    void AreaFillRectEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    
    //"draw" filled box but ignore cells that match with mask
    void AreaFillEntTypeWithMask(unsigned type_id, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    
    //"draw" hollow box but ignore cells that match with mask
    void AreaFillRectEntTypeWithMask(unsigned type_id, const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    
    Cell& GetRandomCell();
    Cell* GetRandomCellWithEntType(unsigned type_id);
    Cell* GetRandomCellInArea(unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    Cell* GetRandomCellInAreaWithEntType(unsigned type_id, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    Cell* GetRandomCellInAreaWithMask(const std::set<unsigned>& mask, unsigned tlx, unsigned tly, unsigned brx, unsigned bry);
    Cell* GetRandomEmptyCell();
    
    unsigned GetNumEnts();
    unsigned GetNumEntsOfType(unsigned type_id);
    
    unsigned GetNumCellsWithEntType(unsigned type_id);
    unsigned GetNumCellsWithMask(const std::set<unsigned>& mask);

    unsigned GetNumEmptyCells();
    unsigned GetNumNonEmptyCells();
    
    void GetAdjacentCells(std::vector<Cell*>& out_cells, const Cell& c, bool search_diagonal=true);
    void GetAdjacentCells(std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal=true);
    
    void GetAdjacentCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, const Cell& c, bool search_diagonal=true);
    void GetAdjacentCellsWithEntType(std::vector<Cell*>& out_cells, unsigned type_id, unsigned x, unsigned y, bool search_diagonal=true);

    void GetAdjacentEmptyCells( std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal = true );
    void GetAdjacentNonEmptyCells( std::vector<Cell*>& out_cells, unsigned x, unsigned y, bool search_diagonal = true );

    unsigned GetNumAdjacentCellsWithEntType(unsigned type_id, const Cell& c, bool search_diagonal=true);
    unsigned GetNumAdjacentCellsWithEntType(unsigned type_id, unsigned x, unsigned y, bool search_diagonal=true);
    
  private:
    
    Cell* GetCellFromStorage(unsigned index);
    void FreeAllCells();
    void ResizeCells();
    unsigned GetIndex(unsigned x, unsigned y) const;
    unsigned GetMaxIndex() const;
    
    int _rootx, _rooty;
    unsigned _w, _h;
    std::vector<Cell> _layout;
  };
  
  void FillCellsWithEntType( unsigned type_id, std::vector<Cell*>& cells );
  void FillEmptyCellsWithEntType( unsigned type_id, std::vector<Cell*>& cells );
  
  
  
  
}
















