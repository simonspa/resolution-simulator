// Simon Spannagel (DESY) January 2016
// Hendrik Jansen (DESY) August 2018

#include "TCanvas.h"
#include "TProfile.h"
#include "TString.h"
#include "TFile.h"

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

  
  Log::ReportingLevel() = Log::FromString("DEBUG");

  for (int i = 1; i < argc; i++) {
    // Setting verbosity:
    if (std::string(argv[i]) == "-v") { 
      Log::ReportingLevel() = Log::FromString(std::string(argv[++i]));
      continue;
    } 
  }

  TFile * out = TFile::Open("datura-kink-resolution.root","RECREATE");
  gDirectory->pwd();

  TCanvas *c1 = new TCanvas("c1","resolution",700,700);
  TProfile *resolution = new TProfile("resolution"," ",16,0,160.);

  TCanvas *c2 = new TCanvas("c2","kink_resolution",700,700);
  TProfile *kink_resolution = new TProfile("kink_resolution"," ",16,0,160);

  //----------------------------------------------------------------------------
  // Preparation of the telescope and beam properties:

  // MIMOSA26 telescope planes consist of 50um silicon plus 2x25um Kapton foil only:
  double MIM26 = 55e-3 / X0_Si + 50e-3 / X0_Kapton;
  // The intrinsic resolution has been measured to be around 3.25um:
  double RES = 3.24e-3;

  //        M26  M26  M26        DUT      M26    M26  M26
  //         |    |    |          |        |      |    |
  //  -----> |    |    |          |        |      |    |
  //         |<-->|    |<-------->|<------>|      |<-->|
  //        DIST_up   DUT_DIST_up  DUT_DIST_down  DIST_down
  
  // Distance between telescope planes in mm:
  double DIST_up= 150; 

  // Distance of telescope arms and DUT assembly:
  double DUT_DIST_up = 60;
  double DUT_DIST_down = 40;

  // Beam energy 2 GeV electrons/positrons at DESY, highest rate
  double BEAM = 2.0;

  // Nickel target with 16 mm thickness 
  double dut_x0 = 16./14.24;

  //----------------------------------------------------------------------------
  // Build the trajectory through the telescope device:

  // Build a vector of all telescope planes:
  std::vector<plane> datura;
  double position = 0;
 
  //scan reso and kink reso a.f.o. downstream plane spacing "DIST_down"
  for(double DIST_down = 10.; DIST_down < 155.; DIST_down += 10.) {

    // Upstream telescope arm:
    for(int i = 0; i < 3; i++) {
      datura.push_back(plane(position,MIM26,true,RES));
      position += DIST_up;
    }

    // Downstream telescope arm:
    position = 2*DIST_up + DUT_DIST_up + DUT_DIST_down;
    for(int i = 0; i < 3; i++) {
      datura.push_back(plane(position,MIM26,true,RES));
      position += DIST_down;
    }

    // Prepare the unknown DUT (no measurement, to-be-determined scatterer
    plane dut = plane::unknown(360., 10.);

    // Duplicate the planes vector and add the current DUT:
    std::vector<plane> planes = datura;
    planes.push_back(dut);

    // Build the telescope:
    telescope mytel(planes, BEAM);

    // Get the resolution at plane-vector position (x):
    LOG(logRESULT) << "Track resolution at DUT with " << dut_x0 << "% X0: " << mytel.getResolution(3);
    resolution->Fill(DIST_down,mytel.getResolution(3),1);

    LOG(logRESULT) << "Kink resolution  at DUT with " << dut_x0 << "% X0: " << mytel.getKinkResolution(3);
    kink_resolution->Fill(DIST_down,mytel.getKinkResolution(3),1);

    datura.clear();
    position = 0;
  }

    c1->cd();
  resolution->SetTitle("DATURA Track Resolution at DUT;DUT material budget x/X_{0};resolution at DUT #left[#mum#right]");
  resolution->SetMarkerStyle(0);
  resolution->SetLineColor(kRed+1);
  resolution->SetLineWidth(2);
  resolution->SetMarkerColor(kRed+1);
  
  resolution->Draw();
  c1->Write();

    c2->cd();
  kink_resolution->SetTitle("DATURA Kink Track Resolution at DUT;dz of downstream planes [mm]; kink resolution at DUT #left[#murad#right]");
  kink_resolution->SetMarkerStyle(0);
  kink_resolution->SetLineColor(kRed+1);
  kink_resolution->SetLineWidth(2);
  kink_resolution->SetMarkerColor(kRed+1);
  
  kink_resolution->Draw();
  c2->Write();


  // Write result to file
  out->Write();
  return 0;
}
