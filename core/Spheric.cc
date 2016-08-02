#include "Spheric.h"
using namespace GEFICA;
void Spheric::Create(double r0,double r1)
{
  RThetaPhi::CreateGridWithFixedStepLength((r1-r0)/(n1-1));
  for (int i=0;i<n;i++)
  {
    fC1[i]+=r0;
  }
}
void Spheric::SetVoltage(double v1,double v2)
{
  for (int i=0;i<n;i=i+n1)
  {
    fPotential[i]=v1;
    fPotential[i+n1-1]=v2;
    fIsFixed[i]=true;
    fIsFixed[i+n1-1]=true;
  }
}
