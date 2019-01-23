#include <TF3.h>
#include <TFile.h>
#include <TChain.h>
#include <TVectorD.h>

#include "XY.h"
#include "Units.h"
using namespace GeFiCa;

XY::XY(int nx, int ny): X(nx*ny), n2(ny), fE2(0), fC2(0), fdC2p(0), fdC2m(0)
{
   n1=nx; // n1 is set to nx*ny through X constructor, it is fixed here
   fE2=new double[n];
   fC2=new double[n];
   fdC2m=new double[n];
   fdC2p=new double[n];
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
   for (int i=0;i<n;i++) {
      if(i>n1-1)fC2[i]=fC2[i-n1]+steplength2;
      else fC2[i]=0;
      if(i%n1==0)fC1[i]=0;
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
void XY::SOR2(int idx,bool NotImpurityPotential)
{
   // 2nd-order Runge-Kutta Successive Over-Relaxation
   if (fIsFixed[idx])return;
   double density=fImpurity[idx]*Qe;
   double h2=fdC1m[idx];
   double h3=fdC1p[idx];
   double h4=fdC2m[idx];
   double h1=fdC2p[idx];
   double pym,pyp,pxm,pxp;
   if(idx>=n1)pym=fPotential[idx-n1];
   else pym=fPotential[idx];
   if(idx>=n-n1)pyp=fPotential[idx];
   else pyp=fPotential[idx+n1];
   if(idx%n1==0)pxm=fPotential[idx];
   else pxm=fPotential[idx-1];
   if(idx%n1==n1-1)pxp=fPotential[idx];
   else pxp=fPotential[idx+1];
   //double tmp=(density/epsilon+1/fC1[idx]*(pxp-pxm)/(h2+h3)+(pxp/h2+pxm/h3)*2/(h2+h3)+(pyp/h1+pym/h4)*2/(h1+h4))/
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
   
      //over relax
   //fPotential[idx]=Csor*(tmp-fPotential[idx])+fPotential[idx];
   //if need calculate depleted voltage
   double oldP=fPotential[idx];
   tmp=Csor*(tmp-oldP)+oldP;
   if(tmp<min)
   {
      fPotential[idx]=min;
      fIsDepleted[idx]=false;
   }
   else if(tmp>max)
   {
      fPotential[idx]=max;
      fIsDepleted[idx]=false;
   }
   else
      fIsDepleted[idx]=true;
   if(fIsDepleted[idx]||!NotImpurityPotential)
   {
      fPotential[idx]=tmp;
   }
}
//_____________________________________________________________________________
//
int XY::FindIdx(double tarx,double tary ,int ybegin,int yend)
{
   //search using binary search
   // if(ybegin>=yend)cout<<"to x"<<ybegin<<" "<<yend<<endl;;
   if(ybegin>=yend)return X::FindIdx(tarx,yend*n1,(yend+1)*n1-1);
   int mid=((ybegin+yend)/2);
   if(fC2[mid*n1]>=tary){//cout<<"firsthalf"<<ybegin<<" "<<yend<<endl; 
      return FindIdx(tarx,tary,ybegin,mid);
   }
   else{//cout<<"senondhalf"<<ybegin<<" "<<yend<<endl; 
      return FindIdx(tarx,tary,mid+1,yend);}
}
//_____________________________________________________________________________
//
double XY::GetData(double tarx, double tary, EOutput output)
{
   int idx=FindIdx(tarx,tary,0,n2-1);

   //test
   //cout<<"index:"<<idx<<endl;
   //cout<<"(0,0)c1: "<<fC1[idx]<<" c2: "<<fC2[idx]<<" p: "<<fPotential[idx]<<endl;
   //cout<<"(0,1)c1: "<<fC1[idx-1]<<" c2: "<<fC2[idx-1]<<" p: "<<fPotential[idx-1]<<endl;
   //cout<<"(1,0)c1: "<<fC1[idx-n1]<<" c2: "<<fC2[idx-n1]<<" p: "<<fPotential[idx-n1]<<endl;
   //cout<<"(1,1)c1: "<<fC1[idx-n1-1]<<" c2: "<<fC2[idx-n1-1]<<" p: "<<fPotential[idx-n1-1]<<endl;
   //
   //cout<<idx<<" "<<n<<endl;
   double ab=(-tarx+fC1[idx])/fdC1m[idx];
   double aa=1-ab;
   double ba=(-tary+fC2[idx])/fdC2m[idx];
   //cout<<"left"<<fdC2m[idx]<<endl;
   //cout<<"right "<<fdC2p[idx]<<endl;
   //cout<<"next"<<fdC1p[idx]<<endl;
   double bb=1-ba;
   double tar0,tar1,tar2,tar3,*tar=NULL;
   switch(output) {
      case 0:tar= fImpurity;break;
      case 1:tar= fPotential;break;
      case 2:tar= fE1;break;
      case 3:tar= fE2;break;
      default:break;
   }
   tar3=-1;
   tar0=tar[idx];
   if (idx%n1==0){tar1=0;tar3=0;}
   else {tar1=tar[idx-1];}
   if(idx<n1) {tar2=0;tar3=0;}
   else {tar2=tar[idx-n1];}
   if (tar3==-1)tar3=tar[idx-n1-1];
   //cout<<tar0<<" "<<tar1<<" "<<tar2<<" "<<tar3<<endl;
   //cout<<tarx<<", "<<tary<<endl;
   //cout<<aa<<" "<<ab<<" "<<ba<<" "<<bb<<endl;
   //
   //if (fC1[idx]>0. && fC2[idx]>3.445){
   //   cout<<tary-0.21<<endl;
   //   abort();
   //}
   return (tar1*ab+tar0*aa)*bb+(tar3*ab+tar2*aa)*ba;
}
//_____________________________________________________________________________
//
void XY::SaveField(const char * fout)
{
   X::SaveField(fout);
   TFile *file=new TFile(fout,"update");
   TVectorD  v=*(TVectorD*)file->Get("v");
   v[8]=(double)n2;
   v.Write("v");
   TTree * tree=(TTree*)file->Get("t");
   double E2s,C2s,dC2m,dC2p;
   TBranch *be2 = tree->Branch("e2",&E2s,"e2/D");
   TBranch *bc2 = tree->Branch("c2",&C2s,"c2/D"); 
   TBranch *bsl=tree->Branch("dC2m",&dC2m,"dC2m/D"); 
   TBranch *bsr=tree->Branch("dC2p",&dC2p,"dC2p/D");

   for(int i=0;i<n;i++) {
      E2s=fE2[i];
      C2s=fC2[i];
      dC2m=fdC2m[i];
      dC2p=fdC2p[i];
      be2->Fill();
      bc2->Fill();
      bsl->Fill();
      bsr->Fill();
   }
   file->Write();
   file->Close();
   delete file;
}
//_____________________________________________________________________________
//
void XY::LoadField(const char * fin)
{
   //will calculate electric field after load
   X::LoadField(fin);
   TFile *file=new TFile(fin);
   TVectorD *v1=(TVectorD*)file->Get("v");
   double * v=v1->GetMatrixArray();
   n2	= (int)v[8];

   TChain *t =new TChain("t");
   t->Add(fin);
   double Ey,Py,dC2m,dC2p;
   t->SetBranchAddress("c2",&Py);
   t->SetBranchAddress("e2",&Ey);
   t->SetBranchAddress("dC2m",&dC2m);
   t->SetBranchAddress("dC2p",&dC2p);

   fE2=new double[n];
   fC2=new double[n];
   fdC2p=new double[n];
   fdC2m=new double[n];

   for (int i=0;i<n;i++) {
      t->GetEntry(i);
      fE2[i]=Ey;
      fC2[i]=Py;
      fdC2p[i]=dC2p;
      fdC2m[i]=dC2m;
   }
   file->Close();
   delete file;
}
//_____________________________________________________________________________
//
void XY::SetImpurity(TF3 *Im)
{
   Initialize();
   for (int i=n;i-->0;) fImpurity[i] = Im->Eval(fC1[i], fC2[i]);
}
//_____________________________________________________________________________
//
bool XY::CalculateField(int idx)
{
   if (!X::CalculateField(idx)) return false;
   if (fdC2p[idx]==0 || fdC2m[idx]==0) return false;

   if (idx%(n1*n2)<n1) // C2 lower boundary
      fE2[idx]=(fPotential[idx]-fPotential[idx+n1])/fdC2p[idx];
   else if (idx%(n1*n2)>=n-n1) // C2 upper boundary
      fE2[idx]=(fPotential[idx]-fPotential[idx-n1])/fdC2m[idx];
   else { // bulk
      fE2[idx]=(fPotential[idx-n1]-fPotential[idx+n1])/(fdC2m[idx]+fdC2p[idx]);
   }
   return true;
}
