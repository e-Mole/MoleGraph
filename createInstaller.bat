::script gethers .exe file dependencies and creates an installer
::prerequisities: 
::  * path to QtIFW bin directory placed in a system path
::  * script is run from a qt's mingw command line
::arguments:
::  as argument is expected a folder name where builded .exe file is
::installer is created in argument directory in subir 'installer'
 
set curDir=%cd%
rmdir %1\installer /q /s
mkdir %1\installer
xcopy /E Installer\* %1\installer 
cd %1\installer
copy ..\MoleGraph.exe packages\cz.emole\data\MoleGraph.exe
windeployqt --no-quick-import --no-translations --no-system-d3d-compiler --no-webkit2 --no-angle --no-opengl-sw packages\cz.emole\data\
binarycreator --offline-only -c config/config.xml -p packages MoleGraph-Installer

cd %curdir%

mkdir %1\installer\Arduino
mkdir %1\installer\Arduino\MoleGraph
copy Arduino\MoleGraph\keywords.txt %1\installer\Arduino\MoleGraph
copy Arduino\MoleGraph\MoleGraph.cpp %1\installer\Arduino\MoleGraph
copy Arduino\MoleGraph\MoleGraph.h %1\installer\Arduino\MoleGraph

mkdir %1\installer\Arduino\MoleGraph\examples
xcopy /S  Arduino\MoleGraph\examples  %1\installer\Arduino\MoleGraph\examples

mkdir %1\installer\Arduino\MoleGraph\examples\signalEmulation
copy Arduino\MoleGraph\MoleGraph.ino  %1\installer\Arduino\MoleGraph\examples\signalEmulation\signalEmulation.ino



