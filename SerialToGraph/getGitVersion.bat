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

for /f "delims=" %%i in ('git describe --always --tags --dirty') do set version=%%i
set filename= GitVersion.h

if not exist ".lock" (
echo.> ".lock"
del %filename%
echo #ifndef VERSION_H > %filename%
echo #define VERSION_H >> %filename% 
echo #define GIT_VERSION "%version%" >> %filename%
echo #endif //VERSION_H >> %filename%
echo git version: %version% 
 del ".lock"
) 