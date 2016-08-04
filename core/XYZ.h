/////////////////////////
//
//XYZ                  //
//
//
#ifndef GEFICA_XYZ_H
#define GEFICA_XYZ_H

class TF3;

#include "XY.h"

namespace GEFICA { class XYZ; }

class GEFICA::XYZ : public GEFICA::XY
{
   public:
      unsigned short n3; // number of steps along the 3nd axis

   public:
      XYZ(unsigned short n1=101, unsigned short n2=11,unsigned short n3=11);
      //______________________
      //claim a field with n1*n2*n3 grids
      virtual ~XYZ();

      virtual void CreateGridWithFixedStepLength(double steplength);
      virtual void RK2(int idx,bool elec); 

      virtual void SaveField(const char *fout=NULL);
      virtual void LoadField(const char *fin=NULL);

      virtual double GetData(double tarx,double tary,double tarz,int thing);
      //_____________________
      //get item with number: 0:Impurity 1:Potential 2: Ex 3:Ey 4:Ez
      virtual void SetImpurity(TF3 * Im);
      
      ClassDef(XYZ,1);

   protected:

      double *fE3,*fC3,*fDistanceToUp,*fDistanceToDown;//left and right are for y axis
      virtual int FindIdx(double tarx,double tary,
            double tarz,int begin,int end);
};

#endif
