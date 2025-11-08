# README for vPeripherals

vPeripherals is a python implementation of the PC side of the virtual peripherals. 
The rational behind this is that there is only 1 user button and 1 user LED available on the real hardware (STM32H533 board). 
Therefore, the main purpose of this application is to simulate non-existing peripherals. It requires the application running on the 
board to use the plib and at least call the `pl_init()` function to establish a connection over USB/UART to vPeripherals. 
Then, the functions of the plib can be used to control the vPeripherals (e.g. switch on a LED or control the display).

## Dependencies
- A recent Python 3 version 
- The following python packages need to be installed:
  - `sv_ttk`
  - `pyserial` 
  - `darkdetect`
  - `coordinates`

It is probably a good idea to create first a virtual environment. See `run.sh` how to create one and install the dependencies. 
You can also use `run.sh` to run vPeripherals. 

## Running vPeripherals
The application can be run by calling `python vp.py`. There are a couple of commandline options available which can be viewed with `python vp.py --help`. 
Alternatively, you an just execute `run.sh` which installs dependencies and runs the application. It forwards commandline options to the application, so `run.sh --help` also displays all available commandline options. 


## Authors
- Gerhard Jahn 
- Andreas Scheibenpflug