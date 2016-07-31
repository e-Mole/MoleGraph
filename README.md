# MoleGraph
School measuring system based on Arduino

## Desktop app for Mac OS X

### Building
Open Qt Creator project SerialToGraph/SerialToGraph.pro. Set release build Project->Build Settings->Realease and Build Project. Build install image file by run Qt util macdeployqt, see below. This step includes frameworks and libraries in the application.

`macdeployqt <path to MoleGraph.app> -verbose=2 -dmg`

### Installation
Double click on MoleGraph.dmg to mount image file and than install it by copying MoleGraph.app to your Application folder.
