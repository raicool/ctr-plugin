# ctr-plugin

Basic Mario Kart 7 plugin that citra supports

![mpv-shot0019](https://github.com/raicool/ctr-plugin/assets/56082069/dfc740be-eb74-41a2-a6e6-c3b073136423)

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
