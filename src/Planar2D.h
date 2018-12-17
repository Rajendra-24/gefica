#ifndef GeFiCa_PLANAR2D_H
#define GeFiCa_PLANAR2D_H

#include "XY.h"

namespace GeFiCa { class Planar2D; }

class GeFiCa::Planar2D : public GeFiCa::XY
{
   public :
      double XUpperBound,XLowerBound,YUpperBound,YLowerBound; 

   public :
      Planar2D(int ix,int iy) : XY(ix,iy),XUpperBound(1),XLowerBound(0),YUpperBound(1),YLowerBound(0){};
      void Initialize();
      bool CalculatePotential(EMethod method=kSOR2);

      ClassDef(Planar2D, 1);
};

#endif
