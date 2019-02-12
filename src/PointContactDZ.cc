#include <stdlib.h>

#include "PointContactDZ.h"
#include "iostream"
#include "Units.h"
using namespace GeFiCa;
using namespace std;
//for the case which point contact may not fall right on the grid, it will case
//a great different potential on space close to point contact the current
//design does not include when point contact is close to boundary of the whole
//detector.
void PointContactDZ::BoundaryOnPointcontact()
{
   int index=FindIdx(PointContactR,PointContactZ,0,n2-1);
   //cout<<index<<" "<<fC1[index]<<" "<<fC2[index]<<endl;
   int idxZ=(index/n1-1)*n1;
   int idxPos=index%n1-1;
   //cout<<idxPos<<" "<<fC1[idxPos]<<" "<<fC2[idxPos]<<endl;
   //cout<<idxZ<<" "<<fC1[idxZ]<<" "<<fC2[idxZ]<<endl;

   for (int i=0;i<n1;i++) {
      if (PointContactZ==0) break;
      fC2[idxZ+i]=PointContactZ;//set z for the line where z is closest to pc's depth
      //cout<<idxZ+i<<" "<< fC2[idxZ+i]<<"| ";

      //set steps from depthline
      fdC2m[idxZ+i]=fC2[idxZ+i]-fC2[idxZ+i-n1];
      fdC2p[idxZ+i]=fC2[idxZ+i+n1]-fC2[idxZ+i];

      //set steps for two line aside depth of pc
      fdC2m[idxZ+i+n1]=fdC2p[idxZ+i];
      fdC2p[idxZ+i-n1]=fdC2m[idxZ+i];
   }

   for(int i=0;i<n2;i++) {
      fC1[idxPos+i*n1]=PointContactR;//set r for the line where r is closest to pc's R 

      //set steps for Radius line
      fdC1m[idxPos+i*n1]=fC1[idxPos+i*n1]-fC1[idxPos+i*n1-1];
      fdC1p[idxPos+i*n1]=fC1[idxPos+i*n1+1]-fC1[idxPos+i*n1];

      //set steps for two line aside the previous line
      fdC1m[idxPos+i*n1+1]=fdC1p[idxPos+i*n1];
      fdC1p[idxPos+i*n1-1]=fdC1m[idxPos+i*n1];
   }
   int idxNeg=n1-idxPos-1;
   //cout<<idxNeg<<" "<<fC1[idxNeg]<<" "<<fC2[idxNeg]<<endl;
   for(int i=0;i<n2;i++) {
      fC1[idxNeg+i*n1]=-PointContactR;//set r for the line where r is closest to pc's R 

      //set steps for Radius line
      fdC1m[idxNeg+i*n1]=fC1[idxNeg+i*n1]-fC1[idxNeg+i*n1-1];
      fdC1p[idxNeg+i*n1]=fC1[idxNeg+i*n1+1]-fC1[idxNeg+i*n1];

      //set steps for two line aside the previous line
      fdC1m[idxNeg+i*n1+1]=fdC1p[idxNeg+i*n1];
      fdC1p[idxNeg+i*n1-1]=fdC1m[idxNeg+i*n1];
   }
}
//_____________________________________________________________________________
//
void PointContactDZ::BoundaryonWarpAround()
{
   int index=FindIdx(WrapArroundR,0,0,n2-1);
   if (index>n1)index-=n1;
   for(int i=index;i<n;i+=n1)
   {
      fC1[i]=WrapArroundR;
      fdC1m[i]=fC1[i]-fC1[i-1];
      fdC1p[i]=fC1[i+1]-fC1[i];
      fdC1m[i+1]=fdC1p[i];
      fdC1p[i-1]=fdC1m[i];
   }

   index=FindIdx(-WrapArroundR,0,0,n2-1)-1;
   if (index>n1)index-=n1;

   for(int i=index;i<n;i+=n1)
   {
      fC1[i]=-WrapArroundR;
      fdC1m[i]=fC1[i]-fC1[i-1];
      fdC1p[i]=fC1[i+1]-fC1[i];
      fdC1m[i+1]=fdC1p[i];
      fdC1p[i-1]=fdC1m[i];

   }
}
//_____________________________________________________________________________
//
void PointContactDZ::Initialize()
{
   if (n1%2==1) {
      Error("Initialize", "Number of grids in R cannot be odd, abort!");
      abort();
   }

   // The step length is calculated with the following equation:
   // BEGIN_HTML
   // <pre>
   //      double stepLength=(UpperBound-LowerBound)/(n-1);
   // </pre>
   // END_HTML
   // If the inner radius is not larger than the outer radius,
   // no grid will be created
   if (Z0>=Z) {
      Warning("Initialize",
            "Lower bound (%f) >= upper bound (%f)! No grid is created!",
            Z0, Z);
      return;
   }
   double RUpperBound,RLowerBound,PointBegin,PointEnd;
   RUpperBound=Radius;
   RLowerBound=-Radius;
   PointBegin=-PointContactR;
   PointEnd=PointContactR;
   double steplength1=(RUpperBound-RLowerBound)/(n1-1);
   double steplength2=(Z-Z0)/(n2-1);
   SetStepLength(steplength1,steplength2);
   for(int i=n;i-->0;) 
   {
      fC1[i]=fC1[i]+RLowerBound;
   } 
   BoundaryOnPointcontact();
   BoundaryonWarpAround();

   // set initial potential values
   for(int i=n;i-->0;) {
      fV[i]=(V0+V1)/2;//common this line for finding depleat voltage
      // set potential for inner electrodes
      if(fC1[i]>=PointBegin&&fC1[i]<=PointEnd&&fC2[i]<=PointContactZ) {
         fV[i]=V1;
         fIsFixed[i]=true;
      }
   }
   // set potential for outer electrodes
   for(int i=n-1;i>=n-n1;i--) {
      fIsFixed[i]=true;
      fV[i]=V0;
   }
   for(int i=0;i<n-n1;i=i+n1) {
      fIsFixed[i]=true;
      fIsFixed[i+n1-1]=true;
      fV[i]=V0;
      fV[i+n1-1]=V0;
   }
   for (int i=0;i<n1;i++)
   {
      if(fC1[i]>=WrapArroundR||fC1[i]<=-WrapArroundR)
      {
         fIsFixed[i]=true;
         fV[i]=V0;
      }
   }
   double k=TaperZ/(TaperLength);
   double b=-(Radius-TaperLength)*k;

   for(int i=0;i<n;i++)
   {
      if(fC2[i]>=fC1[i]*k-TaperLength)
      {
         fIsFixed[i]=true;
         fV[i]=V0;
      }
      if(fC2[i]<=-fC1[i]*k+b)
      {
         fIsFixed[i]=true;
         fV[i]=V0;
      }
      if(fC2[i]-(fC1[i]*k+b)<fdC2m[i])
      {
         fdC2m[i]=fC2[i]-(k*fC1[i]+b);
         fdC1p[i]=fC2[i]-b-k*fC1[i];
      }
      if(fC2[i]-(-k*fC1[i]+b)<fdC2m[i])
      {
         fdC2m[i]=fC2[i]-(-fC1[i]*k+b);
         fdC1m[i]=-fC2[i]*k+b-fC1[i];
      }
   }
}
//_____________________________________________________________________________
//
bool PointContactDZ::CalculatePotential(EMethod method)
{
   if (!fIsLoaded) Initialize();
   // this commentd block are slow depletion voltage finder
   while(0)
   {
      RZ::CalculatePotential(method);
      if(!X::IsDepleted())
      {
         int maxn=GetIdxOfMaxV();
         int minn=GetIdxOfMinV();
         if(V0>V1)
         {
            V0=(fV[maxn]-V0)*1.01+V0;
            V1=-(V1-fV[minn])*1.01+V1;
         }
         else
         {
            V1=(fV[maxn]-V1)*1.01+V1;
            V0=-(V0-fV[minn])*1.01+V0;
         }
         Initialize();
         RZ::CalculatePotential(method);
         cout<<V0<<" "<<V1<<endl;
      }
      else break;
   }
   return RZ::CalculatePotential(method);
}
//_____________________________________________________________________________
//
bool PointContactDZ::CalculateField(int idx)
{
   if (!XY::CalculateField(idx)) return false;

   if (fC2[idx]>PointContactZ-fdC2m[idx]
         && fC2[idx]<PointContactZ+fdC2p[idx]) // PC top boundary
      fE2[idx]=(fV[idx]-fV[idx+n1])/fdC2p[idx];
   if (fC1[idx]>-PointContactR-fdC1m[idx]
         && fC1[idx]<-PointContactR+fdC1p[idx]) // PC left boundary
      fE1[idx]=(fV[idx]-fV[idx-1])/fdC1m[idx];
   if (fC1[idx]>PointContactR-fdC1m[idx]
         && fC1[idx]<PointContactR+fdC1p[idx]) // PC right boundary
      fE1[idx]=(fV[idx]-fV[idx+1])/fdC1p[idx];

   return true;
}
//_____________________________________________________________________________
//
#include <fstream>
bool PointContactDZ::SaveFieldAsFieldgen(const char * fout)
{
   ofstream outfile(fout);

   outfile<<"# height "<< Z-Z0;        
   outfile<<"\n# xtal_radius "<<Radius;
   outfile<<"\n# pc_length   "<<PointContactZ;        
   outfile<<"\n# pc_radius   "<<PointContactR;         
   outfile<<"\n# wrap_around_radius "<<WrapArroundR; 
   outfile<<"\n# grid size on r "<<fdC1p[0];
   outfile<<"\n# grid size on z "<<fdC2p[0];
   outfile<<"\n# impurity_z0  "<<fImpurity[0];
   outfile<<"\n# xtal_HV      "<<V1;
   outfile<<"\n# max_iterations "<<MaxIterations;
   outfile<<"\n# ";
   outfile<<"\n## r (mm), z (mm), V (V),  E (V/cm), E_r (V/cm), E_z (V/cm)";
   for (int i=0;i<n;i++) {
      double E=sqrt(fE1[i]*fE1[i]+fE2[i]*fE2[i]);
      outfile<<"\n"<<fC1[i]<<"  "<<fC2[i]<<"  "<<fV[i]<<"  "<<E<<"  "<<fE1[i]<<"  "<<fE2[i];
   }
   outfile.close();
   return true;
}
