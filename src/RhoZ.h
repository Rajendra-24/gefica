#ifndef GeFiCa_RhoZ
#define GeFiCa_RhoZ

#include <TNamed.h>
#include "Grid.h"
namespace GeFiCa { class RhoZ; class PointContact; class Segmented; }
/**
 * 2D cylindrical coordinates.
 */
class GeFiCa::RhoZ : public Grid, public TNamed
{
   public:
      RhoZ(size_t n1=101, size_t n2=101) : Grid(n1, n2),
      TNamed("rhoz", "2D cylindrical coordinates") {};

      void GetBoundaryConditionFrom(Detector &detector);
      double GetC();

      ClassDef(RhoZ,1);
   protected:
      void OverRelaxAt(size_t idx); 
      void CalculateE();
      void GetBoundaryConditionFrom(Segmented &detector) {};
      void GetBoundaryConditionFrom(PointContact &detector);
      void ReallocateGridPointsNearBoundaries(PointContact &detector);
};
#endif 

