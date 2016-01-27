// Simon Spannagel (DESY) January 2016

#include "GblTrajectory.h"

#include "propagate.h"
#include "materials.h"
#include "constants.h"

using namespace std;
using namespace gbl;
using namespace gblsim;

int main() {

  /*
   * Create points on initial trajectory, create trajectory from points,
   * fit and write trajectory to MP-II binary file,
   * get track parameter corrections and covariance matrix at points.
   *
   * Equidistant measurement layers and thin scatterers, propagation
   * with simple jacobian (quadratic in arc length differences).
   */

  
  //----------------------------------------------------------------------------
  // Preparation of the particle trajectory:
  
  // Label for the DUT positions
  int dut_label_pad1 = 0;
  int dut_label_pad2 = 0;

  // List of points along the trajectory:
  std::vector<GblPoint> listOfPoints;

  // Position along the trajectory:
  double arclength = 0; // Absolute position
  double distance = 0; // Position relative to last point
  
  // Telescope properties:
  double analog_plane = 285e-3 / X0_Si;

  // Distance between planes:
  double dz = 100; // mm
  double dz_dut = 100; // mm

  
  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  // Analog plane
  GblPoint plane = getPoint(distance,resolution_analog,getScatterer(beam_energy,analog_plane));
  listOfPoints.push_back(plane);

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add air:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.58 * dz; arclength += distance; // [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)

  // Factor 0.5 for the air as it is split into two scatterers:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Analog plane
  plane = getPoint(distance,resolution_analog,getScatterer(beam_energy,analog_plane));
  listOfPoints.push_back(plane);

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

    // Add air:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.58 * dz; arclength += distance; // [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)

  // Factor 0.5 for the air as it is split into two scatterers:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add PAD1
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,500e-3/X0_Diamond)));
  dut_label_pad1 = listOfPoints.size();

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add air:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.58 * dz; arclength += distance; // [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)

  // Factor 0.5 for the air as it is split into two scatterers:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add PAD2
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,500e-3/X0_Diamond)));
  dut_label_pad2 = listOfPoints.size();

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add air:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.58 * dz; arclength += distance; // [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)

  // Factor 0.5 for the air as it is split into two scatterers:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Analog plane
  plane = getPoint(distance,resolution_analog,getScatterer(beam_energy,analog_plane));
  listOfPoints.push_back(plane);

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Add air:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.58 * dz; arclength += distance; // [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)

  // Factor 0.5 for the air as it is split into two scatterers:
  listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*dz/X0_Air)));

  // Propagate
  distance = 0.21 * dz; arclength += distance; // [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)

  // Analog plane
  plane = getPoint(distance,resolution_analog,getScatterer(beam_energy,analog_plane));
  listOfPoints.push_back(plane);


  //----------------------------------------------------------------------------
  // Fit the trajectory:

  GblTrajectory traj(listOfPoints, 0);
  traj.printPoints();

  double Chi2, lostWeight;
  int Ndf;

  traj.fit( Chi2, Ndf, lostWeight );
  cout << " Fit: Chi2=" << Chi2 << ", Ndf=" << Ndf << ", lostWeight=" << lostWeight << endl;
  traj.printTrajectory();

  TVectorD aCorrection(5);
  TMatrixDSym aCovariance(5);

  // Get resolution at position of the DUT:
  traj.getResults(dut_label_pad1, aCorrection, aCovariance);
  double sigma_x_pad1 = sqrt(aCovariance(3,3))*1E3;
  traj.getResults(dut_label_pad2, aCorrection, aCovariance);
  double sigma_x_pad2 = sqrt(aCovariance(3,3))*1E3;

  cout << "Track resolution at PAD1: " << sigma_x_pad1 << endl;
  cout << "Track resolution at PAD2: " << sigma_x_pad2 << endl;
  return 0;
}
