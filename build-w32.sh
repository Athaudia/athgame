#!/bin/bash
i686-w64-mingw32-gcc -DPLATFORM_WIN32 -std=c99 -O3 src/*.c -luser32 -lgdi32 -lwinmm -Werror -Wall -static -Iinclude -I/home/athaudia/depends/freetype-2.5.5/include -L/home/athaudia/depends/freetype-2.5.5/objs -lfreetype-light -omain.exe
strip main.exe
