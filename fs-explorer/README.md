Fricando
========

Copyright (C) 2013-2014 Lemon Jia. All rights reserved.

For conditions of distribution and use, see [LICENSE](https://github.com/lemonjia/Fricando/blob/master/LICENSE).

For release notes, see [ReleaseNote.txt](https://github.com/lemonjia/Fricando/releases).

# Installation

## Windows XP, 7+
[32-bit FS Explorer Install](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer/install32/win)

## Ubuntu 12.04+
[32-bit FS Explorer Install](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer/install32/unix)

[64-bit FS Explorer Install](https://github.com/lemonjia/Fricando-install/blob/master/fs-explorer/install64/unix)

![Screenshot](https://github.com/lemonjia/Fricando-install/blob/master/screenshot-ubuntu.png)

## Mac OS X 10.9+
ongoing...

# Source code

## Overview

[FS Explorer](https://github.com/lemonjia/Fricando/blob/master/fs-explorer) - the GUI front-end of libyafuse2.

The source code of FS Explorer contains several precompiled libraries:

[fs-explorer/lib32/unix](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib32/unix), [fs-explorer/lib64/unix](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib64/unix) - 32-bit/64-bit, precompiled by gcc from libyafuse2.

[fs-explorer/lib32/win](https://github.com/lemonjia/Fricando/blob/master/fs-explorer/lib32/win) - 32-bit, precompiled by Visual Studio 2010 from libyafuse2.

## Building

The build of FS Explorer requires Qt 5.2+.

After satisfying those requirements, the build is pretty simple:

```sh
cd fs-explorer
qmake
make && make install
```

# Authors

Lemon Jia, angersax@gmail.com
