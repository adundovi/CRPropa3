#ifndef CRPROPA_GRIDTURBULENCEDOUBLE_H
#define CRPROPA_GRIDTURBULENCEDOUBLE_H

#ifdef CRPROPA_HAVE_FFTW3

#include "crpropa/Grid.h"
#include "crpropa/magneticField/turbulentField/TurbulentField.h"

#include "fftw3.h"

namespace crpropa {
/**
 * \addtogroup MagneticFields
 * @{
 */

/**
 @class GridTurbulenceDouble
 @brief Turbulent grid-based magnetic field with a general energy spectrum
 */
class GridTurbulenceDouble : public TurbulentField {
  protected:
	unsigned int seed;
	ref_ptr<Grid3d> gridPtr;

	void initGrid(const GridProperties &grid);
	void initTurbulence();

  public:
	/**
	 Create a random initialization of a turbulent field.
	 @param spectrum    TurbulenceSpectrum instance to define the spectrum of
	 turbulence
	 @param gridProp	GridProperties instance to define the underlying grid
	 @param seed	 Random seed
	 */
	GridTurbulenceDouble(const TurbulenceSpectrum &spectum,
	               const GridProperties &gridProp, unsigned int seed = 0);

	Vector3d getField(const Vector3d &pos) const;

	/** Return a const reference to the grid */
	const ref_ptr<Grid3d> &getGrid() const;

	/* Helper functions for synthetic turbulent field models */
	// Check the grid properties before the FFT procedure
	static void checkGridRequirements(ref_ptr<Grid3d> grid, double lMin,
	                                  double lMax);
	// Execute inverse discrete FFT in-place for a 3D grid, from complex to real
	// space
	static void executeInverseFFTInplace(ref_ptr<Grid3d> grid,
	                                     fftw_complex *Bkx, fftw_complex *Bky,
	                                     fftw_complex *Bkz);

	// Usefull checks for a grid field
	/** Evaluate the mean vector of all grid points */
	Vector3d getMeanFieldVector() const;
	/** Evaluate the mean of all grid points */
	double getMeanFieldStrength() const;
	/** Evaluate the RMS of all grid points */
	double getRmsFieldStrength() const;
	/** Evaluate the RMS of all grid points per axis */
	std::array<double, 3> getRmsFieldStrengthPerAxis() const;
};

/** @}*/
} // namespace crpropa

#endif // CRPROPA_HAVE_FFTW3

#endif // CRPROPA_GRIDTURBULENCEDOUBLE_H
