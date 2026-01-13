# MoleGraph
School measuring system based on Arduino for MS Windows, Linux, Android and macOS (App available for download directly here in the Releases section: https://github.com/e-Mole/MoleGraph/releases). More info on MoleGraph project web https://www.molegraph.eu. For information about the Arduino MoleGraph shield, visit https://www.e-mole.cz/diy/molegraph-shield.

## Desktop app for Mac OS X building instructions

### QT version
QT version is 5.10.1 (QT Creator 4.5.1) downloaded from https://download.qt.io/new_archive/qt/5.10/5.10.1/.

### Building
Open Qt Creator project SerialToGraph/SerialToGraph.pro. Set release build Project->Build Settings->Realease and Build Project. Build install image file by run Qt util macdeployqt, see below. This step includes frameworks and libraries in the application.

`~/Qt5.10.1/5.10.1/clang_64/bin/macdeployqt <path to MoleGraph.app> -verbose=2 -dmg`

### Installation
Double click on MoleGraph.dmg to mount image file and than install it by copying MoleGraph.app to your Application folder.
