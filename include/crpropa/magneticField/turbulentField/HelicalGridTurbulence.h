#ifndef CRPROPA_HELICALGRIDTURBULENCE_H
#define CRPROPA_HELICALGRIDTURBULENCE_H

#ifdef CRPROPA_HAVE_FFTW3F

#include "crpropa/Grid.h"
#include "crpropa/magneticField/MagneticFieldGrid.h"
#include "crpropa/magneticField/turbulentField/GridTurbulence.h"

#include "kiss/logger.h"
#include "kiss/string.h"

/**
 @file
 @brief Generate a turbulent field on a grid and related functions.

 This file contains a number of functions related to scalar and vector grids
 (Grid.h).
 */

namespace crpropa {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class HelicalGridTurbulence
 @brief Turbulent grid-based magnetic field with a simple power-law spectrum
 */
class HelicalGridTurbulence : public GridTurbulence {
private:
  double H; 
public:
  HelicalGridTurbulence(const TurbulenceSpectrum &spectrum, const GridProperties &gridProp,
                        double H, unsigned int seed = 0);

  double getCorrelationLength() const;
  static double turbulentCorrelationLength(double lMin, double lMax,
                                           double sindex);

  static void initTurbulence(ref_ptr<Grid3f> grid, double Brms, double lMin,
                             double lMax, double alpha, int seed, double H);
};

// Compatibility with old functions from GridTurbulence:

/**
 Same as the simple turbulent field but with helicity.
 @param H	Helicity
*/
void initHelicalTurbulence(ref_ptr<Grid3f> grid, double Brms, double lMin,
                           double lMax, double alpha = -11 / 3., int seed = 0,
                           double H = 0) {
  KISS_LOG_WARNING
      << "initTurbulence is deprecated and will be removed in the future. "
         "Replace it with an appropriate turbulent field model instance.";
  HelicalGridTurbulence::initTurbulence(grid, Brms, lMin, lMax, alpha, seed, H);
}

/** @}*/
} // namespace crpropa

#endif // CRPROPA_HAVE_FFTW3F

#endif // CRPROPA_HELICALGRIDTURBULENCE_H