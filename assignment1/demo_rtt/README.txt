This is a demo on how to use the RTT. To build it, extract the sources, create a build folder in the top-level folder, change to it and run "cmake .." to configure the project. Running "west build" should build the sources and "west flash" should flash them on your Thingy (must be on and connected via the JLink programmer).

The whole thing can be executed by running the following sequence of commands (Linux) in the extracted folder:

mkdir build && cd build && cmake .. && west build && west flash

If flashed, keep the Thingy connected. Run the JLinkRTTViewer and connect using the "NRF52832_XXAB" device with SWD interface and a frequency of 4000 kHz, auto-detection on the control block. In case of success, you should be able to read some output "%i Test", where %i is an integer that increases on every iteration by one.