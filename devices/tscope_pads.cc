// Simon Spannagel (DESY) January 2016

#include "assembly.h"
#include "propagate.h"
#include "materials.h"
#include "constants.h"
#include "log.h"

using namespace std;
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

  // Beam: 250 MeV Pi at PSI
  double BEAM = 0.250;
  
  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:
  LOG(logRESULT) << "Four-plane tracking:";

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

  telescope mytel(planes, BEAM);
  LOG(logRESULT) << "Track resolution (X) at PAD1: " << mytel.getResolution(2);
  LOG(logRESULT) << "Track resolution (X) at PAD2: " << mytel.getResolution(3);


    //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  plane ypl0(0,analog_plane,true,resolution_analog_y);
  plane ypl1(20.32,analog_plane,true,resolution_analog_y);

  plane ypad1(32,diamond_pad,false);
  plane ypad2(51,diamond_pad,false);
    
  plane ypl2(81.28,analog_plane,true,resolution_analog_y);
  plane ypl3(101.6,analog_plane,true,resolution_analog_y);

  std::vector<plane> yplanes;
  yplanes.push_back(ypl0);
  yplanes.push_back(ypl1);
  yplanes.push_back(ypad1);
  yplanes.push_back(ypad2);
  yplanes.push_back(ypl2);
  yplanes.push_back(ypl3);

  telescope ymytel(yplanes, BEAM);
  LOG(logRESULT) << "Track resolution (Y) at PAD1: " << ymytel.getResolution(2);
  LOG(logRESULT) << "Track resolution (Y) at PAD2: " << ymytel.getResolution(3);


  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:
  LOG(logRESULT) << "Two-plane tracking:";

  std::vector<plane> aplanes;
  aplanes.push_back(pl0);
  aplanes.push_back(pl1);
  aplanes.push_back(pad1);
  aplanes.push_back(pad2);

  telescope amytel(aplanes, BEAM);
  LOG(logRESULT) << "Track resolution (X) at PAD1: " << amytel.getResolution(2);
  LOG(logRESULT) << "Track resolution (X) at PAD2: " << amytel.getResolution(3);

  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  std::vector<plane> yaplanes;
  yaplanes.push_back(ypl0);
  yaplanes.push_back(ypl1);
  yaplanes.push_back(ypad1);
  yaplanes.push_back(ypad2);

  telescope yamytel(yaplanes, BEAM);
  LOG(logRESULT) << "Track resolution (Y) at PAD1: " << yamytel.getResolution(2);
  LOG(logRESULT) << "Track resolution (Y) at PAD2: " << yamytel.getResolution(3);

  return 0;
}
