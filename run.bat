Path=C:\dev\mingw-w64\i686-4.9.2-posix-dwarf-rt_v4-rev2\mingw32\bin
cd ..
del main.exe
REM tcc -DPLATFORM_WIN32 -O3 -luser32 -lgdi32 main.c surface.c window.c platform.c athgame.c events.c
gcc -DPLATFORM_WIN32 -std=c99 -O3 src/*.c -luser32 -lgdi32 -lwinmm -Werror -Wall -static -Iinclude -IC:\Libs\freetype-2.5.5\include -LC:\Libs\freetype-2.5.5\objs -lfreetype-light -omain.exe
main.exe