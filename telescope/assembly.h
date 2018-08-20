#include <utility>

#include "GblTrajectory.h"
#include "materials.h"

namespace gblsim {
  class plane {
  public:
    // Virtual reference plane w/o material or measurement
    static plane reference(double position);
    // A plane with material but no measurement
    static plane inactive(double position, double material);
    // A plane with material and identical resolution along each axis
    static plane active(double position, double material, double resolution);
    // A plane with material and different resolutions along each axis
    static plane active(double position, double material, std::pair<double,double> resolution);
    // A plane with unkonwn material, no measurement, and a certain size
    static plane unknown(double position, double size);

    plane();
    plane(double position, double material, bool measurement, double resolution = 0, double size = -1.);
    plane(double position, bool scatterer, bool measurement, double size);

    double position() const { return m_position; }

    bool operator < (const plane& pl) const {
        return (m_position < pl.m_position);
    }


  private:
    plane(double position,
          bool has_scatterer, double material,
          bool has_measurement, std::pair<double,double> resolution,
	  double size);

    bool m_measurement;
    TVectorD m_resolution;
    bool m_scatterer;
    double m_materialbudget;
    double m_position;
    double m_size;

    void print() {
      std::cout << "Plane position = " << m_position << std::endl;
      std::cout << "Plane res[0] = " << m_resolution[0] << std::endl;
      std::cout << "Plane res[1] = " << m_resolution[1] << std::endl;
    }

    friend class telescope;
  };

  class telescope {
  public:
    telescope(std::vector<gblsim::plane> planes, double beam_energy, double material = X0_Air);

    // Return the trajectory
    gbl::GblTrajectory getTrajectory() const;

    // Return the resolution along the first dimension at given plane:
    double getResolution(int plane) const;
    // Return the resolution in both dimensions on the given plane
    std::pair<double,double> getResolutionXY(int plane) const;

    // Return the kink resolution along the first dimension at given plane:
    double getKinkResolution(int plane) const;
    // Return the kink resolution in both dimensions on the given plane
    std::pair<double,double> getKinkResolutionXY(int plane) const;

    void printLabels() const;
  private:
    // Radiationlength of the material of the surrounding volume, defaults to dry air:
    double m_volumeMaterial;
    
    double getTotalMaterialBudget(const std::vector<plane>& planes) const;
    std::vector<gbl::GblPoint> m_listOfPoints;
    std::vector<int> m_listOfLabels;
    unsigned int m_parameter;
  };
}
