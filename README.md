COCPITT Telescope Resolution Simulation
=====

Small collection of scripts for simulation of the telescope resolution for the COCPITT high-rate telescope family.

### First steps

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
  
* read the code and adapt to your needs.
* Add new devices into the `devices` directory
