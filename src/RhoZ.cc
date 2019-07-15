#include "RhoZ.h"
#include "Units.h"
#include "PointContact.h"
using namespace GeFiCa;

void RhoZ::GetBoundaryConditionFrom(Detector &detector)
{
   Grid::GetBoundaryConditionFrom(detector); // check number of calls

   TString type(detector.ClassName());
   if (type.Contains("PointContact")) {
      if (N1%2==1) { // we want no grid point right on z-axis
         Error("GetBoundaryConditionFrom",
               "Please assign even number of grid points along radius.");
         abort();
      }
      PointContact& pc = (PointContact&) detector;
      pc.CheckConfigurations();
      GetInfoFrom(pc);
   } else {
      Error("GetBoundaryConditionFrom", "%s is not expected.", type.Data());
      Error("GetBoundaryConditionFrom", "Please use "
            "PointContact or Segmented detector.");
      abort();
   }

   fDetector = &detector; // for GetC to use fDetector->Bias[]
}
//______________________________________________________________________________
//
void RhoZ::OverRelaxAt(size_t idx)
{
   /*wrong implement
   if (fIsFixed[idx]) return; // no need to calculate on boundaries

   double vzm; // previous(minus) Vp along z
   if (idx>=N1) vzm=Vp[idx-N1];
   else vzm=Vp[idx+N1]; // mirroring potential for bottom grid points

   double vnew=(Src[idx]+1/C1[idx]*(Vp[idx+1]-Vp[idx-1])/(dC1m[idx]+dC1p[idx]) 
         +(Vp[idx+1]/dC1p[idx]+Vp[idx-1]/dC1m[idx])*2/(dC1m[idx]+dC1p[idx])
         + (Vp[idx+N1]/dC2p[idx]+vzm/dC2m[idx])*2/(dC2p[idx]+dC2m[idx]))/
      ((1/dC1m[idx]+1/dC1p[idx])*2/(dC1m[idx]+dC1p[idx])
       +(1/dC2p[idx]+1/dC2m[idx])*2/(dC2p[idx]+dC2m[idx]));
   Printf("checl %.4e",vnew);
   vnew=RelaxationFactor*(vnew-Vp[idx])+Vp[idx]; // over relax

   double vmin=Vp[idx-1]; // minimal Vp around point[idx]
   if(vmin>Vp[idx+1]) vmin=Vp[idx+1];
   if (vmin>Vp[idx+N1]) vmin=Vp[idx+N1];
   if (vmin>vzm) vmin=vzm;

   double vmax=Vp[idx-1]; // maximal Vp around point[idx]
   if (vmax<Vp[idx+1]) vmax=Vp[idx+1];
   if (vmax<Vp[idx+N1]) vmax=Vp[idx+N1];
   if (vmax<vzm) vmax=vzm;

   if (vnew<vmin) {
      Vp[idx]=vmin; fIsDepleted[idx]=false;
   } else if (vnew>vmax) {
      Vp[idx]=vmax; fIsDepleted[idx]=false;
   } else {
      Vp[idx]=vnew; fIsDepleted[idx]=true;
   }
   // update Vp for impurity-only case even if the point is undepleted
   if (fDetector->Bias[0]==fDetector->Bias[1]) Vp[idx]=vnew;
   Printf("%.4e",vnew);
   */
    if (fIsFixed[idx])return;
   // 2nd-order Successive Over-Relaxation
   double drm=dC1m[idx]!=0?dC1m[idx]:dC1p[idx]; // dr_minus
   double drp=dC1p[idx]!=0?dC1p[idx]:dC1m[idx];
   double dzm=dC2m[idx]!=0?dC2m[idx]:dC2p[idx];
   double dzp=dC2p[idx]!=0?dC2p[idx]:dC2m[idx];
   double pzm,pzp,prm,prp; // pzm: potential_z_plus
   if(idx>=N1)pzm=Vp[idx-N1];
   else pzm=Vp[idx+N1];
   if(idx>=N1*N2-N1)pzp=Vp[idx];
   else pzp=Vp[idx+N1];
   if(idx%N1==0)prm=Vp[idx];
   else prm=Vp[idx-1];
   if(idx%N1==N1-1)prp=Vp[idx];
   else prp=Vp[idx+1];
   //if (idx<200)
   //Printf("idx=%zu, drm=%f, drp=%f, dzm=%f, dzp=%f, pzm=%f, pzp=%f, prm=%f, prp=%f\n",
   //      idx,drm, drp, dC2m[idx], dC2p[idx], pzm, pzp, prm, prp);
   double tmp=(Src[idx]
         + 1/C1[idx]*(prp-prm)/(drm+drp) +(prp/drp+prm/drm)*2/(drm+drp)
         + (pzp/dzp+pzm/dzm)*2/(dzp+dzm))/
      ((1/drm+1/drp)*2/(drm+drp)+(1/dzp+1/dzm)*2/(dzp+dzm));
   //find minmium and maxnium of all five grid, the new one should not go overthem.
   //find min
   double min=prm;
   double max=prm;
   if(min>prp)min=prp;
   if (min>pzp)min=pzp;
   if (min>pzm)min=pzm;

   //find max
   if(max<prp)max=prp;
   if (max<pzp)max=pzp;
   if (max<pzm)max=pzm;
//if tmp is greater or smaller than max and min, set tmp to it.

      //over relax
   //V[idx]=RelaxationFactor*(tmp-V[idx])+V[idx];
   //if need calculate depleted voltage
   double oldP=Vp[idx];
   tmp=RelaxationFactor*(tmp-oldP)+oldP;
   if(tmp<min) {
      Vp[idx]=min;
      fIsDepleted[idx]=false;
   } else if(tmp>max) {
      Vp[idx]=max;
      fIsDepleted[idx]=false;
   } else {
      Vp[idx]=tmp;
      fIsDepleted[idx]=true;
   }

   if(fDetector->Bias[0]==fDetector->Bias[1]) Vp[idx]=tmp;
}
//______________________________________________________________________________
//
double RhoZ::GetC()
{
   Grid::GetC(); // calculate field excluding undepleted region

   // calculate C based on CV^2/2 = epsilon int E^2 dx^3 / 2
   PointContact& pc = (PointContact&) *fDetector;
   double dV=pc.Bias[0]-pc.Bias[1]; if(dV<0)dV=-dV;
   double SumofElectricField=0;
   for(size_t i=0;i<GetN();i++) {
      double e1=E1[i];
      double e2=E2[i];
      double dr=dC1p[i];
      double dz=dC2p[i];
      SumofElectricField+=(e1*e1+e2*e2)*C1[i]*dr*dz;
   }
   double c=SumofElectricField*2*3.14159*epsilon/dV/dV;
   Info("GetC","%.2f pF",c/pF);
   return c;
}
//______________________________________________________________________________
//
void RhoZ::GetInfoFrom(PointContact& pc)
{
   // set positions of grid points
   for (size_t i=0; i<N1; i++) { // bottom line
      dC1p.push_back(2*pc.Radius/(N1-1)); dC1m.push_back(2*pc.Radius/(N1-1));
      dC2p.push_back(pc.Height/(N2-1));
      dC2m.push_back(pc.Height/(N2-1)); // there are mirrored points below
      C1.push_back(-pc.Radius+i*dC1p[i]); C2.push_back(0);
      E1.push_back(0); E2.push_back(0); Et.push_back(0); Vp.push_back(0);
      fIsFixed.push_back(false); fIsDepleted.push_back(false);
      Src.push_back(-pc.GetImpurity(C2[i])*Qe/epsilon);
   }
   for (size_t i=N1; i<N1*N2; i++) { // the rest
      dC1p.push_back(2*pc.Radius/(N1-1)); dC1m.push_back(2*pc.Radius/(N1-1));
      dC2p.push_back(pc.Height/(N2-1)); dC2m.push_back(pc.Height/(N2-1));
      C1.push_back(C1[i-N1]); C2.push_back(C2[i-N1]+dC2p[i-N1]);
      E1.push_back(0); E2.push_back(0); Et.push_back(0); Vp.push_back(0);
      fIsFixed.push_back(false); fIsDepleted.push_back(false);
      Src.push_back(-pc.GetImpurity(C2[i])*Qe/epsilon);
   }
   // set impurity in groove and potentials of grid points
   size_t npc=0; // number of grid points in PC
   for (size_t i=N1*N2; i-->0;) {
      if (C1[i]>=-pc.PointContactR && C1[i]<=pc.PointContactR
            && C2[i]<=pc.PointContactH) { // point contact
         Vp[i]=pc.Bias[0]; fIsFixed[i]=true; npc++;
      } else if (C1[i]<=pc.BoreR && C1[i]>=-pc.BoreR
            && C2[i]>=pc.Height-pc.BoreH) { // bore hole
         Vp[i]=pc.Bias[1]; fIsFixed[i]=true;
      } else if (((C1[i]>pc.WrapAroundR-pc.GrooveW && C1[i]<pc.WrapAroundR)
               || (C1[i]>-pc.WrapAroundR && C1[i]<-pc.WrapAroundR+pc.GrooveW))
            && C2[i]<pc.GrooveH) { // groove
         Vp[i]=pc.Bias[1]/4; Src[i]=0;
      } else // bulk
         Vp[i]=(pc.Bias[0]+pc.Bias[1])/2;
      double slope, intercept;
      if (pc.CornerW>0) { // has top taper
         slope=-pc.CornerH/pc.CornerW;
         intercept=pc.Height-slope*(pc.Radius-pc.CornerW);
         if (C2[i]>-slope*C1[i]+intercept||C2[i]>slope*C1[i]+intercept) {
            Vp[i]=pc.Bias[1]; fIsFixed[i]=true;
         }
      }
      if (pc.TaperW>0) { // has bottom taper
         slope=pc.TaperH/(pc.TaperW);
         intercept=-(pc.Radius-pc.TaperW)*slope;
         if (C2[i]<=C1[i]*slope+intercept || C2[i]<=-C1[i]*slope+intercept) {
            Vp[i]=pc.Bias[1]; fIsFixed[i]=true;
         }
      }
      if (pc.BoreTaperW>0) { // has bore taper
         slope=pc.BoreTaperH/pc.BoreTaperW;
         intercept=pc.Height-slope*(pc.BoreR+pc.BoreTaperW);
         if ((C2[i]>-slope*C1[i]+intercept && C1[i]<=-pc.BoreR) ||
               (C2[i]>slope*C1[i]+intercept && C1[i]>=pc.BoreR)) {
            Vp[i]=pc.Bias[1]; fIsFixed[i]=true;
         }
      }
   }
   if (npc<1) { Error("GetInfoFrom", "no point in point contact!"); abort(); }
   for (size_t i=N1*N2-1; i>=N1*N2-N1; i--) { // top boundary
      Vp[i]=pc.Bias[1]; fIsFixed[i]=true; dC2p[i]=0;
   }
   for (size_t i=0; i<=N1*N2-N1; i=i+N1) { // left & right boundaries
      Vp[i]=pc.Bias[1]; Vp[i+N1-1]=pc.Bias[1];
      fIsFixed[i]=true; fIsFixed[i+N1-1]=true;
      dC1m[i]=0; dC1p[i+N1-1]=0;
   }
   for (size_t i=0; i<N1; i++) { // bottom boundary
      dC2m[i]=0;
      if (C1[i]>=pc.WrapAroundR||C1[i]<=-pc.WrapAroundR) {// wrap arround
         fIsFixed[i]=true;
         Vp[i]=pc.Bias[1];
      }
   }

   ReallocateGridPointsNearBoundaries(pc);
}
//______________________________________________________________________________
//
void RhoZ::ReallocateGridPointsNearBoundaries(PointContact &pc)
{
   double slope, intercept;
   for (size_t i=0; i<GetN(); i++) {
      if (C2[i]-pc.PointContactH<dC2m[i] && C2[i]>pc.PointContactH
            && C1[i]<(pc.PointContactR+1e-4*mm) && C1[i]>-(pc.PointContactR+1e-4*mm)) {
         dC2m[i]=C2[i]-pc.PointContactH; // top of point contact
         // since C2[i] is too close to boundary, we regard it as on the boundary
         // Fixme: same protection should be applied to other boundaries
         if (dC2m[i]<1e-4*mm) { Vp[i]=pc.Bias[0]; fIsFixed[i]=true; }
      }
      if (C1[i]-pc.PointContactR<dC1m[i]&&C1[i]>pc.PointContactR
            &&C2[i]<pc.PointContactH+1e-4*mm) {
         dC1m[i]=C1[i]-pc.PointContactR; // right of point contact
         if (dC1m[i]<1e-4*mm) { Vp[i]=pc.Bias[0]; fIsFixed[i]=true; }
      }
      if (-C1[i]-pc.PointContactR<dC1p[i]&&C1[i]<-pc.PointContactR
            &&C2[i]<pc.PointContactH+1e-4*mm) {
         dC1p[i]=-C1[i]-pc.PointContactR; // left of point contact
         if (dC1p[i]<1e-4*mm) { Vp[i]=pc.Bias[0]; fIsFixed[i]=true; }
      }
      if (C1[i]-pc.BoreR>0&&C1[i]-pc.BoreR<dC1m[i]
            &&C2[i]>pc.Height-pc.BoreH-1e-4*mm){ //right side of bore
         dC1m[i]=C1[i]-pc.BoreR;
         if (dC1m[i]<1e-4*mm) { Vp[i]=pc.Bias[0]; fIsFixed[i]=true; }
      }
      if (-C1[i]-pc.BoreR>0&&-C1[i]-pc.BoreR<dC1p[i]
            &&C2[i]>=pc.Height-pc.BoreH-1e-4*mm){ //left side of bore
         dC1p[i]=-C1[i]-pc.BoreR;
         if (dC1p[i]<1e-4*mm) { Vp[i]=pc.Bias[0]; fIsFixed[i]=true; }
      }
      //down side of bore
      if (pc.Height-pc.BoreH-1e-4*mm-C2[i]>0 && pc.Height-pc.BoreH-C2[i]<dC2p[i]
            && C1[i]>-pc.BoreR-1e-4*mm && C1[i]<pc.BoreR+1e-4*mm) {
         dC2p[i]=pc.Height-pc.BoreH-C2[i];
         if (dC2p[i]<1e-4*mm) { Vp[i]=pc.Bias[1]; fIsFixed[i]=true; }
      }
      // Fixme: V around groove bounaries are all changable,
      // which should be reallocated?
      if (pc.WrapAroundR-C1[i]<dC1p[i]&&C1[i]<pc.WrapAroundR&&i<N1)
         dC1p[i]=pc.WrapAroundR-C1[i];
      if (pc.WrapAroundR+C1[i]<dC1p[i]&&C1[i]>-pc.WrapAroundR&&i<N1)
         dC1m[i]=pc.WrapAroundR+C1[i];
      if (pc.CornerW>0) { // has top taper
         slope=-pc.CornerH/pc.CornerW;
         intercept=pc.Height-slope*(pc.Radius-pc.CornerW);
         if (C1[i]+C2[i]/slope-intercept/slope>0 &&
               C1[i]+C2[i]/slope-intercept/slope<dC1m[i] &&
               C2[i]>pc.Height-pc.CornerH) // left
            dC1m[i]=C1[i]+C2[i]/slope-intercept/slope;
         if (-C1[i]+C2[i]/slope-intercept/slope>0 &&
               -C1[i]+C2[i]/slope-intercept/slope<dC1p[i] &&
               C2[i]>pc.Height-pc.CornerH) // right
            dC1p[i]=-C1[i]+C2[i]/slope-intercept/slope;
         if((C1[i]*slope+intercept)-C2[i]<dC2p[i] &&
               C1[i]>pc.Radius-pc.CornerW&&(C1[i]*slope+intercept)-C2[i]>0)
            dC2p[i]=(C1[i]*slope+intercept)-C2[i];
         if((-C1[i]*slope+intercept)-C2[i]<dC2p[i] &&
               C1[i]<-pc.Radius+pc.CornerW&&(-C1[i]*slope+intercept)-C2[i]>0)
            dC2p[i]=(-C1[i]*slope+intercept)-C2[i];
      }
      if (pc.TaperW>0) { // has bottom taper
         slope=pc.TaperH/pc.TaperW;
         intercept=-(pc.Radius-pc.TaperW)*slope;
         if (C2[i]-(slope*C1[i]+intercept)<dC2m[i]
               && C2[i]-(slope*C1[i]+intercept)>0 && i>=N1-1) // right side, C2
            dC2m[i]=C2[i]-(slope*C1[i]+intercept );
         if ((C2[i]-intercept)/slope-C1[i]<dC1p[i]
               && (C2[i]-intercept)/slope-C1[i]>0) // right side, C1
            dC1p[i]=(C2[i]-intercept)/slope-C1[i];
         if (C2[i]-(-slope*C1[i]+intercept)<dC2m[i]
               && C2[i]-(-slope*C1[i]+intercept)>0 && i>=N1-1) // left side, C2
            dC2m[i]=C2[i]-(-slope*C1[i]+intercept);
         if (C1[i]+(C2[i]-intercept)/slope<dC1m[i]
               && C1[i]+(C2[i]-intercept)/slope>0) // left side, C1
            dC1m[i]=C1[i]+(C2[i]-intercept)/slope;
      }
      if (pc.BoreTaperW>0) { // has bore taper
         slope=pc.BoreTaperH/pc.BoreTaperW;
         intercept=pc.Height-slope*(pc.BoreR+pc.BoreTaperW);
         if (C1[i]-C2[i]/slope+intercept/slope<dC1m[i] &&
               C2[i]>pc.Height-pc.BoreTaperH &&
               C1[i]-C2[i]/slope+intercept/slope>0) //right side of hole taper
            dC1m[i]=C1[i]-C2[i]/slope+intercept/slope;
         if (-C1[i]-C2[i]/slope+intercept/slope>0 &&
               -C1[i]-C2[i]/slope+intercept/slope<dC1p[i] &&
               C2[i]>pc.Height-pc.BoreTaperH) //left side of hole taper
            dC1p[i]=-C1[i]-C2[i]/slope+intercept/slope;
         if((C1[i]*slope+intercept)-C2[i]<dC2p[i] &&
               C1[i]<pc.BoreR+pc.BoreTaperW && C1[i]>pc.BoreR
               && (C1[i]*slope+intercept)-C2[i]>0) // right side of hole taper
            dC2p[i]=(C1[i]*slope+intercept)-C2[i];
         if((-C1[i]*slope+intercept)-C2[i]<dC2p[i] &&
               C1[i]>-pc.BoreR-pc.BoreTaperW && C1[i]<-pc.BoreR
               && (-C1[i]*slope+intercept)-C2[i]>0) // left side of hole taper
            dC2p[i]=(-C1[i]*slope+intercept)-C2[i];
      }
   }
}
//______________________________________________________________________________
//
void RhoZ::CalculateE()
{
   Grid::CalculateE(); // deal with E1
   for (size_t i=0; i<GetN(); i++) { // deal with E2
      E2[i]=-(Vp[i+N1]-Vp[i-N1])/(dC2p[i]+dC2m[i]);
      if (i<N1) E2[i]=-(Vp[i+N1]-Vp[i])/dC2p[i]; // lower boundary
      if (i>GetN()-N1) E2[i]=-(Vp[i]-Vp[i-N1])/dC2m[i]; // upper boundary
      if (i%N1==0) E1[i]=-(Vp[i+1]-Vp[i])/dC1p[i]; // left boundary
      if ((i+1)%N1==0) E1[i]=-(Vp[i]-Vp[i-1])/dC1m[i]; // right boundary
      Et[i]=sqrt(E1[i]*E1[i]+E2[i]*E2[i]);
   }
}
