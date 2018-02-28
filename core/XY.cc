#include <TF2.h>
#include <TFile.h>
#include <TChain.h>
#include <TVectorD.h>

#include "XY.h"
#include "Units.h"
using namespace GeFiCa;

XY::XY(unsigned short nx, unsigned short ny): X(nx*ny), n2(ny),
   fE2(0), fC2(0), fDistanceToLeft(0), fDistanceToRight(0)
{
   Impurity="0*y";
   //claim a 2D field with n1*n2 Grid
   n=nx*ny; 
   n1=nx;
   fE2=new double[n];
   fC2=new double[n];
   fDistanceToLeft=new double[n];
   fDistanceToRight=new double[n];
}
//_____________________________________________________________________________
//
XY::~XY()
{
   if (fE2) delete[] fE2;
   if (fC2) delete[] fC2;
   if (fDistanceToLeft) delete[] fDistanceToLeft;
   if (fDistanceToRight) delete[] fDistanceToRight;
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
      fDistanceToLeft[i]=steplength2;
      fDistanceToRight[i]=steplength2;
   }
}
//_____________________________________________________________________________
//
#include <iostream>
using namespace std;
void XY::SOR2(int idx,bool elec)
{

   // 2nd-order Runge-Kutta Successive Over-Relaxation
   if (fIsFixed[idx])return;
   double density=fImpurity[idx]*Qe;
   double h2=fDistanceToPrevious[idx];
   double h3=fDistanceToNext[idx];
   double h4=fDistanceToLeft[idx];
   double h1=fDistanceToRight[idx];
   double Pym1,Pyp1,Pxm1,Pxp1;
   if(idx>=n1)Pym1=fPotential[idx-n1];
   else Pym1=fPotential[idx];
   if(idx>=n-n1)Pyp1=fPotential[idx];
   else Pyp1=fPotential[idx+n1];
   if(idx%n1==0)Pxm1=fPotential[idx];
   else Pxm1=fPotential[idx-1];
   if(idx%n1==n1-1)Pxp1=fPotential[idx];
   else Pxp1=fPotential[idx+1];
   //double tmp=(density/epsilon+1/fC1[idx]*(Pxp1-Pxm1)/(h2+h3)+(Pxp1/h2+Pxm1/h3)*2/(h2+h3)+(Pyp1/h1+Pym1/h4)*2/(h1+h4))/
   double tmp=(density/epsilon+(Pxp1/h2+Pxm1/h3)*2/(h2+h3)+(Pyp1/h1+Pym1/h4)*2/(h1+h4))/
      ((1/h2+1/h3)*2/(h2+h3)+(1/h1+1/h4)*2/(h1+h4));
   // cout<<tmp<<endl;
   fPotential[idx]=Csor*(tmp-fPotential[idx])+fPotential[idx];
   if(elec)
   {
      fE1[idx]=(Pxp1-Pxm1)/(h2+h3);
      fE2[idx]=(Pyp1-Pym1)/(h1+h4);
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
   // for (int i=0;i<n;i++)
   //  cout<<fDistanceToNext[i]<<" "<<i<<endl;

   int idx=FindIdx(tarx,tary,0,n2-1);

   //cout<<"index:"<<idx<<endl;
   //test
   /*cout<<"(0,0)c1: "<<fC1[idx]<<" c2: "<<fC2[idx]<<" p: "<<fPotential[idx]<<endl;
   cout<<"(1,0)c1: "<<fC1[idx-1]<<" c2: "<<fC2[idx-1]<<" p: "<<fPotential[idx-1]<<endl;
   cout<<"(0,1)c1: "<<fC1[idx-n1]<<" c2: "<<fC2[idx-n1]<<" p: "<<fPotential[idx-n1]<<endl;
   cout<<"(1,1)c1: "<<fC1[idx-n1-1]<<" c2: "<<fC2[idx-n1-1]<<" p: "<<fPotential[idx-n1-1]<<endl;
   */
   
   //cout<<idx<<" "<<n<<endl;
   double ab=(-tarx+fC1[idx])/fDistanceToNext[idx];
   double aa=1-ab;
   double ba=(-tary+fC2[idx])/fDistanceToRight[idx];
   //cout<<"right"<<fDistanceToRight[idx]<<endl;
   //cout<<"next"<<fDistanceToNext[idx]<<endl;
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
   if(idx/n1+1==n1){tar1=0;tar3=0;}
   else {tar1=tar[idx-1];}
   if(idx>n-n1){tar2=0;tar3=0;}
   else {tar2=tar[idx-n1];}
   if (tar3==-1)tar3=tar[idx-n1-1];
   //cout<<tar0<<" "<<tar1<<" "<<tar2<<" "<<tar3<<endl;
   //cout<<aa<<" "<<ab<<" "<<ba<<" "<<bb<<endl;
   return (tar0*ab+tar1*aa)*bb+(tar2*ab+tar3*aa)*ba;
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
   double E2s,C2s,StepLeft,StepRight;
   TBranch *be2 = tree->Branch("e2",&E2s,"e2/D"); // Electric field in y
   TBranch *bc2 = tree->Branch("c2",&C2s,"c2/D"); // persition in y
   TBranch *bsl=tree->Branch("sl",&StepLeft,"StepLeft/D"); // Step length to next point in x
   TBranch *bsr=tree->Branch("sr",&StepRight,"StepRight/D"); // Step length to before point in x

   for(int i=0;i<n;i++) {
      E2s=fE2[i];
      C2s=fC2[i];
      StepLeft=fDistanceToLeft[i];
      StepRight=fDistanceToRight[i];
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
   n2		=(int)	v[8];

   TChain *t =new TChain("t");
   t->Add(fin);
   double fEy,fPy,fstepleft,fstepright;
   t->SetBranchAddress("c2",&fPy);
   t->SetBranchAddress("e2",&fEy);
   t->SetBranchAddress("sl",&fstepleft);
   t->SetBranchAddress("sr",&fstepright);


   fE2=new double[n];
   fC2=new double[n];
   fDistanceToRight=new double[n];
   fDistanceToLeft=new double[n];

   for (int i=0;i<n;i++) {
      t->GetEntry(i);
      fE2[i]=fEy;
      fC2[i]=fPy;
      fDistanceToRight[i]=fstepright;
      fDistanceToLeft[i]=fstepright;


   }
   file->Close();
   delete file;
}
//_____________________________________________________________________________
//
void XY::SetImpurity(TF2 * Im)
{
   for(int i=n;i-->0;) {
      fImpurity[i]=Im->Eval(fC1[i],fC2[i]);
   }
}

void XY::Impuritystr2tf()
{
   const char* expression = Impurity;
   TF2 * IM=new TF2("f",expression);
   SetImpurity(IM);
}
