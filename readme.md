xsdump
------
A very simple screenshot tool for X11.

Usage
-----
```
xsdump OPTION

Options are:

-o or --output [PATH]: outputs the image to PATH(include name of file & extension must be .png), otherwise is saved in HOME
-h or --help: shows this help menu
-v or --version: shows xsdump's version information
```

Build
-----
You need the following:
g++, libx11-dev, libpng, png++

You may want to compile it using the following command:
```
g++ -c xsdump.cc `libpng-config --cflags` -lX11 && g++ -o xsdump xsdump.o `libpng-config --ldflags` -lX11 && rm xsdump.o
```

Copyright (C) 2022 Andrei Datcu.This project is licensed under the GNU GPL v2 (see [license](https://github.com/datcuandrei/xsdump/blob/main/LICENSE))

This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
