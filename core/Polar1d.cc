#include "Polar1d.h"
#include "iostream"
using namespace GEFICA;
using namespace std;


void Polar1d::SetVoltage(double anode_voltage, double cathode_voltage)
{
   //double stepLength=Thickness/(n-1);
   //CreateGridWithFixedStepLength(stepLength);
   fIsFixed[0]=true;
   fIsFixed[n-1]=true;
   double slope = (cathode_voltage-anode_voltage)/(n-1);
   for (int i=0; i<n; i++) {
      fPotential[i]=anode_voltage+slope*i;
   }
}
#include  <cmath>
bool Polar1d::Analyic()
{
  double density=fImpurity[1]*1.6e-19;
   double cnst1=(fPotential[n-1]-fPotential[0]-density*(fC1[n-1]*fC1[n-1]-fC1[0]*fC1[0])/epsilon/4)/(log(fC1[n-1]/fC1[0]));
   double cnst2=fPotential[0]-density*fC1[0]*fC1[0]/epsilon/4-cnst1*log(fC1[0]);
   for (int i=0; i<n; i++) {
      fPotential[i] = fImpurity[i]*1.6e-19/4/epsilon*fC1[i]*fC1[i]+cnst1*log(fC1[i])-cnst2;
      fE1[i]=(fPotential[i+1]-fPotential[i-1])/(fDistanceToNext[i]+fDistanceToPrevious[i]);
   }
   return true;
}
void Polar1d::Create(double r0,double r1)
{
  X::CreateGridWithFixedStepLength((r1-r0)/(n-1));
  for(int i=0;i<n;i++)
  {
    fC1[i]=fC1[i]+r0;
  }
}
