Fricando
========

Copyright (C) 2014-2015 Lemon Jia. All rights reserved.

For conditions of distribution and use, see [LICENSE](https://github.com/lemonjia/Fricando/blob/master/LICENSE).

For release notes, see [ReleaseNote.txt](https://github.com/lemonjia/Fricando/releases).

# Installation

## Windows XP, 7+
[32-bit Fs Explorer Install](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-windows-32bit-installer.exe)
[32-bit Fs Explorer Binary](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-windows-32bit-binary.zip)

[64-bit Fs Explorer Install](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-windows-64bit-installer.exe)
[64-bit Fs Explorer Binary](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-windows-64bit-binary.zip)

## Ubuntu 12.04+
[32-bit Fs Explorer Binary](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-ubuntu-32bit-binary.tar.bz2)
[64-bit Fs Explorer Binary](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer-ubuntu-64bit-binary.tar.bz2)

![Screenshot](https://github.com/lemonjia/Fricando-install/blob/master/screenshot-ubuntu.png)

# Source code

## Overview

[Fs Explorer](https://github.com/lemonjia/Fricando/blob/master/fs-explorer) - the GUI front-end of libyafuse2.

The source code of Fs Explorer includes several precompiled libraries:

[fs-explorer/lib32/unix](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib32/unix), [fs-explorer/lib64/unix](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib64/unix) - 32-bit/64-bit, precompiled by GCC from libyafuse2.

[fs-explorer/lib32/win](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib32/win) - 32-bit, precompiled by Visual Studio 2010 from libyafuse2.

## Building

The build of Fs Explorer requires Qt 5.2+.

```sh
cd fs-explorer
qmake
make && make install
```

# Authors

Lemon Jia, angersax@gmail.com
