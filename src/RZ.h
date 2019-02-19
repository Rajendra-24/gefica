#ifndef GeFiCa_RZ_H
#define GeFiCa_RZ_H

#include "XY.h"
class TF2;

namespace GeFiCa { class RZ; }

/**
 * 2D cylindrical coordinates.
 */
class GeFiCa::RZ : public GeFiCa::XY
{
   public:
      RZ(int n1=101, int n2=101, const char *name="rz",
            const char *title="2D cylindrical coordinates")
         : XY(n1, n2, name, title) {}; ///< Default constructor

      double GetCapacitance();

      ClassDef(RZ,1);

   protected:
      virtual void DoSOR2(int idx);
};
#endif 
