#include <iostream>
using namespace std;

#include "Planar1D.h"
#include "Units.h"
using namespace GeFiCa;

void Planar1D::Initialize()
{
   if (LowerBound>=UpperBound) {
      Warning("CreateGridWithFixedStepLength",
            "Lower bound (%f) >= upper bound (%f)! No grid is created!",
            LowerBound, UpperBound);
      return;
   }
   double steplength=(UpperBound-LowerBound)/(n-1);
   SetStepLength(steplength);
   for (int i=n; i-->0;) fC1[i]=fC1[i]+LowerBound;
   fIsFixed[0]=true;
   fIsFixed[n-1]=true;
   double slope = (Vpos-Vneg)/(n-1);
   for (int i=0; i<n; i++) {
	   fPotential[i]=Vneg+slope*i;
   }
}
//_____________________________________________________________________________
//
#include  <cmath>
bool Planar1D::Analytic()
{
   double d=UpperBound-LowerBound;//thickness or depth of the detector
   double a=-fImpurity[n-1]*Qe/2/epsilon;
   double b=(fPotential[n-1]-fPotential[0]-a*d*d)/d;
   double c=fPotential[0];
   for (int i=0; i<n; i++) {
      fPotential[i] = a*fC1[i]*fC1[i]+b*fC1[i]+c;
      // Fixme: i+1 and i-1 may be out of range
      fE1[i]=(fPotential[i+1]-fPotential[i-1])/(fDistanceToNext[i]+fDistanceToPrevious[i]);
   }
   return true;
}
//_____________________________________________________________________________
//
bool Planar1D::CalculateField(EMethod method)
{
   if(!fIsLoaded)Initialize();
   return X::CalculateField(method);
}
