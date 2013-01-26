ofxQNX
======

BlackBerry PlayBook and BlackBerry 10 add-on for openFrameworks

Project page:
- http://www.multigesture.net/projects/ofxqnx/

Repository: 
- https://github.com/falcon4ever/ofxQNX

Copyright (C) 2012-2013 Laurence Muller
E-mail: laurence.muller@gmail.com
Website: http://www.multigesture.net


LICENSE
=======

New BSD License (3-clause license)
- see LICENSE.txt


REQUIREMENTS
============

A patched openFrameworks until patches are merged into the official develop branch
- git@github.com:falcon4ever/openFrameworks.git (branch: develop-0071-qnx)
> git clone -b develop-0071-qnx https://github.com/falcon4ever/openFrameworks.git

The current version is build using the following Native SDKs:

BlackBerry PlayBook
To compile the project you will need to install the BlackBerry Native SDK (2.1.0-1032):
- http://developer.blackberry.com/native/download/#playbook

BlackBerry 10
To compile the project you will need to install the BlackBerry 10 Native SDK (10.0.9-2138):
- http://developer.blackberry.com/native/download/#blackberry10


QUICK START
===========

Download (a patched) openFrameworks from the link above. Next download this add-on and place it in the addons directory.

Launch the IDE provided by the BlackBerry Native SDK and follow the instructions below:

= PlayBook =

Open the IDE and create a new workspace at:
- "openFrameworks\examples\qnx"

After the IDE is running, import the following projects: 
- "openFrameworks\libs"
- "openFrameworks\addons"
- The example projects (BB10 or PlayBook) under "openFrameworks\addons\ofxQNX\examples\"

= BB10 =

Copy the example projects 
- Copy the examples from "openFrameworks\addons\ofxQNX\examples\BB10" to "openFrameworks\examples\qnx"

Open the IDE and create a new workspace at:
- "openFrameworks\examples\qnx"

After the IDE is running, import the following projects: 
- "openFrameworks\libs"
- "openFrameworks\addons"

And all examples from
- "openFrameworks\examples\qnx"

= Both =

If you installed both the PlayBook and BB10 Native SDK, make sure your workspace has been set to the right platform. 

To check: 
1. Project > Properties > QNX
2. Make sure that the SDK Selection is showing the correct platform

Build and Run
1. Click any of the example projects on the left
2. Right click, and click on Build Project
3. After it is done, right click on the binary (e.g. Device-Debug\qnxEmptyExample) and choose Run As > BlackBerry [platform name] Application


BUILDING THE DEPENDENCIES (from scratch)
========================================
Since it is a bit complicated to build the required dependencies, I have included binaries that are needed to build the openFrameworks "emptyExample" and placed them in "ofxQNX\libs" (For Device-Release, Device-Debug and Simulator-Debug).

If you still want to build these dependencies yourself (or update the libs), check out the scripts in the bootstrap directory. 
- Copy paths.make.default to paths.make and fill out the paths to the SDKs
- The shellscripts in this directory will download and build the require libs.

* Linux/Windows/Mac?
While I am only testing this port on Windows, its easier to compile the dependencies for the QNX platform on a Linux machine. I recommend using a virtual machine (e.g. VirtualBox) with Ubuntu installed on it (and using the Linux version of the Native SDK).

* Setup the build environment
https://bdsc.webapps.blackberry.com/native/documentation/porting_overview_1970891_11.html

