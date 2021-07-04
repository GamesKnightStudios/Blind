# Blind
![Blind title screen](/docs/blind_title.png?raw=true)

BLIND, by Games Knight Studios.  
Created as part of GBJam8 on itch.io

# Latest release
Get the latest release on [itch.io](https://gamesknightstudios.itch.io/blind)

# Developer Zone
The full source code for this game is avaiable as OpenSource. Instructions for building from source are provided below.

## Useful packages
The following packages were used to build this game:
 - [GameBoy Development Kit](https://github.com/Zal0/gbdk-2020/releases/latest) (Recommended: v3.2)
 - [BGB GameBoy Emulator](https://bgb.bircd.org/) (Recommended: v1.5.8)
 - [Gameboy Tile Designer](http://www.devrs.com/gb/hmgd/gbtd.html) (Recommended: v2.2)
 - [Gameboy Map Builder](http://www.devrs.com/gb/hmgd/gbmb.html) (Recommended: v1.8)
 - [EZFlashJr Kernal](http://www.ezflash.cn/product/ezflash-junior/) (Recommended: Firmware v4 K1.04e)
 - [GBT-Player](https://github.com/gingemonster/GamingMonstersGameBoySampleCode/tree/master/19_making_music) (Recommnded: Modified version found in gingemonster's tutorial repository)
 - [OpenMPT](https://openmpt.org/) (Recommnded: v1.29)

# Builders
This game can be built with either using MakeFile or using CMakeLists, both are provided.  
To use CMakeLists.txt files you will need to install CMake [link](https://cmake.org/install/).  
To use MakeFile's on Linux you will need install build-essential:
``` bash
sudo apt-get install build-essential
```
To use MakeFile's on Windows you will need to install a GNU port like MinGW [link](https://www.ics.uci.edu/~pattis/common/handouts/mingweclipse/mingw.html) (this repository assumes using MinGW).

# Build
## Add GBDK compiler to path
To build with cmake or makefiles add GBDK's compiler (lcc) to the environment variable.  
In linux set the path variable using the following command:
``` bash
export PATH=PATH_TO_GBDK/gbdk-3.2/bin:$PATH
```
In windows use following instructions:
 - In Search, search for and then select: System (Control Panel)
 - Click the Advanced system settings link.
 - Click Environment Variables. In the section System Variables, find the PATH environment variable and select it. Click Edit. If the PATH environment variable does not exist, click New.
 - In the Edit System Variable (or New System Variable) window, add the directory to the bin folder of GBDK to the list. Click OK. Close all remaining windows by clicking OK.

## Generate music
Music is generated using [OpenMPT](https://openmpt.org/) [Tutorial](https://www.youtube.com/watch?v=4W7j3GQSmvU)  
This creates a .mod file which can be used with 'mod2gbt' a tool from [gbt_player](https://github.com/AntonioND/gbt-player) to create files that can be compiled with gbdk. Offically this is no longer supported and the latest version of the repository does not work with gbdk. Instead use files from gingemonster's tutorial [here](https://github.com/gingemonster/GamingMonstersGameBoySampleCode/tree/master/19_making_music).

You should end up with 'gbt_player.h', 'gbt_player.s', 'gbt_player_bank.s'.  
You need to generate a '.c' file from the the '.mod' file using the mod2gbt tool:
```
mod2gbt spaceCalm.mod spaceSong -c 2
```
This will create an 'output.c' file. Copy this into the src folder to use in the game build. 

## Make
*WARNING: make.bat currently only way to build due to complete build of adding music*
There are three approaches to build games with GBDK; make, cmake, or directly calling the GBDK compiler.  
Each of the examples in this repository have a MakeFile and CMakeLists.txt that can be used to build them whether you are using make or cmake. Hopefull this will be useful when you build your own games to show what's needed.  
*Note: Expects the GBDK bin directory to be in the PATH environment variable so that lcc is accessible.*

### MakeFile
To build on Linux use the following commands:
``` bash
cd PATH_TO_REPO
make
```
To build on Windows MinGW is recommeded (see [here](https://www.ics.uci.edu/~pattis/common/handouts/mingweclipse/mingw.html) for install instructions):
```
cd PATH_TO_REPO
mingw32-make -f MakeFile
```
These will build the example game in a folder named 'build'.  
The '.gb' file in this build folder is the gameboy game that has just been compiled. 

### CMake
To build on Linux use the following commands:
``` bash
cd PATH_TO_REPO
mkdir build
cd build
cmake ..
make
```
To build on Windows use the following commands:
```
cd PATH_TO_REPO
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```
These will build the example game in the folder named 'build'.  
The '.gb' file in this build folder is the gameboy game that has just been compiled. 

### Compiler
To build on Linux use:
``` bash
cd PATH_TO_REPO
mkdir build
cd build
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o space.gb ../src/main.c
```
To build on Windows use:
```
cd PATH_TO_REPO
mkdir build
cd build
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o space.gb ..\src\main.c ..\src\SpaceBgMap.c ..\src\SpaceBgTiles.c ..\src\SpaceShipTiles.c
```
This will build the app to a build folder in this repository.  
The '.gb' file in this build folder is the gameboy game that has just been compiled. 

# Run
## Emulator
You can use the .gb file generated on a gameboy cartridge however it's advised to test it out in an emulator. 

## Local
BGB is a GameBoy emulator.

Run the BGB emulator, right-click and select 'Load ROM...', and choose the '.gb' file you generated.  
Or start the emulator from the command line specifying the '.gb' file to run with the '-rom' argument:
```
PATH_TO_BGB\bgb.exe -rom PATH_TO_REPO\build\space.gb
```

## Online
[Googulator](https://www.googulator.com) is an online GameBoy emulator that runs in the browser. It relies on a Google drive for loading game files so you will need a Google account.  

Sign in and link your Google drive account. Then go to the library tab, click 'Add games' and select 

## EZ Flash Jr
You can flash the .gb file generated to a gameboy cartridge. This is easiest with EZ Flash Jr [link](http://www.ezflash.cn/product/ezflash-junior/).  
This has a removable micro-SD slot to easily load game files. 

### Prepare the micro-SD card
EZ Flash recommends using the SanDisk Ultra 32 GB microSDHC which MUST be formated FAT32, cluster size 32KB.  
In Windows do this by right clicking the SD card once inserted and selecting 'Format'. Select the correct format options in the dialog and then click 'Start'.  
Now the SD is ready, copy the files in '3rdparty/EZFlashJr-FW4-K1.04e' onto the SD card.  

### Update EZ Flash Firmware
The files in this repository use Firmware v4 K1.04e. This should be the latest firmware from EZ Flash. Check for updates [here](http://www.ezflash.cn/product/ezflash-junior/).  
To update the firmware copy the 'Update_FWX.gb' and 'ezgb.bat' file from the latest firmware onto the SD card. (Where X is the firmware version)
Remove the SD card and insert it into the cartridge.  
Turn on the game boy which should load the EZ startup menu. Select 'Update_FWX.bg' from the menu.  
The screen may flicker in the upgrade progress, it is normal. The progress will cost 10+ seconds. The screen goes normal when upgrade is done. Power off your console as prompted. **WARNING: DO NOT press the reset button!**  
Power up your console. Check the FW version in the HELP tab by press SELECT, if the FW version displaying FW0, you have to do the update progress again until the FW version change to FW4.  Full charged batteries will help to prevent the upgrade failing.
You can delete the 'Update_FW[X].gb' file from the SD card once you have tested it worked. **Note: Do not delete the ezgb.dat file**

### Load game
Copy the .gb file onto the SD card. Remove the SD card and insert it into the EZ Flash Jr cartridge.  
Select the game from the EZ Flash menu. The gameboy will restart and load the game.  

# Notes
## Useful links
**Programming Game Boy Games using GBDK ([link](https://videlais.com/2016/07/03/programming-game-boy-games-using-gbdk-part-1-configuring-programming-and-compiling/)):** Tips for programming and advice on the hardware  
**GBDK Playground ([link](https://github.com/mrombout/gbdk_playground)):** Game code examples

## Programming tips
 - Objects, as they might exist in other languages, aren’t in C. And you can’t use structs to mirror this functionality, either. Most of your code will be based on functions and arrays.
 - Whenever possible, use globals, too. Instead of defining a variable within a function, define it once and then, if possible after, re-use that same name as a way to cut down on calls to the stack.
 - Instead of using “int”, use “UINT8” at all times.
 - Other than in a few specific cases, most values will be in hexadecimal.

# Future improvements
 - [ ] Test Linux support (written but not tested)
 - [ ] Add Linux instructions for emulation
