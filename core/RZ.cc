#include <TF2.h>
#include <TFile.h>
#include <TChain.h>
#include <TVectorD.h>

#include "RZ.h"
#include "Units.h"

using namespace GeFiCa;

//_____________________________________________________________________________
//
#include <iostream>
using namespace std;
void RZ::SOR2(int idx,bool elec)
{
   if (fIsFixed[idx])return; 
   // 2nd-order Successive Over-Relaxation
   double density=fImpurity[idx]*Qe;
   double drm=fdC1m[idx]; // dr_minus
   double drp=fdC1p[idx];
   double dzm=fdC2m[idx];
   double dzp=fdC2p[idx];
   double pzm,pzp,prm,prp; // pzm: potential_z_plus
   if(idx>=n1)pzm=fPotential[idx-n1];
   else pzm=fPotential[idx];
   if(idx>=n-n1)pzp=fPotential[idx];
   else pzp=fPotential[idx+n1];
   if(idx%n1==0)prm=fPotential[idx];
   else prm=fPotential[idx-1];
   if(idx%n1==n1-1)prp=fPotential[idx];
   else prp=fPotential[idx+1];
   double tmp=(density/epsilon
         + 1/fC1[idx]*(prp-prm)/(drm+drp) +(prp/drp+prm/drm)*2/(drm+drp)
         + (pzp/dzp+pzm/dzm)*2/(dzp+dzm))/
      ((1/drm+1/drp)*2/(drm+drp)+(1/dzp+1/dzm)*2/(dzp+dzm));
   fPotential[idx]=Csor*(tmp-fPotential[idx])+fPotential[idx];
}

/*
void PointContactRZ::SOR2(int idx,bool elec)
{
   RZ::SOR2(idx,elec);
   // update electric fields on point contact boundary
   if (elec) {
      if (fC2[idx]>PointDepth-fdC2m[idx]
            && fC2[idx]<PointDepth+fdC2p[idx]) // PC top border
         fE2[idx]=(fPotential[idx]-fPotential[idx+n1])/fdC2p[idx];
      if (fC1[idx]>-PointR-fdC1m[idx]
            && fC1[idx]<-PointR+fdC1p[idx]) // PC left border
         fE1[idx]=(fPotential[idx]-fPotential[idx-1])/fdC1m[idx];
      if (fC1[idx]>PointR-fdC1m[idx]
            && fC1[idx]<PointR+fdC1p[idx]) // PC right border
         fE1[idx]=(fPotential[idx]-fPotential[idx+1])/fdC1p[idx];
   }
}
*/
