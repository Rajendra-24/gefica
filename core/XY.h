#ifndef GEFICA_XY_H
#define GEFICA_XY_H

#include "X.h"
#include <TF2.h>

namespace GEFICA { class XY; }

class GEFICA::XY : public GEFICA::X
{
   public:
      unsigned short n2; // number of steps along the 2nd axis

   public:
      XY(unsigned short n1=101, unsigned short n2=101);
      virtual ~XY();

      virtual void CreateGridWithFixedStepLength(double steplength);
      virtual void Update(int idx); 

      virtual void SaveField(const char *fout=NULL);
      virtual void LoadField(const char *fin=NULL);

      virtual double GetData(double tarx,double tary,int thing);
      virtual void SetImpurity(TF2 * Im);

      ClassDef(XY,1);

   protected:

      double *fE2,*fC2,*fDistanceToLeft,*fDistanceToRight;//left and right are for y axis
      virtual int FindIdx(double tarx,double tary
            ,int ybegin,int yend);
};

#endif
