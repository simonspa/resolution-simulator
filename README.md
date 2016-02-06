Telescope Resolution Simulation
=====

Calculator class for telescope track resolution using the General Broken Lines formalism.

This small collection of scripts provides a simple interface for the simulation of telescope resolutions including the effects of multiple scattering in the telescope planes and the surrounding air.


### Features

* Error propagation of track extrapolation uncertainty using GBL
* Includes scattering in material, estimated via the PDG Highland formula
* Automatically accounts for air between the telescope planes
* Automatic placement of the thin scatterers at correct positions
* Planes ordered automatically in `z` for correct GBL trajectory building
* Radiation length for some common materials are defined in `utils/materials.h`


### Installation

* Install and source ROOT
* Install GBL
  (from https://www.wiki.terascale.de/index.php/GeneralBrokenLines)

  ```
  svn checkout http://svnsrv.desy.de/public/GeneralBrokenLines/tags/V01-16-04/cpp GeneralBrokenLines
  cd cpp
  mkdir build && cd build/
  cmake ..
  make && make install
  ```

* Export the GBL library path:
  ```
  export GBLPATH=/path/to/gbl/installation
  ```
  
* Compile this code:

  ```
  mkdir build
  cd build
  cmake ..
  make
  ```

### Prepare your own telescope simulation

* All telescope assemblies are stored in the `devices` directory.
* Take one of the provided examples, adapt it to your needs and add the executable to the `CMakeLists.txt` file in the devices directory in order to compile the executable.
* Have a look at the `devices/tscope_datura.cc` example for some explanatory comments on how to build the telescope assembly.

### 
