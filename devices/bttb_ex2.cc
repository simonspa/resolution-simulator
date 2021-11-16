// Simon Spannagel (CERN) January 2017

#include "assembly.h"
#include "constants.h"
#include "log.h"
#include "materials.h"
#include "propagate.h"

using namespace std;
using namespace gblsim;
using namespace unilog;

int main(int, char**) {

    /*
     * Telescope resolution simulation for the BTTB5 hands-on exercise 1
     * Beam energy 5GeV
     *
     * Six pixel detectors with 55mm equidistant spacing,
     * Intrinsic sensor resolution 4.5um, thickness 70um Silicon
     *
     * Additional Silicon scatterer at the center of the telescope, 700um
     * Evaluate resolution at the scatterer
     */

    Log::ReportingLevel() = Log::FromString("DEBUG");

    //----------------------------------------------------------------------------
    // Preparation of the telescope and beam properties:

    // Pixel detector planes consist of 70um silicon
    double PXL = 70e-3 / X0_Si;
    // The intrinsic resolution is 4.5um:
    double RES = 4.512e-3;

    // Telescope layout:
    // PXL      PXL      PXL  SCAT PXL      PXL      PXL
    //  |        |        |    |    |        |        |
    //  |        |        |    |    |        |        |
    //  |<------>|        |<-->|<-->|        |        |
    //     DIST          DIST/2 DIST/2
    //  Measure:               X                      X

    // Distance between telescope planes in mm:
    double DIST = 55;

    // Beam energy 5 GeV:
    double BEAM = 5.0;

    // Additional scatterer consist of 700um silicon
    double SCAT = 700e-3 / X0_Si;

    //----------------------------------------------------------------------------
    // Build the trajectory through the telescope device:

    // Build a vector of all telescope planes:
    std::vector<plane> planes;
    double position = 0;

    // Add all six planes:
    for(int i = 0; i < 6; i++) {
        planes.push_back(plane(position, PXL, true, RES));
        position += DIST;
    }

    // Add the additional material (no measurement, just scatterer
    planes.push_back(plane(2.5 * DIST, SCAT, false));

    // Build the telescope:
    telescope mytel(planes, BEAM);

    // Get the resolution at plane-vector position (x):
    LOG(logRESULT) << "Track resolution at SCAT with " << SCAT << "% X0: " << mytel.getResolution(3) << "um";

    LOG(logRESULT) << "Track resolution at first telescope planes: " << mytel.getResolution(0) << "um";
    LOG(logRESULT) << "Track resolution at  last telescope planes: " << mytel.getResolution(6) << "um";

    return 0;
}
