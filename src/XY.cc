#include <TTree.h>

#include "XY.h"
#include "Units.h"
using namespace GeFiCa;

XY::XY(int nx, int ny, const char *name, const char *title)
   : X(nx*ny, name, title)
{
   fN1=nx; // fN1 is set to nx*ny through X constructor, it is fixed here
   fN2=ny;
}
//_____________________________________________________________________________
//
XY::~XY()
{
   if (fE2) delete[] fE2;
   if (fC2) delete[] fC2;
   if (fdC2m) delete[] fdC2m;
   if (fdC2p) delete[] fdC2p;
}
//_____________________________________________________________________________
//
void XY::SetStepLength(double steplength1,double steplength2)
{
   //set field step length
   X::SetStepLength(steplength1);
   for (int i=0;i<fN;i++) {
      if(i>fN1-1)fC2[i]=fC2[i-fN1]+steplength2;
      else fC2[i]=0;
      if(i%fN1==0)fC1[i]=0;
      else fC1[i]=fC1[i-1]+steplength1;

      fE2[i]=0;
      fdC2m[i]=steplength2;
      fdC2p[i]=steplength2;
   }
}
//_____________________________________________________________________________
//
#include <iostream>
using namespace std;
void XY::DoSOR2(int idx)
{
   // 2nd-order Runge-Kutta Successive Over-Relaxation
   if (fIsFixed[idx])return;
   double density=fImpurity[idx]*Qe;
   double h2=fdC1m[idx];
   double h3=fdC1p[idx];
   double h4=fdC2m[idx];
   double h1=fdC2p[idx];
   double pym,pyp,pxm,pxp;
   if(idx>=fN1)pym=fV[idx-fN1];
   else pym=fV[idx];
   if(idx>=fN-fN1)pyp=fV[idx];
   else pyp=fV[idx+fN1];
   if(idx%fN1==0)pxm=fV[idx];
   else pxm=fV[idx-1];
   if(idx%fN1==fN1-1)pxp=fV[idx];
   else pxp=fV[idx+1];
   double tmp=(density/epsilon+(pxp/h2+pxm/h3)*2/(h2+h3)+(pyp/h1+pym/h4)*2/(h1+h4))/
      ((1/h2+1/h3)*2/(h2+h3)+(1/h1+1/h4)*2/(h1+h4));
   //find minmium and maxnium of all five grid, the new one should not go overthem.
   //find min
   double min=pxm;
   double max=pxm;
   if(min>pxp)min=pxp;
   if (min>pyp)min=pyp;
   if (min>pym)min=pym;
   
   //find max
   if(max<pxp)max=pxp;
   if (max<pyp)max=pyp;
   if (max<pym)max=pym;
   //if tmp is greater or smaller than max and min, set tmp to it.
   
   //fV[idx]=Csor*(tmp-fV[idx])+fV[idx];
   //if need calculate depleted voltage
   double oldP=fV[idx];
   tmp=Csor*(tmp-oldP)+oldP;
   if(tmp<min) {
      fV[idx]=min;
      fIsDepleted[idx]=false;
   } else if(tmp>max) {
      fV[idx]=max;
      fIsDepleted[idx]=false;
   } else
      fIsDepleted[idx]=true;

   if(fIsDepleted[idx]||V0==V1) fV[idx]=tmp;
}
//_____________________________________________________________________________
//
int XY::FindIdx(double tarx,double tary ,int ybegin,int yend)
{
   //search using binary search
   // if(ybegin>=yend)cout<<"to x"<<ybegin<<" "<<yend<<endl;;
   if(ybegin>=yend)return X::FindIdx(tarx,yend*fN1,(yend+1)*fN1-1);
   int mid=((ybegin+yend)/2);
   if(fC2[mid*fN1]>=tary){//cout<<"firsthalf"<<ybegin<<" "<<yend<<endl; 
      return FindIdx(tarx,tary,ybegin,mid);
   }
   else{//cout<<"senondhalf"<<ybegin<<" "<<yend<<endl; 
      return FindIdx(tarx,tary,mid+1,yend);}
}
//_____________________________________________________________________________
//
double XY::GetData(double x, double y, double z, double *data)
{
   int idx=FindIdx(x,y,0,fN2-1);

   double ab=(-x+fC1[idx])/fdC1m[idx];
   double aa=1-ab;
   double ba=(-y+fC2[idx])/fdC2m[idx];
   double bb=1-ba;
   double tar0,tar1,tar2,tar3;
   tar3=-1;
   tar0=data[idx];
   if (idx%fN1==0){tar1=0;tar3=0;}
   else {tar1=data[idx-1];}
   if(idx<fN1) {tar2=0;tar3=0;}
   else {tar2=data[idx-fN1];}
   if (tar3==-1)tar3=data[idx-fN1-1];
   return (tar1*ab+tar0*aa)*bb+(tar3*ab+tar2*aa)*ba;
}
//_____________________________________________________________________________
//
bool XY::CalculateField(int idx)
{
   if (!X::CalculateField(idx)) return false;
   if (fdC2p[idx]==0 || fdC2m[idx]==0) return false;

   if (idx%(fN1*fN2)<fN1) // C2 lower boundary
      fE2[idx]=(fV[idx]-fV[idx+fN1])/fdC2p[idx];
   else if (idx%(fN1*fN2)>=fN-fN1) // C2 upper boundary
      fE2[idx]=(fV[idx]-fV[idx-fN1])/fdC2m[idx];
   else { // bulk
      fE2[idx]=(fV[idx-fN1]-fV[idx+fN1])/(fdC2m[idx]+fdC2p[idx]);
   }
   return true;
}
//_____________________________________________________________________________
//
TGraph * XY::GetFieldLineFrom(double x, double y)
{

}

//_____________________________________________________________________________
//
//when dex/Ex  =PresentDiffferentOnE?
//dex=gete1(x+dx)-gete1(x)
