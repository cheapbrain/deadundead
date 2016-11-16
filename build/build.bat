@echo off
SETLOCAL
setlocal EnableDelayedExpansion

rmdir /S /Q objs
mkdir objs

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

set "_libs= kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glu32.lib gdi32.lib opengl32.lib glfw3.lib glew32.lib "

set "_compile_debug=cl /nologo /DDEBUG /D_CRT_SECURE_NO_WARNINGS /c /FC /EHa- /Od /Gd /W3 /MD /Gm- /GR- /Zi /I..\code\include "
set "_compile_release=cl /nologo /DRELEASE /D_CRT_SECURE_NO_WARNINGS /c /FC /EHa- /O2 /GA /Gd /W3 /MD /Gm- /GR- /I..\code\include "

set "_link_debug=link /NOLOGO /OUT:debug.exe /OPT:REF /SUBSYSTEM:windows /INCREMENTAL:NO /LIBPATH:..\code\lib "
set "_link_release=link /NOLOGO /OUT:release.exe /OPT:REF /SUBSYSTEM:windows /INCREMENTAL:NO /LIBPATH:..\code\lib "

set "_compile=%_compile_debug%"
set "_link=%_link_debug%%_libs%"

for /R ..\code\src\ %%i in (*.cpp) do %_compile% %%~i /Foobjs\%%~ni.obj

set seq=
for %%i in (objs/*.*) do set seq=!seq! objs\%%~ni.obj 

%_link% %seq%

rem cl /nologo /DDEBUG /D_CRT_SECURE_NO_WARNINGS /c /FC /EHa- /Od /Gd /W3 /MD /Gm- /GR- /Zi /I..\code\include ..\code\src\win32_main.cpp ..\code\src\renderer.cpp ..\code\src\assets.cpp ..\code\src\math.cpp
rem link /NOLOGO /OUT:debug.exe /OPT:REF /SUBSYSTEM:windows /INCREMENTAL:NO /LIBPATH:..\code\lib win32_main.obj renderer.obj assets.obj math.obj kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glu32.lib gdi32.lib opengl32.lib glfw3.lib glew32.lib

REM -arch:IA32, SSE, SSE2, AVX, AVX2
REM -Dname defines macro
REM -EHa- disables exceptions
REM -favor:blend, ATOM, AMD64, INTEL64
REM -fp:fast, strict, precise
REM -FC shows source file path
REM -Fmpath.map location to function map file
REM -GA better performance for .exe
REM -GL enables whole program optimization
REM -Gm- disable incremental build
REM -GR- disables runtine type information (needed for c++ OOP only)
REM -Idirectory include directory
REM -MT pack runtime library in exe
REM -nologo disable compiler infos text
REM -O2 enables optimizations
REM -Od disable optimizations
REM -Oi enables native asm functions (like sin cos)
REM -Ot optimizations
REM -Oy omits frame pointers
REM -wd4201 ignore warning 4201
REM -W4 enables warning up to level 4
REM -WX consider warnings as errors
REM -Zi produce debug information
REM -Z7 different debug format (better??)

REM /link
REM /link -subsystem:windows,5.1 windows xp support
REM -link -opt:ref remove not used functions