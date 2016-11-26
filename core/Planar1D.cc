#include "Planar1D.h"
#include <iostream>
using namespace std;

using namespace GeFiCa;

void Planar1D::Initialize()
{
   // The step length is calculated with the following equation:
   // BEGIN_HTML
   // <pre>
   //      double stepLength=(UpperBound-LowerBound)/(n-1);
   // </pre>
   // END_HTML
   // If the inner radius is not larger than the outer radius,
   // no grid will be created
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
//Here we calculate the field analytically using the fomrula (d^2 phi/d phi^2) = rho/epsilon.
//Rho in this case is just the impurity density. This gives us a differential equation 
//the type phi(x)= ax^2 + bx + c . We can find a, b, and c by evaluating this equation
//at the boundary conditions where we know the potential
// we know:
// a=rho/(2*epsilon)
// b=(Vneg-Vpos-a(LowerBound^2-UpperBound^2))/(LowerBound-UpperBound)
// c=Vneg-a*LowerBound^2-LowerBound(Vneg-Vpos-a*(LowerBound^2-UpperBound^2))/(LowerBound-UpperBound)

// Potential problem could be resulting from the deffinition of volt and impurity.
//Impurity is in cm while the SI unit of volt invlolves meters.
#include  <cmath>
bool Planar1D::Analytic()
{
   double density=-fImpurity[1]*1.6e-19;
   double d=LowerBound-UpperBound;
   double dSquare=LowerBound*LowerBound-UpperBound*UpperBound;
   double Aconst=density/2/epsilon;
   double Bconst=(fPotential[0]-fPotential[n-1]-Aconst*dSquare)/d;
   double Cconst=fPotential[0]-Aconst*LowerBound*LowerBound-LowerBound*(fPotential[0]-fPotential[n-1]-Aconst*dSquare)/d;
   for (int i=0; i<n; i++) {
     fPotential[i] = -fImpurity[i]*1.6e-19/2/epsilon*fC1[i]*fC1[i]+Bconst*fC1[i]+Cconst;
     fE1[i]=(fPotential[i+1]-fPotential[i-1])/(fDistanceToNext[i]+fDistanceToPrevious[i]);
   }
   cout<<"Aconst= "<<Aconst<<endl;
   cout<<"Bconst= "<<Bconst<<endl;
   cout<<"Cconst= "<<Cconst<<endl;

  return true;
}



// bool Planar1D::Analytic()
// {
   // double d=UpperBound-LowerBound;//thickness or depth of the detector
   // double cnst1=fPotential[0];
   // double cnst2=(fPotential[n-1]-fImpurity[n-1]*Qe/2/epsilon*d*d-cnst1)/d;
   // for (int i=0; i<n; i++) {
      // fPotential[i] = fImpurity[i]*Qe/2/epsilon*fC1[i]*fC1[i]+cnst2*fC1[i]+cnst1;
      // fE1[i]=(fPotential[i+1]-fPotential[i-1])/(fDistanceToNext[i]+fDistanceToPrevious[i]);
   // }
   // return true;
// }
//_____________________________________________________________________________
//
bool Planar1D::CalculateField(EMethod method)
{
   if(!fIsLoaded)Initialize();
   return X::CalculateField(method);
}
