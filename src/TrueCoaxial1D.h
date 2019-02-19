#ifndef GeFiCa_TRUECOAXIAL1D_H
#define GeFiCa_TRUECOAXIAL1D_H

#include "Rho.h"

namespace GeFiCa { class TrueCoaxial1D; }

/**
 * Grid setup for 1D true coaxial detectors.
 */
class GeFiCa::TrueCoaxial1D : public GeFiCa::Rho
{
   public :
      double OuterRadius; ///< Outer radius of the detector
      double InnerRadius; ///< Inner radius of the detector

      /**
       * Default constructor.
       */
      TrueCoaxial1D(int n=101, const char *name="tc1",
            const char *title="1D true coaxial detector");

      virtual void Initialize();

      ClassDef(TrueCoaxial1D, 1);

   protected:
      /**
       * Analytic calculation of 1D field in cylindrical coordinates with fixed
       * impurity concentration.
       *
       * In case of fixed impurity, according to
       * https://www.wolframalpha.com/input/?i=1%2Fx*+%28x*f%28x%29%27%29%27%3Da
       * potential(r)=a + b log(r) - rho/4/epsilon*r^2 with boundary conditions:
       *
       * - potential(rinner) = V0,
       * - potential(router) = V1,
       *
       * So, 
       *
       * - a = - rho/2/epsilon
       * - b = [rho/4/epsilon*(router^2-rinner^2)+(V1-V0)]/log(router/rinner)
       */
      bool Analytic();
};
#endif

