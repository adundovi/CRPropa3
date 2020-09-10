#include "crpropa/magneticField/turbulentField/GridTurbulenceDouble.h"
#include "crpropa/GridTools.h"
#include "crpropa/Random.h"

#ifdef CRPROPA_HAVE_FFTW3

namespace crpropa {


GridTurbulenceDouble::GridTurbulenceDouble(const TurbulenceSpectrum &spectrum,
                               const GridProperties &gridProp,
                               unsigned int seed)
    : TurbulentField(spectrum), seed(seed) {
	initGrid(gridProp);
	checkGridRequirements(gridPtr, spectrum.getLmin(), spectrum.getLmax());
	initTurbulence();
}

void GridTurbulenceDouble::initGrid(const GridProperties &p) {
	gridPtr = new Grid3d(p);
}

Vector3d GridTurbulenceDouble::getField(const Vector3d &pos) const {
	return gridPtr->interpolate(pos);
}

const ref_ptr<Grid3d> &GridTurbulenceDouble::getGrid() const { return gridPtr; }

void GridTurbulenceDouble::initTurbulence() {

	Vector3d spacing = gridPtr->getSpacing();
	size_t n = gridPtr->getNx(); // size of array
	size_t n2 = (size_t)floor(n / 2) +
	            1; // size array in z-direction in configuration space

	// arrays to hold the complex vector components of the B(k)-field
	fftw_complex *Bkx, *Bky, *Bkz;
	Bkx = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n * n * n2);
	Bky = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n * n * n2);
	Bkz = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n * n * n2);

	Random random;
	if (seed != 0)
		random.seed(seed); // use given seed

	// calculate the n possible discrete wave numbers
	double K[n];
	for (int i = 0; i < n; i++)
		K[i] = ((double)i / n - i / (n / 2));

	// construct the field in configuration space
	int i;
	double k;

	// parameters goes for non-helical calculations
	double theta;

	// double kMin = 2*M_PI / lMax; // * 2 * spacing.x; // spacing.x / lMax;
	// double kMax = 2*M_PI / lMin; // * 2 * spacing.x; // spacing.x / lMin;
	double kMin = spacing.x / spectrum.getLmax();
	double kMax = spacing.x / spectrum.getLmin();
	auto lambda = spectrum.getLbendover() / spacing.x * 2 * M_PI;

	Vector3d b;           // real b-field vector
	Vector3d ek, e1, e2;  // orthogonal base
	Vector3d n0(1, 1, 1); // arbitrary vector to construct orthogonal base

	for (size_t ix = 0; ix < n; ix++) {
		for (size_t iy = 0; iy < n; iy++) {
			for (size_t iz = 0; iz < n2; iz++) {

				i = ix * n * n2 + iy * n2 + iz;
				ek.setXYZ(K[ix], K[iy], K[iz]);
				k = ek.getR();

				// wave outside of turbulent range -> B(k) = 0
				if ((k < kMin) || (k > kMax)) {
					Bkx[i][0] = 0;
					Bkx[i][1] = 0;
					Bky[i][0] = 0;
					Bky[i][1] = 0;
					Bkz[i][0] = 0;
					Bkz[i][1] = 0;
					continue;
				}

				// construct an orthogonal base ek, e1, e2
				if (ek.isParallelTo(n0, double(1e-3))) {
					// ek parallel to (1,1,1)
					e1.setXYZ(-1., 1., 0);
					e2.setXYZ(1., 1., -2.);
				} else {
					// ek not parallel to (1,1,1)
					e1 = n0.cross(ek);
					e2 = ek.cross(e1);
				}
				e1 /= e1.getR();
				e2 /= e2.getR();

				// random orientation perpendicular to k
				theta = 2 * M_PI * random.rand();
				b = e1 * std::cos(theta) + e2 * std::sin(theta);

				// normal distributed amplitude with mean = 0
				b *= std::sqrt(spectrum.energySpectrum(k*lambda));
				
				// uniform random phase
				double phase = 2 * M_PI * random.rand();
				double cosPhase = std::cos(phase); // real part
				double sinPhase = std::sin(phase); // imaginary part

				Bkx[i][0] = b.x * cosPhase;
				Bkx[i][1] = b.x * sinPhase;
				Bky[i][0] = b.y * cosPhase;
				Bky[i][1] = b.y * sinPhase;
				Bkz[i][0] = b.z * cosPhase;
				Bkz[i][1] = b.z * sinPhase;
			} // for iz
		}     // for iy
	}         // for ix

	executeInverseFFTInplace(gridPtr, Bkx, Bky, Bkz);

	fftw_free(Bkx);
	fftw_free(Bky);
	fftw_free(Bkz);

	scaleGrid(gridPtr, spectrum.getBrms() /
	                       rmsFieldStrength(gridPtr)); // normalize to Brms
}

