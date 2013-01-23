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
	cd "$BB_BOOTSTRAP_ROOT/bootstrap/ogg"	
	
	# Build libs
	${QNX_HOST}/usr/bin/make clean
	${QNX_HOST}/usr/bin/make

	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/ogg/include/ogg"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/X86/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/X86/Debug"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/ARMv7/Release"
	mkdir -p "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/ARMv7/Debug"

	cp ogg/ogg.h "$BB_BOOTSTRAP_ROOT/libs/ogg/include/ogg"
	cp ogg/os_types.h "$BB_BOOTSTRAP_ROOT/libs/ogg/include/ogg"
	cp x86/a/libogg.a "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/X86/Release"
	cp x86/a-g/libogg.a "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/X86/Debug"
	cp arm/a.le-v7/libogg.a "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/ARMv7/Release"
	cp arm/a.le-v7-g/libogg.a "$BB_BOOTSTRAP_ROOT/libs/ogg/libPlayBook/ARMv7/Debug"	
fi
