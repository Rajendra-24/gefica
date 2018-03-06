#ifndef GeFiCa_POINTCONTACTRZ_H
#define GeFiCa_POINTCONTACTRZ_H

#include "RZ.h"

namespace GeFiCa { class PointContactRZ; }

class GeFiCa::PointContactRZ : public GeFiCa::RZ
{
  public:
    double RUpperBound,RLowerBound,ZUpperBound,ZLowerBound,PointBegin,PointEnd;//bounds for X and Y and point start and end
   public :
     PointContactRZ(int ix,int iy) : RZ(ix,iy), RUpperBound(1),RLowerBound(0),ZUpperBound(1),ZLowerBound(0), PointBegin(0.4),PointEnd(0.6){};

     /**
      * Assign initial voltage values to grid points.
      */
     void Initialize();
     bool CalculateField(EMethod method=kSOR2);

     ClassDef(PointContactRZ,1);
};

#endif
