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

The Janus CMake file expects the user to specify a JANUS_IMPLEMENTATION variable that gives the name of the library the Janus utility executables can link against at compile time. Using PittPatt 5 as an example, the command to cmake would be

    $ cmake -DJANUS_IMPLEMENTATION="pittpatt" ..

In addition to a JANUS_IMPLEMENTATION, users may also optionally specify a JANUS_IO_IMPLEMENTATION which implements the Janus I/O functions. A working version that uses the OpenCV library comes with the API. The command to use it is 

    $ cmake -DJANUS_IO_IMPLEMENTATION="opencv_io" ..

# Building the PittPatt wrapper

To build the PittPatt 5 wrapper you would use the cmake command

    $ cmake -DJANUS_IMPLEMENTATION="pittpatt" -DJANUS_IO_IMPLEMENTATION="opencv_io" ..

CMake will automatically try and locate the PittPatt libraries on your system. If they cannot be found you can manually set the PP5_DIR variable, either from the command line or using the CMake GUI. Setting it from the command line can be done like 

    $ cmake -DPP5_DIR=/path/to/pp5 ..
