#!/bin/bash 

# Get NDK_ROOT
source paths.make

# Bootstrap folder
BB_BOOTSTRAP_ROOT=$(pwd)
BB_BOOTSTRAP_ROOT=${BB_BOOTSTRAP_ROOT%/*}

# Build BB10 platform
if [ -d "$BBNDK_BB10_ROOT" ]; then

	# Get build environment	
	source "$BBNDK_BB10_ROOT/bbndk-env_10_3_1_995.sh"	
			
	# Build libs
	cd "$BB_BOOTSTRAP_ROOT/bootstrap/GLU"
	${QNX_HOST}/usr/bin/make clean
	${QNX_HOST}/usr/bin/make
	
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/include"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/X86/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/X86/Debug"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/ARMv7/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/ARMv7/Debug"

	cp glues.h "$BB_BOOTSTRAP_ROOT/libs/glu/include"
	cp x86/a/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/X86/Release"
	cp x86/a-g/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/X86/Debug"
	cp arm/a.le-v7/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/ARMv7/Release"
	cp arm/a.le-v7-g/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libBB10/ARMv7/Debug"
fi
