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
   * Telescope resolution simulation for the DATURA telescope at the DESY TB21 beam line
   * Six MIMOSA26 planes with 150mm spacing, intrinsic sensor resolution 3.4um
   * DUT with variable thickness (scan)
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
  // Preparation of the telescope and beam properties:

  // MIMOSA26 telescope planes consist of 50um silicon plus 2x25um Kapton foil only:
  double MIM26 = 50e-3 / X0_Si + 50e-3 / X0_Kapton;
  
  // The intrinsic resolution has been measured to be around 3.4um:
  double RES = 3.4e-3;
  
  // Beam energy 5 GeV electrons/positrons at DESY:
  double BEAM = 5.0;

  
  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  // Upstream telescope arm:
  plane pl0(0,MIM26,true,RES);
  plane pl1(150,MIM26,true,RES);
  plane pl2(300,MIM26,true,RES);

  // Downstream telescope arm:
  plane pl3(400,MIM26,true,RES);
  plane pl4(550,MIM26,true,RES);
  plane pl5(700,MIM26,true,RES);

  // Build a vector of all telescope planes:
  std::vector<plane> datura;
  datura.push_back(pl0);
  datura.push_back(pl1);
  datura.push_back(pl2);
  
  datura.push_back(pl3);
  datura.push_back(pl4);
  datura.push_back(pl5);

  for(double dut_x0 = 0.005; dut_x0 < 15; dut_x0 += 0.1) {

    // Prepare the DUT (no measurement, just scatterer
    plane dut(350, dut_x0, false);

    // Duplicate the planes vector and add the current DUT:
    std::vector<plane> planes = datura;
    planes.push_back(dut);

    // Build the telescope:
    telescope mytel(planes, BEAM);

    // Get the resolution at plane-vector position (x):
    LOG(logRESULT) << "Track resolution at DUT with " << dut_x0 << "% X0: " << mytel.getResolution(3);
  }
  
  return 0;
}
