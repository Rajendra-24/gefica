#include <TMath.h>
#include <TTree.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TStopwatch.h>

#include "Grid.h"
#include "Units.h"
#include "Detector.h"
using namespace GeFiCa;

TGraph* FieldLine::GetGraph()
{
   if (GetN()<1) {
      Warning("GetGraph", "No data for graph. Return 0!");
      return 0;
   }
   if (fGl) { return fGl; }
   fGl = new TGraph(GetN(),C1.data(),C2.data());
   fGl->SetName(GetName()); fGl->SetTitle(GetTitle());
   return fGl;
}
//_____________________________________________________________________________
//

Grid::Grid(size_t n1, size_t n2, size_t n3) : N1(n1), N2(n2), N3(n3),
   MaxIterations(5000), RelaxationFactor(1.95), Precision(1e-7*volt),
   fTree(0), fDetector(0)
{
   // pick up a good style to modify
   gStyle->SetName("GeFiCa");
   gStyle->SetLegendBorderSize(0);
   gStyle->SetLegendFont(132);
   gStyle->SetLabelFont(132,"XYZ");
   gStyle->SetTitleFont(132,"XYZ");
   gStyle->SetLabelSize(0.05,"XYZ");
   gStyle->SetTitleSize(0.05,"XYZ");
   gStyle->SetTitleOffset(-0.4,"Z");
   gStyle->SetPadTopMargin(0.02);
   // create a smoother palette than the default one
   const int nRGBs = 5;
   const int nCont = 255;
   double stops[nRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
   double red[nRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
   double green[nRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
   double blue[nRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
   TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, nCont);
   gStyle->SetNumberContours(nCont);
}
//_____________________________________________________________________________
//
Grid& Grid::operator+=(Grid& other)
{
   if (GetN()!=other.GetN()) {
      Warning("+=", 
            "Numbers of points in two grids are different, return *this.");
      return *this; 
   }
   for (size_t i=0; i<GetN(); i++) {
      Vp[i]=Vp[i]+other.Vp[i];
      Src[i]=Src[i]+other.Src[i];
   }
   return *this;
}
//_____________________________________________________________________________
//
Grid& Grid::operator*=(double scale)
{
   for (size_t i=0; i<GetN(); i++) Vp[i]=Vp[i]*scale;
   return *this;
}
//_____________________________________________________________________________
//
size_t Grid::GetIdxOfMaxV()
{
   double max=Vp[0]; size_t idx=0;
   for (size_t i=1; i<GetN(); i++) {
      if (Vp[i]>max) {
         idx=i;
         max=Vp[i];
      }
   }
   return idx;
}
//_____________________________________________________________________________
//
size_t Grid::GetIdxOfMinV()
{
   double min=Vp[0]; size_t idx=0;
   for (size_t i=1; i<GetN(); i++) {
      if (Vp[i]<min) {
         idx=i;
         min=Vp[i];
      }
   }
   return idx;
}
//_____________________________________________________________________________
//
bool Grid::IsDepleted()
{
   for(size_t i=0;i<GetN();i++) {
      // calculate one more time in case of adding two fields together,
      // one is depleted, the other is not
      OverRelaxAt(i);
      if (fIsDepleted[i]=false) return false;
   }
   return true;
}
//_____________________________________________________________________________
//
void Grid::GetBoundaryConditionFrom(Detector &detector)
{
   if (GetN()>0) { // this function can only be called once
      Warning("GetBoundaryConditionFrom", "has been called. Do nothing.");
      return;
   }
}
//_____________________________________________________________________________
//
void Grid::SuccessiveOverRelax()
{
   if (dC1p.size()<1) {
      Error("SuccessiveOverRelax", "Grid is not ready. "
            "Please call GetBoundaryConditionFrom(Detector&) first.");
      abort();
   }
   Info("SuccessiveOverRelax","Start...");
   double cp=1; // current presision
   fIterations=0;
   TStopwatch watch; watch.Start();
   while (fIterations<MaxIterations) {
      if (fIterations%100==0)
         Printf("%4zu steps, precision: %.1e (target: %.0e)",
               fIterations, cp, Precision);
      double numerator=0, denominator=0;
      for (size_t i=0; i<GetN(); i++) {
         double old=Vp[i]; // save old value of Vp[i]
         OverRelaxAt(i); // update Vp[i]
         if(old>0) denominator+=old; else denominator-=old;
         double diff=Vp[i]-old;
         if(diff>0) numerator+=(diff); else numerator-=(diff);
      }
      cp = numerator/denominator;
      fIterations++;
      if (cp<Precision) break;
   }
   CalculateE();
   Printf("%4zu steps, precision: %.1e (target: %.0e)",
         fIterations, cp, Precision);
   Info("SuccessiveOverRelax", "CPU time: %.1f s", watch.CpuTime());
}
//_____________________________________________________________________________
//
void Grid::SolveAnalytically()
{
   if (Src[0]!=Src[N1-1]) {
      Error("SolveAnalytically", "can't handle changing impurity.");
      abort();
   }
}
//_____________________________________________________________________________
//
size_t Grid::GetIdxOfPointToTheRightOf(double c1,size_t begin,size_t end) const
{
   if (end==0) end=N1-1;
   if (begin>=end)return end;
   size_t mid=(begin+end)/2;
   if(C1[mid]>=c1)return GetIdxOfPointToTheRightOf(c1,begin,mid);
   else return GetIdxOfPointToTheRightOf(c1,mid+1,end);
}
//_____________________________________________________________________________
//
size_t Grid::GetIdxOfPointToTheRightOf(double c1,double c2,
      size_t begin,size_t end) const
{
   if (end==0) end=N2-1;
   //search using binary search
   // if(begin>=end)cout<<"to x"<<begin<<" "<<end<<endl;;
   if(begin>=end)return GetIdxOfPointToTheRightOf(c1,end*N1,(end+1)*N1-1);
   size_t mid=((begin+end)/2);
   if(C2[mid*N1]>=c2) return GetIdxOfPointToTheRightOf(c1,c2,begin,mid);
   else return GetIdxOfPointToTheRightOf(c1,c2,mid+1,end);
}
//_____________________________________________________________________________
//
size_t Grid::GetIdxOfPointToTheRightOf(double c1, double c2,double c3,
      size_t begin,size_t end) const
{
   if (end==0) end=N3-1;
   //search using binary search
   if(begin>=end)return GetIdxOfPointToTheRightOf(c1,c2,begin,begin+N1*N2-1);
   size_t mid=((begin/(N1*N2)+end/(N1*N2))/2)*N1*N2;
   if(C3[mid]>=c3)return GetIdxOfPointToTheRightOf(c1,c2,c3,begin,mid);
   else return GetIdxOfPointToTheRightOf(c1,c2,c3,mid+1,end);
}
//_____________________________________________________________________________
//
TTree* Grid::GetTree(bool createNew)
{
   if (fTree) { if (createNew) delete fTree; else return fTree; }

   // define tree
   bool b,d; double vp,et,e1,e2,e3,c1,c2,c3;
   fTree = new TTree("t","field data");
   fTree->SetDirectory(0);
   fTree->Branch("v",&vp,"v/D");
   fTree->Branch("e",&et,"e/D");
   // 1D data
   fTree->Branch("e1",&e1,"e1/D");
   fTree->Branch("c1",&c1,"c1/D");
   if (N2!=0) { // 2D data
      fTree->Branch("e2",&e2,"e2/D");
      fTree->Branch("c2",&c2,"c2/D");
   }
   if (N3!=0) { // 3D data
      fTree->Branch("e3",&e3,"e3/D");
      fTree->Branch("c3",&c3,"c3/D");
   }
   fTree->Branch("b",&b,"b/O"); // boundary flag
   fTree->Branch("d",&d,"d/O"); // depletion flag

   // fill tree
   Info("GetTree","%zu entries",GetN());
   for (size_t i=0; i<GetN(); i++) {
      e1=E1[i]; c1=C1[i]; // 1D data
      if (N2!=0) { e2=E2[i]; c2=C2[i]; } // 2D data
      if (N3!=0) { e3=E3[i]; c3=C3[i]; } // 3D data
      vp=Vp[i]; et=Et[i]; b=fIsFixed[i]; d=fIsDepleted[i]; // common data
      fTree->Fill();
   }

   fTree->GetListOfBranches()->ls();
   fTree->ResetBranchAddresses(); // disconnect from local variables
   return fTree;
}
//_____________________________________________________________________________
//
double Grid::GetData(const std::vector<double> &data,
      double x, double y, double z) const
{
   if (z==0) { // <3D
      if (y==0) { // 1D
         //     |<---dC1m[idx]--->|
         //     +---r1---+---r2---+
         // C1[idx-1]    x      C1[idx]
         size_t idx=GetIdxOfPointToTheRightOf(x);
         double r2=(C1[idx]-x)/dC1m[idx];
         double r1=1-r2;
         double xval=data[idx]*r1+data[idx-1]*r2;
         if (gDebug>0) Info("GetData","data(x=%.4f)=%.2f, "
               "C1[%zu]=%.4f, C1[%zu]=%.4f, r1=%.2f, r2=%0.2f",
               x,xval,idx-1,C1[idx-1],idx,C1[idx],r1,r2);
         return xval;
      }
      // 2D
   }
   // 3D
   return 0;
}
