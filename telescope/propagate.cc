#include "propagate.h"

gbl::Matrix5d gblsim::Jac5(double ds) {
  /* 
     straight line, no B-field
     track = 
     q/p, x', y', x, y
     0,   1,  2,  3, 4
  */
  gbl::Matrix5d jac;
  jac.setIdentity();
  jac(3, 1) = ds; // x = xp * ds
  jac(4, 2) = ds; // y = yp * ds
  return jac;
}

double gblsim::getTheta(double energy, double radlength, double total_radlength) {

  // Return the scattering distribution with Theta according to the Highland forumla
  // http://pdg.lbl.gov/2015/reviews/rpp2014-rev-passage-particles-matter.pdf (Equation 32.15)

  // Radiation length fraction with no unit (thickness / rad. length), particle energy in [GeV]
  return (0.0136*sqrt(radlength)/energy*(1+0.038*log(total_radlength)));
}

Eigen::Vector2d gblsim::getScatterer(double energy, double radlength, double total_radlength) {
  Eigen::Vector2d scat;

  double theta = getTheta(energy,radlength,total_radlength);
  scat << 1.0 / (theta*theta), 1.0 / (theta*theta);

  return scat;
}

// construct a GblPoint with a scatterer and a measurement
gbl::GblPoint gblsim::getPoint(double dz, const Eigen::Vector2d& res, const Eigen::Vector2d& wscat) {

  // Propagate:
  auto jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  // Add scatterer:
  Eigen::Vector2d scat(0., 0.);
  point.addScatterer(scat, wscat);
  
  // Add measurement:
  // measurement = residual
  Eigen::Vector2d meas;
  meas.setZero(); // ideal
  // Precision = 1/resolution^2
  Eigen::Vector2d measPrec;
  measPrec << 1.0 / res[0] / res[0], 1.0 / res[1] / res[1];

  // measurement plane == propagation plane
  Eigen::Matrix2d proL2m;
  proL2m.setIdentity();
  point.addMeasurement(proL2m, meas, measPrec);

  return point;
}

gbl::GblPoint gblsim::getPoint(double dz, double res, const Eigen::Vector2d& wscat)
{
  
  Eigen::Vector2d res2;
  res2 << res, res;
  return getPoint(dz, res2, wscat);
}

// construct a GblPoint with only a scatterer
gbl::GblPoint gblsim::getPoint(double dz, const Eigen::Vector2d& wscat) {

  // Propagate:
  auto jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  // Add scatterer:
  Eigen::Vector2d scat;
  scat.setZero(); // mean is zero
  point.addScatterer(scat, wscat);

  return point;
}

gbl::GblPoint gblsim::getMarker(double dz) {

  // Propagate:
  auto jacPointToPoint = Jac5(dz);
  gbl::GblPoint point(jacPointToPoint);

  return point;
}
