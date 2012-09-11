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
	
	# Copy source code
	cd "$BB_BOOTSTRAP_ROOT/bootstrap/tess2"	
	cp ../../../../libs/tess2/Sources/* .
	cp ../../../../libs/tess2/include/tesselator.h .

	# Build libs
	${QNX_HOST}/usr/bin/make clean
	${QNX_HOST}/usr/bin/make
	
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/tess2/include"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/X86/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/X86/Debug"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/ARMv7/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/ARMv7/Debug"

	cp tesselator.h "$BB_BOOTSTRAP_ROOT/libs/tess2/include"
	cp x86/a/libtess2.a "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/X86/Release"
	cp x86/a-g/libtess2.a "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/X86/Debug"
	cp arm/a.le-v7/libtess2.a "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/ARMv7/Release"
	cp arm/a.le-v7-g/libtess2.a "$BB_BOOTSTRAP_ROOT/libs/tess2/libPlayBook/ARMv7/Debug"
fi
