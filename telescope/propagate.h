#include <Eigen/Core>

#include "GblTrajectory.h"
#include "GblData.h"

namespace gblsim {

  gbl::Matrix5d Jac5(double ds);
  double getTheta(double energy, double radlength, double total_radlength);
  Eigen::Vector2d getScatterer(double energy, double radlength, double total_radlength);
  gbl::GblPoint getPoint(double dz, double res, const Eigen::Vector2d& wscat);
  gbl::GblPoint getPoint(double dz, const Eigen::Vector2d& res, const Eigen::Vector2d& wscat);
  gbl::GblPoint getPoint(double dz, const Eigen::Vector2d& wscat);
  gbl::GblPoint getMarker(double dz);

}
