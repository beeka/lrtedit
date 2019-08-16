REM This script creates a formal release build of the program.
set PROGRAM=lrtedit

set ROOTDIR=%CD%
set BUILDDIR=%CD%\.build

REM Ensure Qt is in the path (Would like to call this script from Qt Creator)?
set MYQTDIR=C:\Qt\5.9.7\mingw53_32
set MYCXXDIR=C:\Qt\Tools\mingw530_32\bin
set ZIPDIR=C:\Program Files\7-Zip

set PATH=%MYQTDIR%\bin;%MYCXXDIR%;%ZIPDIR%;%PATH%;

REM Build it
mkdir %BUILDDIR%
cd %BUILDDIR%
qmake ..\src
mingw32-make -j8 release

REM Create deployment dir
set DEPLOYDIR=%BUILDDIR%\%PROGRAM%
mkdir %DEPLOYDIR%
xcopy /Y %ROOTDIR%\LICENSE %DEPLOYDIR%
xcopy /Y %ROOTDIR%\README.md %DEPLOYDIR%
xcopy /Y %BUILDDIR%\release\%PROGRAM%.exe %DEPLOYDIR%
cd %DEPLOYDIR%
windeployqt --no-quick-import --no-translations --no-system-d3d-compiler --compiler-runtime  --no-webkit2 --no-angle --no-opengl-sw %PROGRAM%.exe

REM Create an archive
cd %BUILDDIR%
7z a %ROOTDIR%\%PROGRAM%.zip %PROGRAM%

REM Show the user the status
pause

REM Tidy up
cd %ROOTDIR%
rmdir  /S /Q %BUILDDIR%
