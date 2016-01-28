// Simon Spannagel (DESY) January 2016

#include "GblTrajectory.h"

#include "assembly.h"
#include "propagate.h"
#include "materials.h"
#include "constants.h"
#include "log.h"

using namespace std;
using namespace gbl;
using namespace gblsim;
using namespace unilog;

int main() {

  Log::ReportingLevel() = Log::FromString("DEBUG5");

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
  double diamond_pad = 500e-3 / X0_Diamond;

  // Distance between planes:
  double dz = 100; // mm
  double dz_dut = 100; // mm

  
  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:


  plane pl0(0,analog_plane,true,resolution_analog);
  plane pl1(20.32,analog_plane,true,resolution_analog);

  plane pad1(32,diamond_pad,false);
  plane pad2(51,diamond_pad,false);
    
  plane pl2(81.28,analog_plane,true,resolution_analog);
  plane pl3(101.6,analog_plane,true,resolution_analog);

  std::vector<plane> planes;
  planes.push_back(pl0);
  planes.push_back(pl1);
  planes.push_back(pad1);
  planes.push_back(pad2);
  planes.push_back(pl2);
  planes.push_back(pl3);

  telescope mytel(planes);
  mytel.printLabels();

  LOG(logRESULT) << "Track resolution at PAD1: " << mytel.getResolution(2);
  LOG(logRESULT) << "Track resolution at PAD2: " << mytel.getResolution(3);

  return 0;
}
