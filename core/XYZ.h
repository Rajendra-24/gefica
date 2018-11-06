#ifndef GeFiCa_XYZ_H
#define GeFiCa_XYZ_H

class TF3;

#include "XY.h"

namespace GeFiCa { class XYZ; }

class GeFiCa::XYZ : public GeFiCa::XY
{
   public:
      int n3; ///< number of grid points along the 3nd coordinate

   public:
      XYZ(int n1=101, int n2=11, int n3=11);
      
      virtual ~XYZ();

      virtual void SaveField(const char *fout=NULL);
      virtual void LoadField(const char *fin=NULL);

      void SetImpurity(TF3 * Im);
      
      double GetPotential(double x,double y,double z){return GetData(x,y,z,kPotential);};
      virtual double GetE1(double x,double y,double z){return GetData(x,y,z,kE1);};
      virtual double GetE2(double x,double y,double z){return GetData(x,y,z,kE2);};
      virtual double GetE3(double x,double y,double z){return GetData(x,y,z,kE3);};
      virtual double GetImpurity(double x,double y,double z){return GetData(x,y,z,kImpurity);};
      void Copy(const XYZ&);
      ClassDef(XYZ,1);

   protected:
      virtual double GetData(double tarx,double tary,double tarz, EOutput output);
      virtual void SetStepLength(double steplength1,double steplength2,double steplength3);
      double *fE3,*fC3;
      double *fdC3p; ///< distance between this and next grid points alone C3
      double *fdC3m; ///< distance between this and previous grid points alone C3
      virtual int FindIdx(double tarx,double tary,
            double tarz,int begin,int end);
      virtual void SOR2(int idx,bool elec); 
      virtual bool CalculateField(int idx);
};
#endif

