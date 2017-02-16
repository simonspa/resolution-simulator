#include "TMatrixD.h"
#include "GblTrajectory.h"

namespace gblsim {
  
  TMatrixD Jac5(double ds);
  double getTheta(double energy, double radlength, double total_radlength);
  TVectorD getScatterer(double energy, double radlength, double total_radlength);
  gbl::GblPoint getPoint(double dz, const TVectorD& res, const TVectorD& wscat);
  gbl::GblPoint getPoint(double dz, const TVectorD& wscat);
  gbl::GblPoint getMarker(double dz);

}
