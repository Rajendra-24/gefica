#include "Sphere1D.h"
#include "Units.h"
#include <iostream>
using namespace GeFiCa;

void Sphere1D::Initialize()
{
   if (OuterRadius<=InnerRadius) {
      Warning("Initialize",
            "Lower bound (%f) >= upper bound (%f)! No grid is created!",
            InnerRadius, OuterRadius);
      return;
   }
   double steplength=(OuterRadius-InnerRadius)/(n-1);
   SetStepLength(steplength); // fC1[i] set to [0, (n-1)*steplength]
   for (int i=n;i-->0;) fC1[i]=fC1[i]+InnerRadius;
   fIsFixed[0]=true;
   fIsFixed[n-1]=true;
   double slope = (V1-V0)/(n-1);
   for (int i=0; i<n; i++) fV[i]=V0+slope*i;
}
//_____________________________________________________________________________
//
#include  <cmath>
bool Sphere1D::Analytic()
{
   bool isConstantImpurity=true;
   for(int i=0;i+1<n;i++)
      if (fImpurity[i]!=fImpurity[i+1]) isConstantImpurity=false;
   if(!isConstantImpurity) {
      Warning("Analytic","can't handle changing impurity! Return false.");
      return false;
   }
   double density=fImpurity[0]*Qe;
   double c1=(V1-V0 + density/epsilon/6*(fC1[n-1]*fC1[n-1]-fC1[0]*fC1[0]))
      /(1/fC1[n-1]-1/fC1[0]);
   double c2=V0+density/epsilon/6*fC1[0]*fC1[0]-c1/fC1[0];
   for (int i=0; i<n; i++) {
      fV[i] = -density/6/epsilon*fC1[i]*fC1[i]+c1/fC1[i]+c2;
      // Fixme:
      if (i!=0||i!=n-1)fE1[i]=(fV[i+1]-fV[i-1])/(fdC1p[i]+fdC1m[i]);
   }
   return true;
}