* Boost 
- http://www.boost.org/
- Included version is 1.52.0
- Required because "std::tr1::shared_ptr" and "std::tr1::weak_ptr" are not available from the Native SDK (#include <boost/tr1/memory.hpp>)
- The included version has been stripped down for use with openFrameworks.

* FreeImage 
- http://freeimage.sourceforge.net/
- Using version 3.14.1 and removed support for EXR to make it compile.
- Modify the makefiles (MKDIR and CP command) if you're not using MinGW but Linux or Mac

* GLU
- Since no GLU is included with the Native SDK I am using the one from:
- http://code.google.com/p/glues/
- Included version is 1.3

* Tess 2
- I am using the source files from "openFrameworks\libs\tess2\Sources"
- In tesselator.h you will need to add: "|| defined(__QNX__)" to make sure it will select: "typedef unsigned short TESSindex;"

* Poco
- http://pocoproject.org/
- Version 1.4.6 (use -DPOCO_NO_GCC_ATOMICS in PlayBook projects)
- This can only be build on a Linux or Mac platform for some reason it will fail on Windows.
- Included a dummy syslog.h implementation (borrowed from the Android NDK) as it will not build without.

* SDL
- Add __PLAYBOOK__ to your project if you use Audio/SDL
- https://github.com/blackberry/SDL

* TouchControlOverlay
- https://github.com/blackberry/TouchControlOverlay

* Ogg/Vorbis
- https://github.com/blackberry/OggVorbis
- Build Vorbis with -fPIC

* Additional information can be found here:
- http://forum.openframeworks.cc/index.php/topic,9189.msg42786.html#msg42786
- http://forum.openframeworks.cc/index.php/topic,10726.0.html

NEWS
====

note: Examples still need to be updated. In the meantime you can use these hints: http://forum.openframeworks.cc/index.php/topic,10726.msg52228.html#msg52228
PlayBook -> Add -DPOCO_NO_GCC_ATOMICS

01/24/2013
- Fixed libvorbis
- Added (auto) rotation support
- Updated qnxSoundPlayerExample

01/24/2013
- Added SoundPlayer implementation
- Stream is implemented using the Multimedia Renderer
* List of supported media types: https://developer.blackberry.com/devzone/develop/supported_media/bb_media_support_at_a_glance.html#kba1328545526665
- Non-stream can only play Wav or Ogg

01/23/2013
- Added Ogg/Vorbis libs + SoundPlayer placeholders
- Added log support (update develop-0071-qnx)

01/22/2013
- Updated libraries for PlayBook Native SDK 2.1.0.1032 and BB10 Native SDK 10.0.9.2318
- Upgraded POCO from 1.4.3p1 to 1.4.6 (use -DPOCO_NO_GCC_ATOMICS in PlayBook projects)
- Upgraded Boost from  1.50.0 to 1.52.0

01/14/2013
- Cleaned up ofAppQNXWindow
- Changed bps event handler loop to pause properly
- Added callbacks for navigator events (Swipe down and Window states):
* virtual void navigatorSwipeDown()
* virtual void navigatorWindowState(int state)
* virtual void navigatorWindowActive()
* virtual void navigatorWindowInactive()
- Automatically enable touch (No need for ofRegisterTouchEvents(this) anymore)
- Added fixed time step option

12/12/2012
- Updated BB10 target (BlackBerry 10 Native SDK 10.0.09 gold)

11/03/2012
- Updated BB10 example projects

11/02/2012
- Updated BB10 target (BlackBerry 10 Native SDK 10.0.09 beta 3)

09/11/2012
- Updated ofxQNX for openFrameworks 0071
- Added buildscripts for dependencies (for both platforms)

05/05/2012
- Updated project files for PlayBook and BB10 target

04/05/2012
- Updated libraries for PlayBook and BB10 target
- Added instructions on how to build Poco and FreeImage for both platforms (including makefiles), check the Sources folder.

01/05/2012
- Updated code for screen dimensions
- Disabled keyboard gesture, you can still use the methods by calling the keyboard functions on-demand:
https://bdsc.webapps.blackberry.com/native/reference/com.qnx.doc.bps.lib_ref/com.qnx.doc.bps.lib_ref/topic/virtualkeyboard.h_functions_overview.html

26/04/2012
- Updated the instructions/readme and added a tutorial video.

26/04/2012
- Changed data dir.
- Private assets: ofToDataPath("", true) + "/app/native/assets/" (read-only)
- Private data: ofToDataPath("", true) + "/data/" (Contains the application's private data. The application has full access to read and write files in this folder.)
- Public data should be in: ofToDataPath("", true) + "/shared/XXX" (see link below for available subfolders)
-- More info: https://bdsc.webapps.blackberry.com/native/documentation/com.qnx.doc.ide.userguide/topic/files_base_.html

25/04/2012
- Implemented keyboard (use a downward swipe gesture from the top to open the on-screen keyboard)
- Updated qnxFontsExample example

10/04/2012
- ofxQNXSoundStream is now using SDL instead of QSA (Implemented by Seth). Seems to be providing low latency audio without distortions.

06/04/2012
- Added x86 binaries for use with the simulator
- Updated example project config files for the simulator (Set the project to Simulator-Debug)

04/04/2012
- Moved audio callback into its own thread (using ofThreads). Sound seems to be more stable, however its still noisy and seems to lag a bit.

02/04/2012
- Recompiled libtess2 with proper flags (this fixes the issues with qnxFontsExample and qnxPolygonExample)

01/04/2012
- Replaced freeimage with version 3.14.1 (This fixed the png issue with qnxImageLoaderExample)

31/03/2012
- Converted more examples to ofxQNX

30/03/2012 - beta
- Initial commit


FAQ
===
* Is ofxQNX stable enough for prime time?
- Yes, Definitely! It's currently used in NodeBeat (www.nodebeat.com) on the PlayBook and BB10 platforms

* Where do I store my files?
- Private assets: ofToDataPath("", true) + "/app/native/assets/" (read-only)
- Private data: ofToDataPath("", true) + "/data/" (Contains the application's private data. The application has full access to read and write files in this folder.)
- Public data should be in: ofToDataPath("", true) + "/shared/XXX" (see link below for available subfolders)
-- More info: https://bdsc.webapps.blackberry.com/native/documentation/com.qnx.doc.ide.userguide/topic/files_base_.html

* Camera access?
Currently the Native SDK doesnt provide access to the camera. More info about it here:
http://supportforums.blackberry.com/t5/Native-SDK-for-BlackBerry-Tablet/Camera-access/m-p/1398701
So unfortunately there wont be a qnxCameraExample or qnxOpenCVExample until then.


TODO
====
- Add GPS support/example
- Clean up ofxQNXSoundPlayer class and Example

BUGS?
=====
- Please report on github ;)

Examples
========
qnxAudioOutputExample 
- Runs ok
- Plays a generate waveform, touch makes modifies the waveform and position (Left/Right)

qnxEmptyExample
- Runs ok
- Use this project as a template 
- Open up .cproject, .project and bar-descriptor.xml and replace the text "qnxEmptyExample" with "yourprojectname"

qnxFontsExample
- Runs ok
- Example on how to use truetype fonts

qnxGraphicsExample 
- Runs ok 
- Basic graphics example

qnxImageLoaderExample 
- Runs ok
- Demonstrates how to use image resources in openFrameworks

qnxInputExample
- Runs ok 
- Demonstrates how to enable and use multitouch input in a project

qnxSoundPlayerExample
- Runs
- Demonstrates how to play audio files

qnxPolygonExample 
- Runs ok
- Demonstrates how to draw (an animated) polygon

qnxTouchExample 
- Runs ok
- Demonstrates how to use the accelerometer (and touch input)

qnxVBOExample
- Runs ok
- Demonstrates how to use VBO in openFrameworks


PROJECT SETTINGS
================
These are already set in the example projects.

* Defines (-D)
- _FORTIFY_SOURCE=2
- USING_GL11
- TARGET_QNX
- TARGET_OPENGLES
- TARGET_LITTLE_ENDIAN
- __PLAYBOOK__

* Libraries (-l)
- m
- bps
- screen
- EGL
- GLESv1_CM
- freetype
- png
- socket
- PocoNet
- PocoUtil
- PocoXML
- PocoFoundation
- freeimage
- tess2
- GLU
- asound
