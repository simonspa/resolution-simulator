GBL Track Resolution Calculator
=====
[![DOI](https://zenodo.org/badge/doi/10.5281/zenodo.48795.svg)](http://dx.doi.org/10.5281/zenodo.48795)

Calculator class for telescope track resolution using the General Broken Lines formalism.

This small collection of scripts provides a simple interface for the simulation of telescope resolutions including the effects of multiple scattering in the telescope planes and the surrounding air.


### Features

* Error propagation of track extrapolation uncertainty using GBL
* Includes scattering in material, estimated via the PDG Highland formula
* Automatically accounts for air between the telescope planes
* Allows to exchange the volume material considered for scattering (Air, vacuum, Helium...)
* Automatic placement of the thin scatterers at correct positions
* Planes ordered automatically in `z` for correct GBL trajectory building
* Radiation length for some common materials are defined in `utils/materials.h`
* The scattering is correctly treated by using the total scattering material of the track and weighting the individual scatterer contributions with their respective material budget.


### Installation

* Install and source ROOT (from https://root.cern.ch/), either ROOT5 or ROOT6 will work fine.

* Install GBL
  (from https://www.wiki.terascale.de/index.php/GeneralBrokenLines)

  ```
  svn checkout http://svnsrv.desy.de/public/GeneralBrokenLines/tags/V01-18-00/cpp GeneralBrokenLines
  cd GeneralBrokenLines
  mkdir build && cd build/
  cmake ..
  make && make install
  ```

* Export the GBL library path:
  ```
  export GBLPATH=/path/to/gbl/installation
  ```
  If you exactly follow the above description for GBL installation, it will simply be
  ```
  cd ../
  export GBLPATH=`pwd`
  ```
  
* Clone and compile the GBL Resolution Calculator:

  ```
  cd ../
  git clone https://github.com/simonspa/resolution-simulator.git && cd resolution-simulator/
  mkdir build && cd build
  cmake ..
  make
  ```
  All binaries are now in the build directory under `build/devices/`

### Prepare your own telescope simulation

* All telescope assemblies are stored in the `devices` directory.
* Take one of the provided examples, adapt it to your needs and simply store the `.cc` file in the `devices` directory. CMake will automatically create a build target and compile an executable with the same name as the source file.
* Have a look at the `devices/tscope_datura.cc` example for some explanatory comments on how to build the telescope assembly.

### Further instructions and hints

* The resolution can only be evaluated at a previously defined plane. This can either be a plane with measurements, a scatterer, or a plane with no material attached. They can be defined as follows:

  `gblsim::plane measurement(position, material, TRUE, resolution);` - plane with measurement and scattering material

  `gblsim::plane scatterer(position, material, FALSE);` - plane with scattering material but no measurement

  `gblsim::plane reference(position, 0, FALSE);` - plane with zero material and no measurement (simple reference point)


* The material budget is always given as fractions of radiation lengths. Thus, divide your material thickness by its radiation length, and add up different materials as linear sum, e.g.

  ```
  // MIMOSA26 telescope planes consist of 50um silicon plus 2x25um Kapton foil:
  double MIM26 = 50e-3 / X0_Si + 50e-3 / X0_Kapton;
  ```

* The resolution should always be given as intrinsic resolution of the respective sensor in units of micrometer.

* The constructor of the telescope class takes the radiation length of the surrounding volume as optional parameter:

  `telescope(std::vector<gblsim::plane> planes, double beam_energy, double material = X0_Air);`

  It defaults to the radiation length of dry air but can be replaced with other materials or with vacuum (`X0 = 0`) for comparison.

### License and Citation

This software is published under the terms of the GNU Lesser General Public License v3.0 (LGPLv3). Please refer to the LICENSE.md file for more information.

If you use this software for your scientific research, please cite it as

```S. Spannagel and H. Jansen, GBL Track Resolution Calculator v2.0, 2016. doi:10.5281/zenodo.48795```
