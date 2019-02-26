#ifndef GeFiCa_RHOPHI_H
#define GeFiCa_RHOPHI_H

#include "XY.h"

namespace GeFiCa { class RhoPhi; }

/**
 * 2D cylindrical coordinates.
 */
class GeFiCa::RhoPhi : public GeFiCa::XY
{
   public:
      /**
       * Default constructor
       */
      RhoPhi(int n1=101, int n2=101,
            const char *name="rp",
            const char *title="2D cylindrical coordinates")
         : XY(n1, n2, name, title) {}; 

      ClassDef(RhoPhi,1);

   protected:
      void DoSOR2(int idx);
      virtual double GetData(double tarx,double tary,EOutput output);
};
#endif

