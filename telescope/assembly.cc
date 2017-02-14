#include "assembly.h"
#include "log.h"
#include "constants.h"
#include "materials.h"
#include "propagate.h"

#include <algorithm>

using namespace gblsim;
using namespace unilog;
using namespace gbl;

plane plane::reference(double position)
{
  return plane(position, false, 0.0, false, {0.0, 0.0});
}

plane plane::inactive(double position, double material)
{
  return plane(position, true, material, false, {0.0, 0.0});
}

plane plane::active(double position, double material, double resolution)
{
  return plane(position, true, material, true, {resolution, resolution});
}

plane plane::active(double position, double material, std::pair<double, double> resolution)
{
  return plane(position, true, material, true, resolution);
}

plane::plane(double position,
             bool has_scatterer, double material,
             bool has_measurement, std::pair<double,double> resolution) :
  m_measurement(has_measurement),
  m_resolution(2),
  m_scatterer(has_scatterer),
  m_materialbudget(material),
  m_position(position)
{
  m_resolution[0] = std::get<0>(resolution);
  m_resolution[1] = std::get<1>(resolution);
}

plane::plane(double position, double material, bool has_measurement, double resolution) :
  plane(position, true, material, has_measurement, std::make_pair(resolution, resolution)) {}

plane::plane() : plane(0, false, 0, false, std::make_pair(0.0, 0.0)) {}

telescope::telescope(std::vector<gblsim::plane> planes, double beam_energy, double material) :
  m_volumeMaterial(material),
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
    double distance = 0;
    
    // Check if a volume scatterer with radiation length != 0 has been defined:
    if(m_volumeMaterial > 0.0) {
      // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
      distance = 0.21 * plane_distance; arclength += distance;

      // Add volume scatterer:
      m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/m_volumeMaterial,total_materialbudget)));
      LOG(logDEBUG3) << "Added volume scat at " << arclength;

      // Propagate [mm] 0.58 = from 0.21 to 0.79 = 0.5 + 1/sqrt(12)
      distance = 0.58 * plane_distance; arclength += distance;

      // Factor 0.5 for the volume as it is split into two scatterers:
      m_listOfPoints.push_back(getPoint(distance,getScatterer(beam_energy,0.5*plane_distance/m_volumeMaterial,total_materialbudget)));
      LOG(logDEBUG3) << "Added volume scat at " << arclength;

      // Propagate [mm] from 0 to 0.21 = 0.5 - 1/sqrt(12)
      distance = 0.21 * plane_distance; arclength += distance;
      LOG(logDEBUG) << "Added volume scatterers.";
    }
    else {
      // No volume scatterer defined (vacuum), simply propagate to the next plane:
      distance = plane_distance;
    }
    
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

  // Add the planes as scatterer:
  for(std::vector<plane>::iterator p = planes.begin(); p != planes.end(); p++) {
    LOG(logDEBUG2) << "Adding x/X0=" << p->m_materialbudget;
    total_materialbudget += p->m_materialbudget;
  }

  if(m_volumeMaterial > 0.0) {
    // Add the air as scattering material:
    double total_distance = (planes.back().m_position - planes.front().m_position);
    LOG(logDEBUG2) << "Adding x/X0=" << (total_distance/m_volumeMaterial) << " (air)";
    total_materialbudget += total_distance/m_volumeMaterial;
  }
  
  LOG(logDEBUG) << "Total track material budget x/X0=" << total_materialbudget;
  return total_materialbudget;
}

GblTrajectory telescope::getTrajectory() {

  GblTrajectory traj(m_listOfPoints, 0);
  IFLOG(logDEBUG2) { traj.printPoints(); }
  return traj;
}

std::pair<double,double> telescope::getFullResolution(int plane) {

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
  return std::make_pair(sqrt(aCov(3,3))*1E3,sqrt(aCov(4,4))*1E3);
}

double telescope::getResolution(int plane) {
  return std::get<0>(getFullResolution(plane));
}

void telescope::printLabels() {

  for(size_t l = 0; l < m_listOfLabels.size(); l++) {
    LOG(logDEBUG) << "Plane " << l << " label " << m_listOfLabels.at(l);
  }
}
