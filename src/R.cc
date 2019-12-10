#include "R.h"
#include "Units.h"
#include "Hemispherical.h"
using namespace GeFiCa;

void R::SetupWith(Detector &detector)
{
   Grid::SetupWith(detector); // check number of calls

   TString type(detector.ClassName());
   if (type.Contains("Hemispherical")==false) {
      Error("SetupWith", "%s is not expected. "
            "Please pass in a Hemispherical detector.", type.Data());
      abort();
   }
   Hemispherical& hemi = (Hemispherical&) detector;
   hemi.CheckConfigurations();
   fDetector = &detector; // for GetC to use fDetector->Bias[]

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
//______________________________________________________________________________
//
void R::SolveAnalytically()
{
   Grid::SolveAnalytically(); // check if impurity is constant
   // https://www.wolframalpha.com/input/?i=(r%5E2V%27)%27%2Fr%5E2%3Da
   // V(x) = a*r^2/6 + c1/r + c2
   double a = -Src[0];
   double c1= a*C1[0]*C1[N1-1]*(C1[N1-1]+C1[0])/6
      -(Vp[N1-1]-Vp[0])*C1[0]*C1[N1-1]/(C1[N1-1]-C1[0]);
   double c2= (Vp[N1-1]*C1[N1-1]-Vp[0]*C1[0])/(C1[N1-1]-C1[0])
      -a*(C1[N1-1]*C1[N1-1]+C1[N1-1]*C1[0]+C1[0]*C1[0])/6;
   for (size_t i=0; i<N1; i++) Vp[i] = a*C1[i]*C1[i]/6 + c1/C1[i] + c2;
   CalculateE();
}
//______________________________________________________________________________
//
double R::GetC()
{
   Grid::GetC(); // calculate field excluding undepleted region

   double dV = fDetector->Bias[1]-fDetector->Bias[0]; if (dV<0) dV=-dV;
   double integral=0;
   for (size_t i=0; i<GetN(); i++) {
      integral+=E1[i]*E1[i]*dC1p[i]; // Fixme:: this only works for X
      if (!fIsDepleted[i]) fIsFixed[i]=false; // release undepleted points
   }
   double c=integral*epsilon/dV/dV;
   Info("GetC","%.2f pF/cm2",c/pF*cm2);
   return c;
}
//______________________________________________________________________________
//
void R::OverRelaxAt(size_t idx)
{
   if (fIsFixed[idx]) return; // no need to calculate on boundaries

   double vnew=Src[idx]*dC1m[idx]*dC1p[idx]/2+(1/C1[idx]*(Vp[idx+1]-Vp[idx-1])
         +Vp[idx+1]/dC1p[idx]+Vp[idx-1]/dC1m[idx])/(1/dC1m[idx]+1/dC1p[idx]);
   vnew=RelaxationFactor*(vnew-Vp[idx])+Vp[idx];

   // check depletion and update Vp[idx] accordingly
   double min=Vp[idx-1], max=Vp[idx-1];
   if (min>Vp[idx+1]) min=Vp[idx+1];
   if (max<Vp[idx+1]) max=Vp[idx+1];
   if (vnew<min) {
      fIsDepleted[idx]=false; Vp[idx]=min;
   } else if (vnew>max) {
      fIsDepleted[idx]=false; Vp[idx]=max;
   } else {
      fIsDepleted[idx]=true; Vp[idx]=vnew;
   }

   // update Vp for impurity-only case even if the point is undepleted
   if (Vp[0]==Vp[N1-1]) Vp[idx]=vnew;
}
