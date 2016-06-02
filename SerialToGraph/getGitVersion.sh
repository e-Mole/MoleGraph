#!/bin/bash
cd $1
filename=GitVersion.h
echo '//generated file - do not edit' > $filename
echo '#ifndef VERSION_H' >> $filename
echo '#define VERSION_H' >> $filename
version=$(git describe --always --tags --dirty)
echo '#	define GIT_VERSION "'$version'"' >> $filename
echo '#endif //VERSION_H' >> $filename
echo $version
