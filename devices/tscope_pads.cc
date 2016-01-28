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

int main(int argc, char* argv[]) {

  /*
   * Create points on initial trajectory, create trajectory from points,
   * fit and write trajectory to MP-II binary file,
   * get track parameter corrections and covariance matrix at points.
   *
   * Equidistant measurement layers and thin scatterers, propagation
   * with simple jacobian (quadratic in arc length differences).
   */

  
  Log::ReportingLevel() = Log::FromString("INFO");

  for (int i = 1; i < argc; i++) {
    // Setting verbosity:
    if (std::string(argv[i]) == "-v") { 
      Log::ReportingLevel() = Log::FromString(std::string(argv[++i]));
      continue;
    } 
  }
  
  //----------------------------------------------------------------------------
  // Preparation of the particle trajectory:
  
  // Telescope properties:
  double analog_plane = 285e-3 / X0_Si + 500e-3 / X0_Si + 700e-3 / X0_PCB;
  double diamond_pad = 20e-3 / X0_Al + 500e-3 / X0_Diamond + 20e-3 / X0_Al;
  
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
  LOG(logRESULT) << "Four-plane tracking:";
  LOG(logRESULT) << "Track resolution at PAD1: " << mytel.getResolution(2);
  LOG(logRESULT) << "Track resolution at PAD2: " << mytel.getResolution(3);


  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  plane apl0(0,analog_plane,true,resolution_analog);
  plane apl1(20.32,analog_plane,true,resolution_analog);

  plane apad1(32,diamond_pad,false);
  plane apad2(51,diamond_pad,false);

  std::vector<plane> aplanes;
  aplanes.push_back(pl0);
  aplanes.push_back(pl1);
  aplanes.push_back(pad1);
  aplanes.push_back(pad2);

  telescope amytel(aplanes);
  LOG(logRESULT) << "Two-plane tracking:";
  LOG(logRESULT) << "Track resolution at PAD1: " << amytel.getResolution(2);
  LOG(logRESULT) << "Track resolution at PAD2: " << amytel.getResolution(3);
  
  return 0;
}
