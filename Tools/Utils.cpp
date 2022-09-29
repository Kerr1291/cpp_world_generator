#include <Utils.h>


namespace RUtils
{
  void FitArea(int w, int h, unsigned& tlx, unsigned& tly, unsigned& brx, unsigned& bry)
  {
    RUtils::Sort2(tlx, brx);
    RUtils::Sort2(tly, bry);
    
    RUtils::Clamp<unsigned>( tlx, 0, w );
    RUtils::Clamp<unsigned>( tly, 0, h );
    
    RUtils::Clamp<unsigned>( brx, 0, w );
    RUtils::Clamp<unsigned>( bry, 0, h );
  }
}