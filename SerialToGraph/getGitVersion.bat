:;#in this file is bash and batch code together because I want to be able build android version of the MoleGraph on linux as same on windows 
:;cd $1
:;filename=GitVersion.h
:;echo '//generated file - do not edit' > $filename
:;echo '#ifndef VERSION_H' >> $filename
:;echo '#define VERSION_H' >> $filename
:;version=$(git describe --always --tags --dirty)
:;echo '#	define GIT_VERSION "'$version'"' >> $filename
:;echo '#endif //VERSION_H' >> $filename
:;echo git version: $version
:;exit

@echo off
cd %1
set filename= GitVersion.h
echo #ifndef VERSION_H > %filename%
echo #define VERSION_H >> %filename%
for /f "delims=" %%i in ('git describe --always --tags --dirty') do set version=%%i
echo #	define GIT_VERSION "%version%" >> %filename%
echo #endif //VERSION_H >> %filename%
echo git version: %version% 