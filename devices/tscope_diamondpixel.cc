// Simon Spannagel (DESY) January 2016

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

    //----------------------------------------------------------------------------
    // Preparation of the particle trajectory:

    // Telescope properties:
    double analog_plane = 285e-3 / X0_Si + 500e-3 / X0_Si + 700e-3 / X0_PCB;
    double diamond_plane =
        40e-3 / X0_Au + 1550e-3 / X0_PCB + 40e-3 / X0_Au + 700e-3 / X0_Si + 500e-3 / X0_Diamond + 10e-3 / X0_Au;
    double digital_plane = 1550e-3 / X0_PCB + 700e-3 / X0_Si + 285e-3 / X0_Si;

    // Beam: 250 MeV Pi at PSI
    double BEAM = 0.250;

    //----------------------------------------------------------------------------
    // Build the trajectory through the telescope device:

    plane pl0(0, analog_plane, true, resolution_analog);
    plane pl1(20.32, analog_plane, true, resolution_analog);

    plane diamond1(60.96, diamond_plane, false);
    plane diamond2(81.28, diamond_plane, false);
    plane silicon(101.6, digital_plane, true, resolution_digital);
    // plane silicon(101.6,digital_plane,false);

    plane pl2(142.24, analog_plane, true, resolution_analog);
    plane pl3(162.56, analog_plane, true, resolution_analog);

    std::vector<plane> planes;
    planes.push_back(pl0);
    planes.push_back(pl1);
    planes.push_back(diamond1);
    planes.push_back(diamond2);
    planes.push_back(silicon);
    planes.push_back(pl2);
    planes.push_back(pl3);

    telescope mytel(planes, BEAM);
    LOG(STATUS) << "Track resolution at Diamond 1: " << mytel.getResolution(2);
    LOG(STATUS) << "Track resolution at Diamond 2: " << mytel.getResolution(3);

    return 0;
}
