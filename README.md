# ctr-plugin

Basic Mario Kart 7 plugin that citra supports

![mpv-shot0019](https://i.imgur.com/iFsXWDD.jpeg)

# installing to citra
1. move ctr-plugin.3gx into the path depending on your game's region below
- usa: `%appdata%/citra/sdmc/luma/plugins/0004000000030800`
- europe: `%appdata%/citra/sdmc/luma/plugins/0004000000030700`
- japan : `%appdata%/citra/sdmc/luma/plugins/0004000000030600`
2. enable plugin loading within citra at `emulation -> configure -> system -> enable 3gx plugin loader`
- if the option isnt available, update to the newest version of citra at https://github.com/PabloMK7/citra

# Building
## Prerequisites 
  - This plugin requires the most recent version of CTRPluginFramework installed with devkitPro (https://gitlab.com/thepixellizeross/ctrpluginframework/)
    - see their readme for how to install it or smth
## Windows
  - launch devkitPro's msys2 shell from devkitPro/msys2/msys2_shell.bat
  - set directory to root of ctr-plugin
  - run `sh build` with some optional parameters:
    - `clean`: deletes CMakeFiles folder
    - `nobuild`: do not run cmake generation or make
    - `copy`: copies bin/plugin.3gx to citra (environment var CITRA_DATA must be set to citra's appdata folder, otherwise you must supply the path)
    - `run-nogui`: runs mariokart7 from citra.exe (environment var CITRA must be set to citra.exe's path, otherwise you must supply the path)
    - `run-qt`: runs mariokart7 from citra.exe (environment var CITRA_QT must be set to citra-qt.exe's path, otherwise you must supply the path)
	
## Linux
  - idk how this os works
