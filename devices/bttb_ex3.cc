// Simon Spannagel (DESY) January 2016

#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TString.h"

#include "assembly.h"
#include "constants.h"
#include "log.h"
#include "materials.h"
#include "propagate.h"

using namespace std;
using namespace gblsim;
using namespace unilog;

int main(int argc, char* argv[]) {

    /*
     * Telescope resolution simulation for the DATURA telescope at the DESY TB21 beam line
     * Six MIMOSA26 planes with 150mm spacing, intrinsic sensor resolution 3.4um
     * DUT with variable thickness (scan)
     */

    // Add cout as the default logging stream
    Log::addStream(std::cout);
    Log::setReportingLevel(LogLevel::INFO);

    for(int i = 1; i < argc; i++) {
        // Setting verbosity:
        if(std::string(argv[i]) == "-v") {
            try {
                LogLevel log_level = Log::getLevelFromString(std::string(argv[++i]));
                Log::setReportingLevel(log_level);
            } catch(std::invalid_argument& e) {
                LOG(ERROR) << "Invalid verbosity level \"" << std::string(argv[i]) << "\", ignoring overwrite";
            }
        }
    }

    TFile* out = TFile::Open("datura-plane-distance.root", "RECREATE");
    gDirectory->pwd();

    auto* c1 = new TCanvas("c1", "resolution", 700, 700);
    auto* resolution = new TGraph(130);

    auto* resolution2 = new TGraph(130);

    //----------------------------------------------------------------------------
    // Preparation of the telescope and beam properties:

    // MIMOSA26 telescope planes consist of 50um silicon plus 2x25um Kapton foil only:
    double MIM26 = 50e-3 / X0_Si + 50e-3 / X0_Kapton;
    // The intrinsic resolution has been measured to be around 3.4um:
    double RES = 3.25e-3;

    // M26  M26  M26      DUT      M26  M26  M26
    //  |    |    |        |        |    |    |
    //  |    |    |        |        |    |    |
    //  |<-->|    |<------>|<------>|    |    |
    //   DIST      DUT_DIST DUT_DIST

    // Distance of telescope arms and DUT assembly:
    double DUT_DIST = 20;

    // Beam energy 5 GeV electrons/positrons at DESY:
    double BEAM = 5.0;

    double DUT_X0_1 = 0.001;
    double DUT_X0_2 = 0.01;

    //----------------------------------------------------------------------------
    // Build the trajectory through the telescope device:

    for(double dist = 20; dist < 151; dist += 1.) {
        // Build a vector of all telescope planes:
        std::vector<plane> planes;
        double position = 0;

        // Upstream telescope arm:
        for(int i = 0; i < 3; i++) {
            planes.emplace_back(position, MIM26, true, RES);
            position += dist;
        }

        // Downstream telescope arm:
        position = 2 * dist + 2 * DUT_DIST;
        for(int i = 0; i < 3; i++) {
            planes.emplace_back(position, MIM26, true, RES);
            position += dist;
        }

        // Prepare the DUT (no measurement, just scatterer
        plane dut1(2 * dist + DUT_DIST, DUT_X0_1, false);
        plane dut2(2 * dist + DUT_DIST, DUT_X0_2, false);

        // Build the telescope:
        planes.push_back(dut1);
        telescope mytel1(planes, BEAM);
        // Get the resolution at plane-vector position (x):
        LOG(STATUS) << "Track resolution at DUT with plane dist " << dist << "mm " << mytel1.getResolution(3);
        resolution->SetPoint(static_cast<int>(dist - 20), dist, mytel1.getResolution(3));

        planes.pop_back();
        planes.push_back(dut2);
        telescope mytel2(planes, BEAM);
        LOG(STATUS) << "Track resolution at DUT with plane dist " << dist << "mm " << mytel2.getResolution(3);
        resolution2->SetPoint(static_cast<int>(dist - 20), dist, mytel2.getResolution(3));
    }

    c1->cd();
    resolution->SetTitle(
        "DATURA Track Resolution at DUT;DATURA plane distance #left[mm#right];resolution at DUT #left[#mum#right]");
    resolution->GetYaxis()->SetRangeUser(1.5, 3.5);
    resolution->GetYaxis()->SetTitleOffset(1.3f);
    resolution->GetXaxis()->SetTitleOffset(1.3f);

    resolution->SetMarkerStyle(0);
    resolution->SetLineColor(kRed + 1);
    resolution->SetLineWidth(2);
    resolution->SetMarkerColor(kRed + 1);

    resolution2->SetLineColor(kBlue + 1);
    resolution2->SetLineWidth(2);

    auto* leg = new TLegend();
    leg->SetX1NDC(0.52);
    leg->SetY1NDC(0.79);
    leg->SetX2NDC(0.795);
    leg->SetY2NDC(0.865);

    leg->AddEntry(resolution, "DUT with 0.001% X_{0}", "l");
    leg->AddEntry(resolution2, "DUT with  0.01% X_{0}", "l");
    leg->SetTextFont(42);
    leg->SetTextAlign(12);
    leg->SetTextSize(0.04f);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);

    resolution->Draw();
    resolution2->Draw("SAME");
    leg->Draw();

    c1->Write();
    out->Close();

    return 0;
}
