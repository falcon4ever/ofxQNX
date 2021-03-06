#!/bin/bash 

# Get NDK_ROOT
source paths.make

# Bootstrap folder
BB_BOOTSTRAP_ROOT=$(pwd)
BB_BOOTSTRAP_ROOT=${BB_BOOTSTRAP_ROOT%/*}

# FreeImage
BB_FREEIMAGE_URL=http://downloads.sourceforge.net/project/freeimage/Source%20Distribution/3.14.1/FreeImage3141.zip
BB_FREEIMAGE_FILE=FreeImage3141.zip
BB_FREEIMAGE_FOLDER=FreeImage

# Create download folder
mkdir -p "$BB_BOOTSTRAP_ROOT/bootstrap/downloads"
cd "$BB_BOOTSTRAP_ROOT/bootstrap/downloads"

# Build PlayBook platform
if [ -d "$BBNDK_PLAYBOOK_ROOT" ]; then

	# Download and unpack FreeImage 3.14.1
	if [ ! -f $BB_FREEIMAGE_FILE ];
	then
		wget $BB_FREEIMAGE_URL
	fi
	rm -Rf FreeImage
	unzip $BB_FREEIMAGE_FILE
	cd FreeImage

	# Patch FreeImage
	cp ../../FreeImage/Plugin.cpp Source/FreeImage/
	cp ../../FreeImage/Makefile.srcs .
	cp ../../FreeImage/Makefile.qnx .

	# Set export dir
	BB_FREEIMAGE_PREFIX="$BB_BOOTSTRAP_ROOT/libs/FreeImage"
	export BB_FREEIMAGE_PREFIX 
	
	# Get build environment
	source "$BBNDK_PLAYBOOK_ROOT/bbndk-env.sh"
	
	PLATFORM_POSTFIX="PlayBook"
	export PLATFORM_POSTFIX
	
	BB_TARGET="X86"
	export BB_TARGET
	
	cp ../../FreeImage/Makefile.qnx-playbook-sim .
	make -f Makefile.qnx-playbook-sim clean
	make -f Makefile.qnx-playbook-sim
	make -f Makefile.qnx-playbook-sim install
	
	BB_TARGET="ARMv7"
	export BB_TARGET
	
	cp ../../FreeImage/Makefile.qnx-playbook-device .
	make -f Makefile.qnx-playbook-device clean
	make -f Makefile.qnx-playbook-device
	make -f Makefile.qnx-playbook-device install

fi
