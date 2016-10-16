#ifndef GeFiCa_SPHERE_H
#define GeFiCa_SPHERE_H

#include "RThetaPhi.h"

namespace GeFiCa { class Sphere;}

class GeFiCa::Sphere: public GeFiCa::RThetaPhi
{
   public :
     double UpperBound,LowerBound; // boundary of the planar detector   
     double cathode_voltage,anode_voltage;
   public:
      Sphere(int r,int O,int a) : RThetaPhi(r, O,a ),UpperBound(10),LowerBound(0), cathode_voltage(2000),anode_voltage(0){};

      void initialize();
      bool CalculateField(EMethod method=kSOR2);

      ClassDef(Sphere,1);
};

#endif
