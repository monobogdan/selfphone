# Description
This is selfphone - advanced smartphone modding project, that attempts to (re)create custom firmware almost from scratch for various old smartphones.
It's working on pure Linux - no package managers, no Debian-like enviornment and also android libraries are almost not used.

The idea behind is simple - if we have custom recoveries, why we can't develop our custom lightweight "launcher" software for showing phone UI?
Right now, it can only dial, but there's kind of framework to work with various hardware of smartphones:

- Graphics subsystem with support of 16 & 24 bit framebuffers, semi-transparent TGA bitmaps and drawing some of primitives. Also it got not ready yet, but working animation system - simple interpolator.
- Input subsystem with support of single touch even on multitouch touchscreens. However, it's universal and can work with carious kinds with touch screen drivers.
- Basic modem driver. Right now, we're supporting SIM800 and basic AT command set. I've successfuly started MediaTek native modem, but unable to get it works in calls due to speech processing implemented in proprietary android libraries
- Very simple GUI
- Apps "framework"

# Which devices does it support?

Almost any smartphones from 2011-2013, produced on MediaTek & Spreadtrum chipsets. I've personally tested on MT6575/6577, but notice that for working modem, you need at RX?TX pins on your motherboard.
It can be easily ported to many devices.

# Compilation

Just fix NDK path in Makefile and here you go. If you're compiling for ICS+ firmware, add -fPIE to g++ flags, since android 4.0+ doesn't support nonn-fPIE binaries.

# Schematic

![https://i.imgur.com/tddkzFQ.png](https://i.imgur.com/tddkzFQ.png)
