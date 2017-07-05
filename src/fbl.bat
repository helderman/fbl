@echo off
rem --------------------------------------------------------------------------
rem  fbl.bat - batch file to compile a FBL program to native code
rem 
rem  Step 1: fblc.exe compiles .fbl file (together with fblrt.fbl) to .c file
rem  Step 2: cl.exe compiles .c file and links it with the runtime library
rem --------------------------------------------------------------------------
rem  TODO: FBLC_OPTS

set BINDIR=%~dp0
set LIBDIR=%BINDIR%\..\lib
set INCLUDEDIR=%BINDIR%\..\include
set DATADIR=%BINDIR%\..\share
set TMPDIR=C:\windows\temp
set TMPCFILE=%TMPDIR%\fblc_output.c

set RUNTIME=%LIBDIR%\fblrt.lib
set SOURCES=%DATADIR%\fblrt.fbl %*
set FBLC_OPTS=
set CL_OPTS=/nologo /I%INCLUDEDIR%

"%BINDIR%\fblc.exe" %FBLC_OPTS% %SOURCES% > %TMPCFILE%
if not errorlevel 1 cl.exe %CL_OPTS% %TMPCFILE% %RUNTIME%
