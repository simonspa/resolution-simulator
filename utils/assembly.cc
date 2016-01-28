#include "assembly.h"
#include "log.h"
#include "constants.h"
#include "materials.h"
#include "propagate.h"

using namespace gblsim;
using namespace unilog;
using namespace gbl;

plane::plane() :
  m_measurement(false),
  m_resolution(0),
  m_scatterer(false),
  m_materialbudget(0),
  m_position(0) {}

plane::plane(double position, double material, bool measurement, double resolution) :
  m_measurement(measurement),
  m_scatterer(true),
  m_materialbudget(material),
  m_resolution(resolution),
  m_position(position) {}


telescope::telescope(std::vector<gblsim::plane> planes) :
  m_listOfPoints(),
  m_listOfLabels()
{
  LOG(logINFO) << "Received " << planes.size() << " planes.";

  // FIXME make sure they are ordered in z...
  double arclength = 0;
  double oldpos = 0;

  // Add first plane:
  std::vector<plane>::iterator pl = planes.begin();
  if(pl->m_measurement) {
    m_listOfPoints.push_back(getPoint(pl->m_position,pl->m_resolution,getScatterer(beam_energy,pl->m_materialbudget)));
  }
  else {
    m_listOfPoints.push_back(getPoint(pl->m_position,getScatterer(beam_energy,pl->m_materialbudget)));
  }
  oldpos = pl->m_position;
  // Advance the iterator:
  pl++;
  // Store plane label:
  m_listOfLabels.push_back(m_listOfPoints.size());
  LOG(logINFO) << "Added first plane at " << arclength;

  // All planes except first:
  for(pl; pl != planes.end(); pl++) {
    // Let's first add the air:
    double plane_distance = pl->m_position - oldpos;
    LOG(logDEBUG) << "Distance to next plane: " << plane_distance;
    
    // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
    double distance = 0.21 * plane_distance; arclength += distance;
    // Add air:
    m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/X0_Air)));
    LOG(logDEBUG2) << "Added air scat at " << arclength;
    // Propagate [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)
    distance = 0.58 * plane_distance; arclength += distance;
    // Factor 0.5 for the air as it is split into two scatterers:
    m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/X0_Air)));
    LOG(logDEBUG2) << "Added air scat at " << arclength;
    // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
    distance = 0.21 * plane_distance; arclength += distance;
    LOG(logINFO) << "Added air scatterers.";
    
    if(pl->m_measurement) {
      m_listOfPoints.push_back(getPoint(distance,pl->m_resolution,getScatterer(beam_energy,pl->m_materialbudget)));
    }
    else {
      m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,pl->m_materialbudget)));
    }
    // Update position of previous plane:
    oldpos = pl->m_position;
    // Store plane label:
    m_listOfLabels.push_back(m_listOfPoints.size());
    LOG(logINFO) << "Added plane at " << arclength;
  }

  LOG(logINFO) << "Finished building trajectory.";
}

GblTrajectory telescope::getTrajectory() {

  GblTrajectory traj(m_listOfPoints, 0);
  IFLOG(logDEBUG) { traj.printPoints(); }
  return traj;
}

double telescope::getResolution(int plane) {

  GblTrajectory tr = getTrajectory();

  double c2, lw;
  int ndf;

  tr.fit(c2, ndf, lw);
  LOG(logDEBUG) << " Fit: Chi2=" << c2 << ", Ndf=" << ndf << ", lostWeight=" << lw;
  tr.printTrajectory();

  TVectorD aCorr(5);
  TMatrixDSym aCov(5);

  // Get resolution at position of the DUT:
  if(plane < m_listOfLabels.size()) {
    tr.getResults(m_listOfLabels.at(plane), aCorr, aCov);
  }
  return sqrt(aCov(3,3))*1E3;
}

void telescope::printLabels() {

  for(size_t l = 0; l < m_listOfLabels.size(); l++) {
    LOG(logINFO) << "Plane " << l << " label " << m_listOfLabels.at(l);
  }
}
