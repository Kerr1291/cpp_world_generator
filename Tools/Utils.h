#pragma once
#include <algorithm>



namespace RUtils
{
  
  template <typename T>
  void Clamp(T& out_val, const T& min, const T& max)
  {
    out_val = std::max<T>(out_val, min);
    out_val = std::min<T>(out_val, max);
  }
  
  template <typename T>
  void Sort2(T& out_val0, T& out_val1)
  {
    if( out_val0 > out_val1 )
      std::swap(out_val0,out_val1);
  }
  
  void FitArea(int w, int h, unsigned& tlx, unsigned& tly, unsigned& brx, unsigned& bry);
}