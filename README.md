# Slimmer

**A user interface controller program for Logitech Media Server.**

Basically it's a hub between LMS, LCDProc and standard linux keyboard events. For more information visit the Slimmer wiki.

## Quickstart

For the [PiCorePlayer image](https://sites.google.com/site/picoreplayer/home) running on a Raspberry PI you can download binary packages from here. See the this wiki page for instructions.

## Build

### Requirements

* a C++ build environment supporting C++11
* CMake 3.1.0+
* lcdapi library with development files (headers)
* curl library with development files (headers)
* ICU library with development files (headers)

### Basic build process

```
git clone https://github.com/terba/slimmer.git
mkdir slimmer-build
cd slimmer-build
cmake ../slimmer
make
```

After a successfull build you get the executable named *slimmer* in the build directory.

## Usage

You can configure Slimmer through command line arguments:

```
Usage:
   ./slimmer  [-v] [-s <ip or hostname>] [-p <number>] [-l <ip or
              hostname>] [-P <number>] [-m <AA:BB:CC:DD:EE:FF>] [-f] [-o
              <0-100>] [-i <input device file>] [-e <single-byte encoding>]
              [--] [--version] [-h]

Where: 
   -v,  --verbose
     be verbose

   -s <ip or hostname>,  --lmshost <ip or hostname>
     LMS host (default: localhost)

   -p <number>,  --lmsport <number>
     LMS port (default: 9000)

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
     keyboard input device file (default: /dev/input/event0)

   -e <single-byte encoding>,  --encoding <single-byte encoding>
     the LCD's character encoding (default: ISO-8859-1)

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.


   Copyright (C) 2016  Terényi, Balázs (terenyi@freemail.hu)

   This program comes with ABSOLUTELY NO WARRANTY. This is free software,
   and you are welcome to redistribute it under certain conditions. See
   LINCENSE file in the source distribution.
```

### Exiting

To exit from Slimmer press `Ctrl+c`.

### Run as a daemon

To run Slimmer as a daemon you can use and adapt the init script located in the script folder of the sources.
