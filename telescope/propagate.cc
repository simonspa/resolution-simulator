#include "propagate.h"

TMatrixD gblsim::Jac5(double ds) {
  /* 
     straight line, no B-field
     track = 
     q/p, x', y', x, y
     0,   1,  2,  3, 4
  */
  TMatrixD jac(5, 5);
  jac.UnitMatrix();
  jac[3][1] = ds; // x = xp * ds
  jac[4][2] = ds; // y = yp * ds
  return jac;
}

double gblsim::getTheta(double energy, double radlength, double total_radlength) {

  // Return the scattering distribution with Theta according to the Highland forumla
  // http://pdg.lbl.gov/2015/reviews/rpp2014-rev-passage-particles-matter.pdf (Equation 32.15)

  // Radiation length fraction with no unit (thickness / rad. length), particle energy in [GeV]
  return (0.0136*sqrt(radlength)/energy*(1+0.038*log(total_radlength)));
}

TVectorD gblsim::getScatterer(double energy, double radlength, double total_radlength) {
  TVectorD scat(2);

  double theta = getTheta(energy,radlength,total_radlength);
  scat[0] = 1.0 / (theta*theta);
  scat[1] = 1.0 / (theta*theta);

  return scat;
}

// construct a GblPoint with a scatterer and a measurement
gbl::GblPoint gblsim::getPoint(double dz, const TVectorD& res, const TVectorD& wscat) {

  // Propagate:
  TMatrixD jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  // Add scatterer:
  TVectorD scat(2);
  scat.Zero(); // mean is zero
  point.addScatterer(scat, wscat);
  
  // Add measurement:
  // measurement = residual
  TVectorD meas(2);
  meas.Zero(); // ideal
  // Precision = 1/resolution^2
  TVectorD measPrec(2);
  measPrec[0] = 1.0 / res[0] / res[0];
  measPrec[1] = 1.0 / res[1] / res[1];
  // measurement plane == propagation plane
  TMatrixD proL2m(2,2);
  proL2m.UnitMatrix();
  point.addMeasurement(proL2m, meas, measPrec);

  return point;
}

// construct a GblPoint with only a scatterer
gbl::GblPoint gblsim::getPoint(double dz, const TVectorD& wscat) {

  // Propagate:
  TMatrixD jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  // Add scatterer:
  TVectorD scat(2);
  scat.Zero(); // mean is zero
  point.addScatterer(scat, wscat);

  return point;
}

gbl::GblPoint gblsim::getMarker(double dz) {

  // Propagate:
  TMatrixD jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  return point;
}
