#include "Units.h"
#include "Sphere1D.h"
using namespace GeFiCa;

Sphere1D::Sphere1D(int n, const char *name, const char *title)
   : R(n, name, title), InnerR(0.3*cm), OuterR(3*cm) {};
//_____________________________________________________________________________
//
void Sphere1D::InitializeGrid()
{
   if (OuterR<=InnerR) Fatal("InitializeGrid",
         "Inner R (%.1f) >= outer R (%.1f)! Abort!", InnerR, OuterR);

   double stepLength=(OuterR-InnerR)/(fN-1);
   SetStepLength(stepLength);
   for (int i=fN;i-->0;) fC1[i]=fC1[i]+InnerR;
   fIsFixed[0]=true; fIsFixed[fN-1]=true;
   double slope = (V1-V0)/(fN-1);
   for (int i=0; i<fN; i++) fV[i]=V0+slope*i;
}
//_____________________________________________________________________________
//
void Sphere1D::FillGridWithAnalyticResult()
{
   bool isConstantImpurity=true;
   for (int i=0;i+1<fN;i++)
      if (fImpurity[i]!=fImpurity[i+1]) isConstantImpurity=false;
   if (!isConstantImpurity) {
      Warning("FillGridWithAnalyticResult",
            "can't handle changing impurity! Abort.");
      abort();
   }
   double density=fImpurity[0]*Qe;
   double c1=(V1-V0 + density/epsilon/6*(fC1[fN-1]*fC1[fN-1]-fC1[0]*fC1[0]))
      /(1/fC1[fN-1]-1/fC1[0]);
   double c2=V0+density/epsilon/6*fC1[0]*fC1[0]-c1/fC1[0];
   for (int i=0; i<fN; i++) {
      fV[i] = -density/6/epsilon*fC1[i]*fC1[i]+c1/fC1[i]+c2;
      // Fixme:
      if (i!=0||i!=fN-1)fE1[i]=(fV[i+1]-fV[i-1])/(fdC1p[i]+fdC1m[i]);
   }
}
