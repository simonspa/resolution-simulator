#include "TMatrixD.h"
#include "GblTrajectory.h"

namespace gblsim {
  
  TMatrixD Jac5(double ds);
  double getTheta(double energy, double radlength, double total_radlength);
  TVectorD getScatterer(double energy, double radlength, double total_radlength);
  gbl::GblPoint getPoint(double dz, double res, TVectorD wscat, bool has_meas = true);
  gbl::GblPoint getPoint(double dz, TVectorD wscat);
  gbl::GblPoint getMarker(double dz);

}
