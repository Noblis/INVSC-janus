Janus
=====

[IARPA Janus Program API](http://www.libjanus.org)

# Build Instructions

    $ git clone https://github.com/biometrics/janus.git
    $ cd janus
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

# Implementations

The Janus CMake file expects the user to specify a JANUS_IMPLEMENTATION variable that gives the full path to the library that contains the implementations of the API functions. This can be done like:

    $ cmake -DJANUS_IMPLEMENTATION="/path/to/janus/implementation" ..

In addition to a JANUS_IMPLEMENTATION, users may also optionally specify a JANUS_IO_IMPLEMENTATION which implements the Janus I/O functions. Similar to JANUS_IMPLEMENTATION it can be set like: 

    $ cmake -DJANUS_IO_IMPLEMENTATION="/path/to/janus/io/implementation" ..

# Building the PittPatt wrapper

This API ships with an optional PittPatt 5 wrapper that can be used to test the functionality. Note that the wrapper requires the PittPatt 5 libaries to be provided separately. To build the PittPatt 5 wrapper you would use the cmake command

    $ cmake -DBUILD_JANUS_PP5_IMPLEMENTATION=ON -DBUILD_JANUS_OPENCV_IO_IMPELEMENTATION=ON ..

CMake will automatically try and locate the PittPatt and OpenCV libraries on your system. If they cannot be found you will be manually prompted to provide the PP5_DIR and OpenCV_DIR variables. The PP5_DIR should point to the root of the PittPatt 5 SDK directory. The OpenCV_DIR should point to the directory that contains OpenCVConfig.cmake. From the command line these values can be set like

    $ cmake -DPP5_DIR=/path/to/pp5 -DOpenCV_DIR=/path/to/opencv ..
