## ofxQNX ##

### BlackBerry PlayBook and BlackBerry 10 add-on for openFrameworks ###

Project page: [http://www.multigesture.net/projects/ofxqnx/](http://www.multigesture.net/projects/ofxqnx/)

Repository: [https://github.com/falcon4ever/ofxQNX](https://github.com/falcon4ever/ofxQNX)

Copyright (C) 2012-2015 Laurence Muller

### Contact ###

Website: [http://www.multigesture.net](http://www.multigesture.net)

## LICENSE ##

New BSD License (3-clause license)

- see [LICENSE.txt](http://https://raw.github.com/falcon4ever/ofxQNX/develop/LICENSE.txt)

## REQUIREMENTS ##

A patched openFrameworks until my patches are merged into the official develop branch. For now it is recommended to use **openframeworks-v0.7.4-stable**.

- git clone -b openframeworks-v0.7.4-stable https://github.com/falcon4ever/openFrameworks.git

The current version is build using the following Native SDKs:

### BlackBerry PlayBook ###

To compile the project you will need to install the BlackBerry Native SDK ( **2.1.0-1032** ):

- [http://developer.blackberry.com/native/download/#playbook](http://developer.blackberry.com/native/download/#playbook)

### BlackBerry 10 ###

To compile the project you will need to install the BlackBerry 10 Native SDK ( **10.3.1.995** ):

- [http://developer.blackberry.com/native/download/#blackberry10](http://developer.blackberry.com/native/download/#blackberry10)

## QUICK START ##
Download (a patched) openFrameworks from the repository that is listed above. Next download this add-on and place it in the addons directory.

### Workspace setup ###

Copy the example projects from

- "openFrameworks\addons\ofxQNX\examples\BB10" or "openFrameworks\addons\ofxQNX\examples\PlayBook"
 
to

- "openFrameworks\examples\qnx"

Launch the IDE provided by the BlackBerry Native SDK and create a new workspace at:

- "openFrameworks\examples\qnx"

After the IDE is running, import the following projects: 

- "openFrameworks\libs"
- "openFrameworks\addons"

And all examples from

- "openFrameworks\examples\qnx"

### Build and run ###

If you installed both the PlayBook and BB10 Native SDK, make sure your workspace has been set to the right platform. 

To check

1. Project > Properties > QNX
2. Make sure that the SDK Selection is showing the correct platform

Build and Run

1. Click any of the example projects on the left
2. Right click, and click on Build Project
3. After it is done, right click on the project and choose Run As > BlackBerry [platform name] Application


## BUILDING THE DEPENDENCIES (from scratch) ##

Since it is a bit complicated to build the required dependencies, I have included binaries that are needed to build the openFrameworks "emptyExample" and placed them in "ofxQNX\libs" (For Device-Release (ARMv7), Device-Debug (ARMv7) and Simulator-Debug (X86)).

If you still want to build these dependencies yourself (or update the libs), check out the scripts in the bootstrap directory. 

- Copy **paths.make.default** to **paths.make** and fill out the paths to the SDKs
- The shell scripts in this directory will download and build the required libs.

### Windows / Linux / Mac? ###

While I am only testing this port on Windows, its easier to compile the dependencies for the QNX platform on a Linux machine. I recommend using a virtual machine (e.g. VirtualBox) with Ubuntu installed on it (and using the Linux version of the Native SDK).

### Setup the build environment ###

Boost

- [http://www.boost.org/](http://www.boost.org/)
- Included version is 1.52.0
- Required because `std::tr1::shared_ptr` and `std::tr1::weak_ptr` are not available from the Native SDK (`#include <boost/tr1/memory.hpp>`)
- The included version has been stripped down for use with openFrameworks.

FreeImage 
 
- [http://freeimage.sourceforge.net/](http://freeimage.sourceforge.net/)
- Using version 3.14.1 and removed support for EXR to make it compile.
- Modify the makefiles (MKDIR and CP command) if you're not using MinGW but Linux or Mac
- Please note that this version of FreeImage is bundled with libpng14 (BB10 Native SDK 10.2.x+ now ships with both libpng14 and libpng16 (default))

GLU

- Since no GLU is included with the Native SDK I am using the one from:
- [http://code.google.com/p/glues/](http://code.google.com/p/glues/)
- Included version is 1.3

Tess 2

- I am using the source files from "openFrameworks\libs\tess2\Sources"
- In **tesselator.h** you will need to add: `|| defined(__QNX__)` to make sure it will select: `typedef unsigned short TESSindex;`

Poco

- [http://pocoproject.org/](http://pocoproject.org/)
- Version 1.4.3p1
- This can only be build on a Linux or Mac machine. Windows is not supported.
- Included a dummy syslog.h implementation (borrowed from the Android NDK) as it will not build without.
- Uses the included header files from openFrameworks 0.7.4

SDL

- Add `__PLAYBOOK__` to your project if you use Audio/SDL
- [https://github.com/blackberry/SDL](https://github.com/blackberry/SDL)

TouchControlOverlay

- [https://github.com/blackberry/TouchControlOverlay](https://github.com/blackberry/TouchControlOverlay)

Ogg/Vorbis

- [https://github.com/blackberry/OggVorbis](https://github.com/blackberry/OggVorbis)
- Build Vorbis with `-fPIC`

Additional information can be found here:

- [https://developer.blackberry.com/native/documentation/bb10/porting.html](https://developer.blackberry.com/native/documentation/bb10/porting.html)
- [http://forum.openframeworks.cc/index.php/topic,9189.msg42786.html#msg42786](http://forum.openframeworks.cc/index.php/topic,9189.msg42786.html#msg42786)
- [http://forum.openframeworks.cc/index.php/topic,10726.0.html](http://forum.openframeworks.cc/index.php/topic,10726.0.html)

## NEWS ##

05/24/2015
- Updated libraries for **BB10 Native SDK 10.3.1.995**
- Using libpng14 (make sure you're using libpng14 as the default is libpng16 now).

04/04/2013

- Downgraded poco 1.4.6 to poco-1.4.3p1 to be compatible with the included modified poco headers in openFrameworks 0.7.4 ( [8a59e327](https://github.com/openframeworks/openFrameworks/commit/8a59e327842eaddeaf1e3279b0e2c944ae1c4a23) )
- Updated PlayBook examples
- Updated BB10 examples
- Requires the latest **openframeworks-v0.7.4-stable**
 
04/03/2013
 
- Updated BB10 examples
- Tested with **openframeworks-v0.7.4-stable**

04/02/2013

- Updated libraries for **BB10 Native SDK 10.0.10.738**

02/15/2013

- Updated path behaviour (requires the latest **develop-0071-qnx**)
- Please consult the FAQ on how to adjust the paths.

01/24/2013

- Fixed libvorbis
- Added (auto) rotation support
- Updated qnxSoundPlayerExample

01/24/2013

- Added SoundPlayer implementation
- Stream is implemented using the Multimedia Renderer
- List of supported media types: [supported_media](https://developer.blackberry.com/devzone/develop/supported_media/bb_media_support_at_a_glance.html#kba1328545526665)
- Non-stream can only play Wav or Ogg

01/23/2013

- Added Ogg/Vorbis libs + SoundPlayer placeholders
- Added log support (update develop-0071-qnx)

01/22/2013

- Updated libraries for **PlayBook Native SDK 2.1.0.1032** and **BB10 Native SDK 10.0.9.2318**
- Upgraded POCO from 1.4.3p1 to 1.4.6 (use `-DPOCO_NO_GCC_ATOMICS` in PlayBook projects)
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

- Updated BB10 target ( **BlackBerry 10 Native SDK 10.0.09 gold** )

11/03/2012

- Updated BB10 example projects

11/02/2012

- Updated BB10 target ( **BlackBerry 10 Native SDK 10.0.09 beta 3** )

09/11/2012

- Updated ofxQNX for **openFrameworks 0071**
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

30/03/2012 

- First beta version

## FAQ ##

**Is ofxQNX stable enough for prime time?**

- Yes, Definitely! It's currently being used in [NodeBeat](www.nodebeat.com) and [Super Hexagon](http://superhexagon.com/).

**Where do I store my files?**

Private assets that are read only

- Example: You've added a "data" folder as asset with "readonly.jpg" in it:
- ofToDataPath("app/native/data/readonly.jpg");

Private data is read/write from app (sandboxed): 

- Example: settings.dat
- ofToDataPath("data/settings.dat");

Public data (requires `<permission>access_shared</permission>` in the bar descriptor file)

- Example: screenshot.jpg
- ofToDataPath("shared/camera/screenshot.jpg");

More info: 

- [accessible_folders](http://developer.blackberry.com/native/documentation/bb10/com.qnx.doc.native_sdk.devguide/com.qnx.doc.native_sdk.devguide/topic/accessible_folders.html)

## TODO ##

- Update FreeImage and make the project use libpng16 instead of libpng14
- Add Camera support for BB10.
- Add GPS support and example.
- Clean up ofxQNXSoundPlayer and qnxAudioOutputExample class.
- Remove SDL dependency (only used for ofSoundStream / qnxAudioOutputExample) and use the QSA audio system directly.
- Figure out why loading OGG files is painfully slow.

## BUGS? ##

- Please report bugs and submit fixes on github ;)

## Examples ##

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


## PROJECT SETTINGS ##

These are already set in the example projects. In addition to these settings you will also need to include the SDL and TouchControlOverlay shared libraries in the bar descriptor file.

Defines (-D)

- `_FORTIFY_SOURCE=2`
- `USING_GL11`
- `TARGET_QNX`
- `TARGET_OPENGLES`
- `TARGET_LITTLE_ENDIAN`
- `__PLAYBOOK__`

Libraries (-l)

- m
- bps
- screen
- mmrndclient
- strm
- EGL
- GLESv1_CM
- freetype
- png14
- socket
- PocoNet
- PocoUtil
- PocoXML
- PocoFoundation
- freeimage
- tess2
- GLU
- asound
- OpenAL
- alut
- vorbis
- ogg