// Check the grid properties before the FFT procedure
void GridTurbulenceDouble::checkGridRequirements(ref_ptr<Grid3d> grid, double lMin,
                                           double lMax) {
	size_t Nx = grid->getNx();
	size_t Ny = grid->getNy();
	size_t Nz = grid->getNz();
	Vector3d spacing = grid->getSpacing();

	if ((Nx != Ny) or (Ny != Nz))
		throw std::runtime_error("turbulentField: only cubic grid supported");
	if ((spacing.x != spacing.y) or (spacing.y != spacing.z))
		throw std::runtime_error("turbulentField: only equal spacing suported");
	if (lMin < 2 * spacing.x)
		throw std::runtime_error("turbulentField: lMin < 2 * spacing");
	if (lMax > Nx * spacing.x) // before was (lMax > Nx * spacing.x / 2), why?
		throw std::runtime_error("turbulentField: lMax > size");
}

// Execute inverse discrete FFT in-place for a 3D grid, from complex to real
// space
void GridTurbulenceDouble::executeInverseFFTInplace(ref_ptr<Grid3d> grid,
                                              fftw_complex *Bkx,
                                              fftw_complex *Bky,
                                              fftw_complex *Bkz) {

	size_t n = grid->getNx(); // size of array
	size_t n2 = (size_t)floor(n / 2) +
	            1; // size array in z-direction in configuration space

	// in-place, complex to real, inverse Fourier transformation on each
	// component note that the last elements of B(x) are unused now
	double *Bx = (double *)Bkx;
	fftw_plan plan_x = fftw_plan_dft_c2r_3d(n, n, n, Bkx, Bx, FFTW_ESTIMATE);
	fftw_execute(plan_x);
	fftw_destroy_plan(plan_x);

	double *By = (double *)Bky;
	fftw_plan plan_y = fftw_plan_dft_c2r_3d(n, n, n, Bky, By, FFTW_ESTIMATE);
	fftw_execute(plan_y);
	fftw_destroy_plan(plan_y);

	double *Bz = (double *)Bkz;
	fftw_plan plan_z = fftw_plan_dft_c2r_3d(n, n, n, Bkz, Bz, FFTW_ESTIMATE);
	fftw_execute(plan_z);
	fftw_destroy_plan(plan_z);

	// save to grid
	int i;
	for (size_t ix = 0; ix < n; ix++) {
		for (size_t iy = 0; iy < n; iy++) {
			for (size_t iz = 0; iz < n; iz++) {
				i = ix * n * 2 * n2 + iy * 2 * n2 + iz;
				Vector3d &b = grid->get(ix, iy, iz);
				b.x = Bx[i];
				b.y = By[i];
				b.z = Bz[i];
			}
		}
	}
}

Vector3d GridTurbulenceDouble::getMeanFieldVector() const {
	return meanFieldVector(gridPtr);
}

double GridTurbulenceDouble::getMeanFieldStrength() const {
	return meanFieldStrength(gridPtr);
}

double GridTurbulenceDouble::getRmsFieldStrength() const {
	return rmsFieldStrength(gridPtr);
}

std::array<double, 3> GridTurbulenceDouble::getRmsFieldStrengthPerAxis() const {
	return rmsFieldStrengthPerAxis(gridPtr);
}
	
} // namespace crpropa

#endif // CRPROPA_HAVE_FFTW3
