#ifndef GEFICA_X_H
#define GEFICA_X_H

#include <TObject.h>
class TF1;

namespace GEFICA { 
   enum EMethod {
      kAnalytic,
      kSOR2,
      kSOR4,
   };

   class X;

   static const double epsilon = 16*8.854187817e-12;
   static const double cm =1;
   static const double volt=1;
   static const double cm3=cm*cm*cm;
}

//______________________________________________________________________________
// Create grid for 1-D field calculation.
// begin_macro
// planar1d.C           
// end_macro
class GEFICA::X : public TObject 
{
   public:
      int n1; // number of steps along the 1st axis
      int MaxIterations; // Iteration cuts
      int n; // n = n1*n2*n3
      double Csor; // boost Iteration speed
      double Precision;

   public:
      X(int nx=101);

      virtual ~X();

      virtual void CreateGridWithFixedStepLength(double steplength);
      virtual void CreateGridWithFixedStepLength(double LowerBound,double UpperBound);
      virtual bool CalculateField(EMethod method=kSOR2);
      virtual void SetVoltage(double anode_voltage, double cathode_voltage);

      
      virtual void SaveField(const char *fout=NULL);
      virtual void LoadField(const char *fin=NULL);
      virtual void SetImpurity(double density);
      virtual void SetImpurity(TF1 * Im);

      virtual double GetData(double tarx,int thing); 
      
      

      ClassDef(X,1);

   protected:
      bool * fIsFixed;
      double *fE1, *fPotential,*fC1,*fDistanceToNext,*fDistanceToPrevious,*fImpurity;

      virtual int FindIdx(double tarx,int begin,int end);

      virtual bool Analyic();
      
      
      virtual void SOR2(int idx,bool elec); 
      virtual void SOR4(int idx); 
};

#endif
