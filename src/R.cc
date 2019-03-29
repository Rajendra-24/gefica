#include "R.h"
#include "Units.h"
#include "Hemispherical.h"
using namespace GeFiCa;

void R::GetBoundaryConditionFrom(Detector &detector)
{
   Grid::GetBoundaryConditionFrom(detector); // check number of calls

   TString type(detector.ClassName());
   if (type.Contains("Hemispherical")==false) {
      Error("GetBoundaryConditionFrom", "%s is not expected. "
            "Please pass in a Hemispherical detector.", type.Data());
      abort();
   }
   Hemispherical& hemi = (Hemispherical&) detector;
   hemi.CheckConfigurations();

   double dR=hemi.Height-hemi.PointContactR;
   for (size_t i=0; i<N1; i++) {
      dC1p.push_back(dR/(N1-1)); dC1m.push_back(dR/(N1-1));
      C1.push_back(hemi.PointContactR+i*dC1p[i]);
      E1.push_back(0); Et.push_back(0);
      fIsFixed.push_back(false); fIsDepleted.push_back(false);
      Src.push_back(-hemi.GetImpurity(C1[i])*Qe/epsilon);
   }
   dC1m[0]=0; dC1p[N1-1]=0;
   // fix 1st and last points
   fIsFixed[0]=true; fIsFixed[N1-1]=true;
   // linear interpolation between Bias[0] and Bias[1]
   double slope = (hemi.Bias[1]-hemi.Bias[0])/(N1-1);
   for (size_t i=0; i<N1; i++) Vp.push_back(hemi.Bias[0]+slope*i);
   Vp[N1-1]=hemi.Bias[1];
}
//_____________________________________________________________________________
//
void R::SolveAnalytically()
{
   Grid::SolveAnalytically(); // check if impurity is constant
   // https://www.wolframalpha.com/input/?i=(r%5E2V%27)%27%2Fr%5E2%3Da
   // V(x) = a*r^2/6 + c1/r + c2
   double a = -Src[0];
   double c1=(Vp[N1-1]-Vp[0] + a*(C1[fN-1]*C1[fN-1]-C1[0]*C1[0])/6)
      /(1/C1[fN-1]-1/C1[0]);
   double c2=Vp[0]+a/6*C1[0]*C1[0]-c1/C1[0];
   for (size_t i=0; i<N1; i++) Vp[i] = Src[0]*C1[i]*C1[i]/6 + c1/C1[i] + c2;
   CalculateE();
}
//_____________________________________________________________________________
//
void R::OverRelaxAt(size_t idx)
{
//   if (fIsFixed[idx])return ;
//   double density=fImpurity[idx]*Qe;
//   double h2=dC1m[idx];
//   double h3=dC1p[idx];
//   double p2=V[idx-1];
//   double p3=V[idx+1];
//   double tmp=(+density/epsilon*(h2+h3)*0.5+1/C1[idx]*(p3-p2)
//         +p3/h2+p2/h3)/(1/h2+1/h3);
//
//   //find minmium and maxnium of all five grid, the new one should not go overthem.
//   //find min
//   double min=p2;
//   double max=p2;
//   if(min>p3)min=p3;
//   //find max
//   if(max<p3)max=p3;
//   //if tmp is greater or smaller than max and min, set tmp to it.
//
//   //V[idx]=RelaxationFactor*(tmp-V[idx])+V[idx];
//   double oldP=V[idx];
//   tmp=RelaxationFactor*(tmp-oldP)+oldP;
//
//   if(tmp<min) {
//      V[idx]=min;
//      fIsDepleted[idx]=false;
//   } else if(tmp>max) {
//      V[idx]=max;
//      fIsDepleted[idx]=false;
//   } else fIsDepleted[idx]=true;
//
//   if(fIsDepleted[idx]||Bias[0]==Bias[1]) V[idx]=tmp;
}
