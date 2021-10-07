# Slimmer

**A user interface controller software for Logitech Media Server.**

Basically it's a hub between LMS, LCDProc and standard linux keyboard events. For more information visit the [Slimmer wiki](https://github.com/terba/slimmer/wiki).

![Front](https://github.com/terba/slimmer/wiki/images/front.jpg)

## Quickstart

You can download binary packages for the [piCorePlayer image](https://sites.google.com/site/picoreplayer/home) running on a Raspberry PI from here. See [this wiki page](https://github.com/terba/slimmer/wiki/Building-a-player) for instructions.

## How to build

### Requirements

* a C++ build environment supporting C++11
* CMake 3.1.0+
* [lcdapi library with development files (headers)](https://github.com/spdawson/lcdapi)
* curl library with development files (headers)
* ICU library with development files (headers)

### Basic build process

```
git clone https://github.com/terba/slimmer.git
mkdir slimmer-build
cd slimmer-build
cmake -DCMAKE_BUILD_TYPE=Release ../slimmer
make
```

For embedded systems use the following cmake command to optimize for size:
```
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ../slimmer
```

After a successful build you can find the executable named *slimmer* in the build directory.

For more information visit the [For developers wiki page](https://github.com/terba/slimmer/wiki/For-developers).

## Usage

You can configure Slimmer through command line arguments. For general usage see the [User manual](https://github.com/terba/slimmer/wiki/User-manual).

```
Usage:
   ./slimmer  [-v] [-s <ip or hostname>] [-p <number>] [-l <ip or
              hostname>] [-P <number>] [-m <AA:BB:CC:DD:EE:FF>] [-f] [-o
              <0-100>] [-i <input device file>] [-e <single-byte encoding>]
              [-c <0-10>] [--] [--version] [-h]

Where: 
   -v,  --verbose
     be verbose

   -s <ip or hostname>,  --lmshost <ip or hostname>
     LMS host (default: autodiscovery)

   -p <number>,  --lmsport <number>
     LMS HTTP port (default: autodiscovery)

   -l <ip or hostname>,  --lcdhost <ip or hostname>
     lcdproc host (default: localhost)

   -P <number>,  --lcdport <number>
     lcdproc port (default: 13666)

   -m <AA:BB:CC:DD:EE:FF>,  --mac <AA:BB:CC:DD:EE:FF>
     the player's MAC address (default: automatic, first interface)

   -f,  --fixedvolume
     volume control disabled

   -o <0-100>,  --volume <0-100>
     set volume on startup

   -i <input device file>,  --input <input device file>
     keyboard input device file(s). Use comma to delimit multiple input files. (default: /dev/input/event0)

   -e <single-byte encoding>,  --encoding <single-byte encoding>
     the LCD's character encoding (default: ISO-8859-1)

   -c <0-10>,  --scrollspeed <0-10>
     text scrolling speed (default: 3)

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.


   Copyright (C) 2016-2017  Terényi, Balázs (terenyi@freemail.hu)

   This program comes with ABSOLUTELY NO WARRANTY. This is free software,
   and you are welcome to redistribute it under certain conditions. See
   LINCENSE file in the source distribution.
```

### Exiting

To exit from Slimmer press `Ctrl+c`.

### Run as a daemon

To run Slimmer as a daemon you can use and adapt [the init script](https://github.com/terba/slimmer/blob/master/scripts/slimmer) located in the *scripts* folder of the sources. It is meant to be used on Tiny Core style distributions (piCore, piCorePlayer), so it doesn't contain LSB headers used on more sophisticated distros.
