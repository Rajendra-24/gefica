#include <TF3.h>
#include <TFile.h>
#include <TChain.h>
#include <TVectorD.h>

#include "RhoPhiZ.h"
using namespace GEFICA;
using namespace std;
void RhoPhiZ::CreateGridWithFixedStepLength(double steplength)
{
   XY::CreateGridWithFixedStepLength(steplength);
   fE3=new double[n];
   fC3=new double[n];
   fDistanceToUp=new double[n];
   fDistanceToDown=new double[n];
   for (int i=0;i<n;i++) {
      if(i/n1*n2==0)fC3[i]=0;
      else fC3[i]=fC3[i-n1*n2]+steplength;
      if((i%(n1*n2))/n1!=0)fC2[i]=fC2[i-n1]+3.14159265*2/(n2-1);
      else fC2[i]=0;
      if(i%n1==0)fC1[i]=0;
      else fC1[i]=fC1[i-1]+steplength;

      fE3[i]=0;
      fDistanceToLeft[i]=3.14159265*2/(n2-1);
      fDistanceToRight[i]=3.14159265*2/(n2-1);
      fDistanceToUp[i]=steplength;
      fDistanceToDown[i]=steplength;
   }
}

void RhoPhiZ::RK2(int idx,bool elec)
{//need update
   if (fIsFixed[idx])return;
   double density=fImpurity[idx]*1.6e12;
   double h2=fDistanceToPrevious[idx];
   double h3=fDistanceToNext[idx];
   double h4=fDistanceToLeft[idx];
   double h1=fDistanceToRight[idx];
   double h0=fDistanceToDown[idx];
   double h5=fDistanceToUp[idx];
   double Pym1,Pyp1,Pxm1,Pxp1,Pzp1,Pzm1;
   if(idx<n1*n2)Pzm1=fPotential[idx];
   else Pzm1=fPotential[idx-n1*n2];
   if(idx>=n-n1*n2)Pzp1=fPotential[idx];
   else Pzp1=fPotential[idx+n1*n2];
   if(idx%(n1*n2)>(n1*n2)-n1-1) Pyp1=fPotential[idx-n1*n2+n1];
   else Pyp1=fPotential[idx+n1];
   if(idx%(n1*n2)<n1)Pym1=fPotential[idx+n1*n2-n1];
   else Pym1=fPotential[idx-n1];
   if((idx%(n1*n2))%n1==n1-1)Pxp1=fPotential[idx];
   else Pxp1=fPotential[idx+1];
   if((idx%(n1*n2))%n1==0)Pxm1=fPotential[idx];
   else Pxm1=fPotential[idx-1];
   double r=fC1[idx];
   double tmp= (-density/2/epsilon+(Pxp1-Pxm1)/(2*r*(h2+h3))+Pxp1/h3/(h2+h3)+Pxm1/h2/(h2+h3)+Pyp1/h4/(h1+h4)/r/r+Pym1/h1/(h1+h4)/r/r+Pzp1/h5/(h0+h5)+Pzm1/h0/(h0+h5))
     /(1/h2/(h2+h3)+1/h3/(h2+h3)+1/h4/(h1+h4)/r/r+1/h1/(h1+h4)/r/r+1/h0/(h0+h5)+1/h5/(h0+h5));
   fPotential[idx]=Csor*(tmp-fPotential[idx])+fPotential[idx];
   if(elec)
   {
     fE1[idx]=(Pxp1-Pxm1)/(h2+h3);
     fE2[idx]=(Pyp1-Pym1)/(h1+h4);
     fE3[idx]=(Pzp1-Pzm1)/(h0+h5);
   }
}

double RhoPhiZ::GetData(double tarx, double tary, double tarz,int thing)
{
   int idx=FindIdx(tarx,tary,tarz,0,n);
   double ab=(tarx-fC1[idx])/fDistanceToNext[idx];
   double aa=1-ab;
   double ba=(tary-fC2[idx])/fDistanceToRight[idx];
   double bb=1-ba;
   double ac=(tarz-fC3[idx])/fDistanceToUp[idx];
   double ca=1-ac;
   double tar0,tar1,tar2,tar3,tar4,tar5,tar6,tar7,*tar=NULL;
   switch(thing)
   {
      case 0:tar= fImpurity;break;
      case 1:tar= fPotential;break;
      case 2:tar= fE1;break;
      case 3:tar= fE2;break;
      case 4:tar= fE3;break;
   }
   tar3=-1;
   tar5=-1;
   tar6=-1;
   tar7=-1;
   tar0=tar[idx];
   if(idx>=(n-n1*n2)){tar4=0;tar5=0;tar6=0;tar7=0;}
   else{tar4=tar[idx+n1*n2];}
   if(idx%(n1*n2)%n1==n1-1){tar2=0;tar3=0;tar6=0;tar7=0;}
   else{tar2=tar[idx+n1];}
   if(idx%(n1*n2)/n1==n2-1){tar1=0;tar3=0;tar5=0;tar7=0;}
   if(tar3==-1)tar3=tar[idx+n1+1];
   if(tar5==-1)tar5=tar[idx+n1*n2+1];
   if(tar6==-1)tar6=tar[idx+n1*n2+n1];
   if(tar7==-1)tar7=tar[idx+n1*n2+n1+1];
   return ((tar0*aa+tar1*ab)*ba+(tar2*aa+tar3*ab)*bb)*ac+((tar4*aa+tar5*ab)*ba+(tar6*aa+tar7*ab)*bb)*ca;
}
