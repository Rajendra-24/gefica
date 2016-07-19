#ifndef GEFICA_HEMISPHERIC_H
#define GEFICA_HEMISPHERIC_H

#include "Spherical.h"

namespace GEFICA { class HemiSpheric; }

class GEFICA::HemiSpheric : public GEFICA::Spherical
{
   public :
      HemiSpheric(int ix,int iy,int iz) : Spherical(ix,iy,iz) {};
      void SetVoltage(double voltage,double r); 

      ClassDef(HemiSpheric,1);
};

#endif
