#include "Planar2D.h"
using namespace GEFICA;

void Planar2D::SetVoltage(double dVoltage)
{
   for(int i=0;i<n2;i++) {
      fIsFixed[i*n1]=true;
      fIsFixed[(i+1)*n1-1]=true;
      fPotential[i*n1]=dVoltage;
   }
}
