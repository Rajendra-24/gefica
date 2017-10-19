#include "PointContactRZ.h"
#include "iostream"
#include "Units.h"
using namespace GeFiCa;

void PointContactRZ::Initialize()
{
   // The step length is calculated with the following equation:
   // BEGIN_HTML
   // <pre>
   //      double stepLength=(UpperBound-LowerBound)/(n-1);
   // </pre>
   // END_HTML
   // If the inner radius is not larger than the outer radius,
   // no grid will be created
   if (RLowerBound>=RUpperBound||ZLowerBound>=ZUpperBound) {
      Warning("Initialize",
            "Lower bound (%f) >= upper bound (%f)! No grid is created!",
            RLowerBound, RUpperBound);
      return;
   }
   double steplength1=(RUpperBound-RLowerBound)/(n1-1);
   double steplength2=(ZUpperBound-ZLowerBound)/(n2-1);
   std::cout<<steplength1<<std::endl; 
   SetStepLength(steplength1,steplength2);
   for(int i=n;i-->0;) fC1[i]=fC1[i]+RLowerBound;
   
   // set potential for electrodes
   for(int i=n-1;i>=n-n1;i--) {
      fIsFixed[i]=true;
      fPotential[i]=V0;
      if(fC1[n-1-i]>=PointBegin&&fC1[n-1-i]<=PointEnd) {
         fPotential[n-1-i]=V1;
         fIsFixed[n-1-i]=true;
      }
   }
   for(int i=0;i<n-n1;i=i+n1) {
      fIsFixed[i]=true;
      fIsFixed[i+n1-1]=true;
      fPotential[i]=V0;
      fPotential[i+n1-1]=V0;
   }
}
//_____________________________________________________________________________
//
bool PointContactRZ::CalculateField(EMethod method)
{
   if(!fIsLoaded)Initialize();
   return RZ::CalculateField(method);
}
