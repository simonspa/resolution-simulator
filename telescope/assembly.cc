#include "assembly.h"
#include "log.h"
#include "constants.h"
#include "materials.h"
#include "propagate.h"

#include <algorithm>

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


telescope::telescope(std::vector<gblsim::plane> planes, double beam_energy) :
  m_listOfPoints(),
  m_listOfLabels()
{
  LOG(logINFO) << "Received " << planes.size() << " planes.";

  // Make sure they are ordered in z by sorting the planes vector:
  std::sort(planes.begin(),planes.end());
  
  double arclength = 0;
  double oldpos = 0;

  // Calculate the total material budget to correctly estimate the scattering:
  double total_materialbudget = getTotalMaterialBudget(planes);
  
  // Add first plane:
  std::vector<plane>::iterator pl = planes.begin();
  if(pl->m_measurement) {
    m_listOfPoints.push_back(getPoint(pl->m_position,pl->m_resolution,getScatterer(beam_energy,pl->m_materialbudget,total_materialbudget)));
    LOG(logDEBUG) << "Added plane at " << arclength << " (scatterer + measurement)";
  }
  else {
    m_listOfPoints.push_back(getPoint(pl->m_position,getScatterer(beam_energy,pl->m_materialbudget,total_materialbudget)));
    LOG(logDEBUG) << "Added plane at " << arclength << " (scatterer)";
  }
  oldpos = pl->m_position;
  // Advance the iterator:
  pl++;

  // Store plane label:
  m_listOfLabels.push_back(m_listOfPoints.size());

  // All planes except first:
  for(pl; pl != planes.end(); pl++) {

    // Let's first add the air:
    double plane_distance = pl->m_position - oldpos;
    LOG(logDEBUG2) << "Distance to next plane: " << plane_distance;
    
    // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
    double distance = 0.21 * plane_distance; arclength += distance;

    // Add air:
    m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/X0_Air,total_materialbudget)));
    LOG(logDEBUG3) << "Added air scat at " << arclength;

    // Propagate [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)
    distance = 0.58 * plane_distance; arclength += distance;

    // Factor 0.5 for the air as it is split into two scatterers:
    m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/X0_Air,total_materialbudget)));
    LOG(logDEBUG3) << "Added air scat at " << arclength;

    // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
    distance = 0.21 * plane_distance; arclength += distance;
    LOG(logDEBUG) << "Added air scatterers.";
    
    if(pl->m_measurement) {
      m_listOfPoints.push_back(getPoint(distance,pl->m_resolution,getScatterer(beam_energy,pl->m_materialbudget,total_materialbudget)));
      LOG(logDEBUG) << "Added plane at " << arclength << " (scatterer + measurement)";
    }
    else {
      m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,pl->m_materialbudget,total_materialbudget)));
      LOG(logDEBUG) << "Added plane at " << arclength << " (scatterer)";
    }
    // Update position of previous plane:
    oldpos = pl->m_position;
    // Store plane label:
    m_listOfLabels.push_back(m_listOfPoints.size());

  }

  LOG(logDEBUG) << "Finished building trajectory.";
}

double telescope::getTotalMaterialBudget(std::vector<gblsim::plane> planes) {

  LOG(logDEBUG) << "Calculating total material budget in the particle path...";

  double total_materialbudget = 0;
  std::vector<plane>::iterator p = planes.begin();

  // Add the plane as scatterer:
  LOG(logDEBUG2) << "Adding x/X0=" << p->m_materialbudget;
  total_materialbudget += p->m_materialbudget;
  double oldpos = p->m_position;

  // Advance the iterator:
  p++;

  for(p; p != planes.end(); p++) {
    double plane_distance = p->m_position - oldpos;

    // Add the air as scattering material:
    LOG(logDEBUG2) << "Adding x/X0=" << (plane_distance/X0_Air) << " (air)";
    total_materialbudget += plane_distance/X0_Air;

    // Add the plane as scatterer:
    LOG(logDEBUG2) << "Adding x/X0=" << p->m_materialbudget;
    total_materialbudget += p->m_materialbudget;

    // Update position of previous plane:
    oldpos = p->m_position;
  }

  LOG(logDEBUG) << "Total track material budget x/X0=" << total_materialbudget;
  return total_materialbudget;
}

GblTrajectory telescope::getTrajectory() {

  GblTrajectory traj(m_listOfPoints, 0);
  IFLOG(logDEBUG2) { traj.printPoints(); }
  return traj;
}

double telescope::getResolution(int plane) {

  GblTrajectory tr = getTrajectory();

  double c2, lw;
  int ndf;

  tr.fit(c2, ndf, lw);
  LOG(logDEBUG2) << " Fit: Chi2=" << c2 << ", Ndf=" << ndf << ", lostWeight=" << lw;
  IFLOG(logDEBUG2) { tr.printTrajectory(); }

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
    LOG(logDEBUG) << "Plane " << l << " label " << m_listOfLabels.at(l);
  }
}
