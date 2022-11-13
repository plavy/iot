This is a standard-sample, much like a hello-world for the Thingy. To build it, extract the sources, create a build folder in the top-level folder, change to it and run "cmake .." to configure the project. Running "west build" should build the sources and "west flash" should flash them on your Thingy (must be on and connected via the JLink programmer).

The whole thing can be executed by running the following sequence of commands (Linux) in the extracted folder:

mkdir build && cd build && cmake .. && west build && west flash