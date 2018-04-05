///////////////////////
//                   //
//XY                 //
//                   //
//2D Field based on X//
//it will create a 2D Field using cartesian coordinate and calculate the result
///////////////////////
#ifndef GeFiCa_XY_H
#define GeFiCa_XY_H

#include "X.h"
class TF2;

namespace GeFiCa { class XY; }

class GeFiCa::XY : public GeFiCa::X
{
   public:
      unsigned short n2; /**< number of steps along the 2nd axis*/
   public:
      /**
      * XY is a constructor with default values n1,n2 = 101
      */
      XY(unsigned short n1=101, unsigned short n2=101);
      
      virtual ~XY();

      virtual void SaveField(const char *fout=NULL);
      virtual void LoadField(const char *fin=NULL);
		/**
		* Returns the potential in two dimensions
		*/
      virtual double GetPotential(double x,double y){return GetData(x,y,kPotential);};
      /**
      * Returns the value for the electric field under the first direction
      */
      virtual double GetE1(double x,double y){return GetData(x,y,kE1);};
      /**
      * Returns the value for the electic field under the second direction
      */
      virtual double GetE2(double x,double y){return GetData(x,y,kE2);};
      /**
      * Returns the two dimensional impurity
      */
      virtual double GetImpurity(double x,double y){return GetData(x,y,kImpurity);};

   	/**
   	* Method involved in setting the impurity. 
   	* This is used for a variable impurity level that changes with x
   	*/
      virtual void SetImpurity(TF2 * Im);
		/**
		* This defines the class for the CINT library
		*/
      ClassDef(XY,1);

   protected:
      virtual void SOR2(int idx,bool elec);
      double *fE2; /**< Electric field under the second coordinate (x, r, or rho) direction */
      double *fC2; /**< The location under the second coordinate (x, r, or rho) direction*/
      double *fdC2p; ///< distance between this and next grid points alone C2
      double *fdC2m; ///< distance between this and previous grid points alone C2
      //left and right are for y axis
      /**
      * Uses a binary search to return the index in two dimensions
      */
      virtual int FindIdx(double tarx,double tary
            ,int ybegin,int yend);



		/**
      * Returns data for various variables. 
      */
      double GetData(double tarx,double tary,EOutput output); 
      virtual void SetStepLength(double steplength1,double steplength2); 
      virtual void Impuritystr2tf();
      virtual bool CalculateField(int idx);
};
#endif 

