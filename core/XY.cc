#include <TFile.h>
#include <TChain.h>
#include <TVectorD.h>

#include "XY.h"
using namespace GEFICA;

XY::XY(unsigned short n1, unsigned short n2): X(n1), n2(n2),
   fE2(0), fC2(0), fDistanceToLeft(0), fDistanceToRight(0)
{n=n1*n2;}

XY::~XY()
{
   if (fE2) delete[] fE2;
   if (fC2) delete[] fC2;
   if (fDistanceToLeft) delete[] fDistanceToLeft;
   if (fDistanceToRight) delete[] fDistanceToRight;
}

void XY::CreateGridWithFixedStepLength(double steplength)
{
   X::CreateGridWithFixedStepLength(steplength);
   fE2=new double[n];
   fC2=new double[n];
   fDistanceToLeft=new double[n];
   fDistanceToRight=new double[n];
   for (int i=0;i<n;i++) {
      if(i>n1-1)fC2[i]=fC2[i-n1]+steplength;
      else fC2[i]=0;
      if(i%n1==0)fC1[i]=0;
      else fC1[i]=fC1[i-1]+steplength;

      fE2[i]=0;
      fDistanceToLeft[i]=steplength;
      fDistanceToRight[i]=steplength;
   }
}

void XY::Update(int idx)
{//need update
   if (fIsFixed[idx])return;
   double density=fImpurity[idx]*1.6e12;
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
   double tmp=((h1+h4)*(h1*h2*h4*Pxp1+h1*h3*h4*Pxm1)+(h2+h3)*(h1*h2*h3*Pyp1+h2*h3*h4*Pym1)-density/epsilon*(h1+h4)*(h2+h3)*h1*h2*h3*h4)/((h1+h4)*(h1*h2*h4+h1*h3*h4)+(h2+h3)*(h1*h2*h3+h2*h3*h4));
   fPotential[idx]=Csor*(tmp-fPotential[idx])+fPotential[idx];
   fE1[idx]=(Pxp1-Pxm1)/(h2+h3);
   fE2[idx]=(Pyp1-Pym1)/(h1+h4);
}

int XY::FindIdx(double tarx,double tary ,int ybegin,int yend)
{
   if(ybegin>=yend)return X::FindIdx(tarx,ybegin,ybegin+n1-1);
   int mid=((ybegin/n1+yend/n1)/2)*n1;
   if(fC2[mid]>=tary)return FindIdx(tarx,tary,ybegin,mid);
   else return FindIdx(tarx,tary,mid+1,yend);
}

double XY::GetData(double tarx, double tary, int thing)
{
   int idx=FindIdx(tarx,tary,0,n);
   double ab=(tarx-fC1[idx])/fDistanceToNext[idx];
   double aa=1-ab;
   double ba=(tary-fC2[idx])/fDistanceToRight[idx];
   double bb=1-ba;
   double tar0,tar1,tar2,tar3,*tar=NULL;
   switch(thing)
   {
      case 0:tar= fImpurity;break;
      case 1:tar= fPotential;break;
      case 2:tar= fE1;break;
      case 3:tar= fE2;break;
   }
   tar3=-1;
   tar0=tar[idx];
   if(idx/n1+1==n1){tar1=0;tar3=0;}
   else {tar1=tar[idx+1];}
   if(idx>n-n1){tar2=0;tar3=0;}
   else {tar2=tar[idx+n1];}
   if (tar3==-1)tar3=tar[idx+n1+1];
   return (tar0*aa+tar1*ab)*ba+(tar2*aa+tar3*ab)*bb;
}
void XY::Save(const char * fout)
{
   X::Save(fout);
   TFile *file=new TFile(fout,"update");
   TVectorD  v=*(TVectorD*)file->Get("v");
   v[8]=(double)n2;
   v.Write();
   TTree * tree=(TTree*)file->Get("t");
   double E2s,C2s;
   tree->Branch("e2",&E2s,"e2/D"); // Electric field in y
   tree->Branch("c2",&C2s,"c2/D"); // persition in y
   for(int i=0;i<n;i++) {
      E2s=fE2[i];
      C2s=fC2[i];
      tree->Fill();
   }
   file->Write();
   file->Close();
   delete file;

}
void XY::Load(const char * fin)
{
   X::Load(fin);
   TFile *file=new TFile(fin);
   TVectorD *v1=(TVectorD*)file->Get("v");
   double * v=v1->GetMatrixArray();
   n2		=(int)	v[8];

   TChain *t =new TChain("t");
   t->Add(fin);
   double fEy,fPy;
   t->SetBranchAddress("c2",&fPy);
   t->SetBranchAddress("e2",&fEy);

   fE2=new double[n];
   fC2=new double[n];

   for (int i=0;i<n;i++) {
      t->GetEntry(i);
      fE2[i]=fEy;
      fC2[i]=fPy;
   }
   file->Close();
   delete file;
}
void XY::SetImpurity(TF2 * Im)
{
   for(int i=n;i-->0;) {
      fImpurity[i]=Im->Eval((double)fC1[i],(double)fC2[i]);
   }
}
