#ifndef GeFiCa_POINTCONTACTXY_H
#define GeFiCa_POINTCONTACTXY_H

#include "XY.h"

namespace GeFiCa { class PointContactXY; }

class GeFiCa::PointContactXY : public GeFiCa::XY
{
  public:
    double cathode_voltage,annode_voltage;
    double XUpperBound,XLowerBound,YUpperBound,YLowerBound,PointBegin,PointEnd;//bounds for X and Y and point start and end
   public :
     PointContactXY(int ix,int iy) : XY(ix,iy),cathode_voltage(2000),annode_voltage(0), XUpperBound(10),XLowerBound(1),YUpperBound(10),YLowerBound(1) {};

     void initialize();
     bool CalculateField(EMethod method=kSOR2);

     ClassDef(PointContactXY,1);
};

#endif
