// Simon Spannagel (DESY) January 2016

#include "TCanvas.h"
#include "TFile.h"
#include "TProfile.h"
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
     * Create points on initial trajectory, create trajectory from points,
     * fit and write trajectory to MP-II binary file,
     * get track parameter corrections and covariance matrix at points.
     *
     * Equidistant measurement layers and thin scatterers, propagation
     * with simple jacobian (quadratic in arc length differences).
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

    TFile* out = TFile::Open("pad-vs-intrinsic-resolution.root", "RECREATE");
    gDirectory->pwd();

    auto* c1 = new TCanvas("c1", "resolution", 700, 700);
    auto* resolution_pad1 = new TProfile("resolution_p1", " ", 50, 5, 55);
    auto* resolution_pad2 = new TProfile("resolution_p2", " ", 50, 5, 55);

    //----------------------------------------------------------------------------
    // Preparation of the particle trajectory:

    // Telescope properties:
    double analog_plane = 285e-3 / X0_Si + 500e-3 / X0_Si + 700e-3 / X0_PCB;
    double diamond_pad = 20e-3 / X0_Al + 500e-3 / X0_Diamond + 20e-3 / X0_Al;

    // Beam: 250 MeV Pi at PSI
    double BEAM = 0.250;

    // Loop over possible intrinsic pixel plane resolutions [um]
    for(double resolution = 5; resolution < 55; resolution++) {

        //----------------------------------------------------------------------------
        // Build the trajectory through the telescope device:

        plane pl0(0, analog_plane, true, resolution * 1e-3);
        plane pl1(20.32, analog_plane, true, resolution * 1e-3);

        plane pad1(32, diamond_pad, false);
        plane pad2(51, diamond_pad, false);

        plane pl2(81.28, analog_plane, true, resolution * 1e-3);
        plane pl3(101.6, analog_plane, true, resolution * 1e-3);

        std::vector<plane> planes;
        planes.push_back(pl0);
        planes.push_back(pl1);
        planes.push_back(pad1);
        planes.push_back(pad2);
        planes.push_back(pl2);
        planes.push_back(pl3);

        telescope mytel(planes, BEAM);
        LOG(STATUS) << "Intrinsic: " << resolution << " Track PAD1: " << mytel.getResolution(2);
        LOG(STATUS) << "Intrinsic: " << resolution << " Track PAD2: " << mytel.getResolution(3) << endl;
        resolution_pad1->Fill(resolution, mytel.getResolution(2), 1);
        resolution_pad2->Fill(resolution, mytel.getResolution(3), 1);
    }

    c1->cd();
    resolution_pad1->SetTitle("Track Resolution at Diamond Pads;Intrinsic resolution PSI46v2 #left[#mum#right];resolution "
                              "at pad #left[#mum#right]");
    resolution_pad1->GetYaxis()->SetRangeUser(0, 100);
    resolution_pad1->SetMarkerStyle(20);
    resolution_pad2->SetMarkerStyle(20);
    resolution_pad1->SetLineColor(kRed + 1);
    resolution_pad2->SetLineColor(kBlack);
    resolution_pad1->SetMarkerColor(kRed + 1);
    resolution_pad2->SetMarkerColor(kBlack);

    resolution_pad1->Draw();
    resolution_pad2->Draw("same");
    c1->Write();

    out->Close();

    return 0;
}
