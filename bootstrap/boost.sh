#!/bin/bash

# Get NDK_ROOT
source paths.make

# Bootstrap folder
BB_BOOTSTRAP_ROOT=$(pwd)
BB_BOOTSTRAP_ROOT=${BB_BOOTSTRAP_ROOT%/*}

# Boost
BB_BOOST_URL=http://sourceforge.net/projects/boost/files/boost/1.50.0/boost_1_50_0.tar.gz
BB_BOOST_FILE=boost_1_50_0.tar.gz
BB_BOOST_FOLDER=boost_1_50_0

# Check for is boost is already installed
if [ -d "$BB_BOOTSTRAP_ROOT/libs/boost/include/boost" ];
then
    echo "Found Boost in: $BB_BOOTSTRAP_ROOT/libs/boost/include/boost";
	return
fi

# Create download folder
mkdir -p "$BB_BOOTSTRAP_ROOT/bootstrap/downloads"
cd "$BB_BOOTSTRAP_ROOT/bootstrap/downloads"

# Download and unpack Boost
if [ ! -f $BB_BOOST_FILE ];
then
    wget $BB_BOOST_URL
fi

# Create a folder for Boost
mkdir -p "$BB_BOOTSTRAP_ROOT/libs/boost/include"

# Extract the Boost folder (Because the package is very large, only extract the header files)
echo "Extracting header files from $BB_BOOST_FILE ... this might take a while";

unamestr=`uname`
if [ "$unamestr" == 'Darwin' ]; then
    gunzip -c $BB_BOOST_FILE | tar -C "$BB_BOOTSTRAP_ROOT/libs/boost/include" -xvf - "$BB_BOOST_FOLDER/boost"
    mv "$BB_BOOTSTRAP_ROOT/libs/boost/include/$BB_BOOST_FOLDER/boost" "$BB_BOOTSTRAP_ROOT/libs/boost/include/"
    rm -Rf "$BB_BOOTSTRAP_ROOT/libs/boost/include/$BB_BOOST_FOLDER"
else
    tar -C "$BB_BOOTSTRAP_ROOT/libs/boost/include" -xaf $BB_BOOST_FILE --strip=1 "$BB_BOOST_FOLDER/boost"
fi

echo "Done!";

# Clean up and return
cd "$BB_BOOTSTRAP_ROOT/bootstrap"

