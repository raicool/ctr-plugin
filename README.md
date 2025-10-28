# ctr-plugin
[![Release](https://badgen.net/github/release/raicool/ctr-plugin)](https://github.com/raicool/ctr-plugin/releases)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/raicool/ctr-plugin/graphs/commit-activity)
[![GitHub last commit](https://badgen.net/github/last-commit/raicool/ctr-plugin/main)](https://github.com/raicool/ctr-plugin/commits/main)

Basic Mario Kart 7 plugin that citra supports

![mpv-shot0019](https://i.imgur.com/iFsXWDD.jpeg)

# Installing to Citra
1. move ctr-plugin.3gx into the path depending on your game's region below
- usa: `%appdata%/citra/sdmc/luma/plugins/0004000000030800`
- europe: `%appdata%/citra/sdmc/luma/plugins/0004000000030700`
- japan : `%appdata%/citra/sdmc/luma/plugins/0004000000030600`
2. enable plugin loading within citra at `emulation -> configure -> system -> enable 3gx plugin loader`
- if the option isnt available, update to the newest version of citra at https://github.com/PabloMK7/citra

# Installing to BizHawk (Recommended)
1. ~~if you dont already have it, install the devbuild version of bizhawk from (https://gitlab.com/TASVideos/BizHawk/-/artifacts), as normal builds of bizhawk do not have 3ds support~~

   (bizhawk's 3ds core has been moved to release builds as well!! download it from here instead https://github.com/TASEmulators/BizHawk/releases/tag/2.10)
2. move ctr-plugin.3gx into the path depending on your game's region below (replace [bizhawk directory] with the location of your bizhawk installation of course)
- usa: `[bizhawk directory]/3DS/User/sdmc/luma/plugins/0004000000030800`
- europe: `[bizhawk directory]/3DS/User/sdmc/luma/plugins/0004000000030700`
- japan : `[bizhawk directory]/3DS/User/sdmc/luma/plugins/0004000000030600`
3. launch mario kart 7, and then enable plugin loading at `3DS -> Settings -> Sync Settings -> Enable 3GX Plugin Loader`
4. reboot the game

# Building
## Prerequisites 
  - This plugin requires the most recent version of CTRPluginFramework installed with devkitPro (https://gitlab.com/thepixellizeross/ctrpluginframework/)
    - see their readme for how to install it or smth
  - Citro3D is also required to be installed (https://github.com/devkitPro/citro3d)
## Windows
  - launch devkitPro's msys2 shell from devkitPro/msys2/msys2_shell.bat
  - set directory to root of ctr-plugin
  - run `sh build` with some optional parameters:
    - `clean`: deletes CMakeFiles folder
    - `nobuild`: do not run cmake generation or make
    - `copy`: copies bin/plugin.3gx to citra (environment var CITRA_DATA must be set to citra's appdata folder, otherwise you must supply the path)
    - `run-headless`: runs mariokart7 from citra.exe (environment var CITRA must be set to citra.exe's path, otherwise you must supply the path)
    - `run-qt`: runs mariokart7 from citra-qt.exe (environment var CITRA_QT must be set to citra-qt.exe's path, otherwise you must supply the path)
	
## Linux
  - idk how this os works
