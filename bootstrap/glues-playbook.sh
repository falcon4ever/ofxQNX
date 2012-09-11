#!/bin/bash 

# Get NDK_ROOT
source paths.make

# Bootstrap folder
BB_BOOTSTRAP_ROOT=$(pwd)
BB_BOOTSTRAP_ROOT=${BB_BOOTSTRAP_ROOT%/*}

# Build PlayBook platform
if [ -d "$BBNDK_PLAYBOOK_ROOT" ]; then

	# Get build environment
	source "$BBNDK_PLAYBOOK_ROOT/bbndk-env.sh"
	
	# Build libs
	cd "$BB_BOOTSTRAP_ROOT/bootstrap/GLU"
	${QNX_HOST}/usr/bin/make clean
	${QNX_HOST}/usr/bin/make
	
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/include"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/X86/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/X86/Debug"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/ARMv7/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/ARMv7/Debug"

	cp glues.h "$BB_BOOTSTRAP_ROOT/libs/glu/include"
	cp x86/a/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/X86/Release"
	cp x86/a-g/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/X86/Debug"
	cp arm/a.le-v7/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/ARMv7/Release"
	cp arm/a.le-v7-g/libGLU.a "$BB_BOOTSTRAP_ROOT/libs/glu/libPlayBook/ARMv7/Debug"
fi
