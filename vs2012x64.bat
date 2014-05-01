call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

set CURRDIR=%CD%

set QTDIR=C:\Qt\Qt5.2.0\5.2.0\msvc2012_64_opengl
set BOOST_DIR=C:\projects\3rdparty\boost_1_55_0
set BOOST_ROOT=C:\projects\3rdparty\boost_1_55_0
set ATISTREAMSDKROOT=C:\projects\3rdparty\AMD APP SDK\2.9
set BUILD_DIR=%CURRDIR%\build\VS2012\x64
set ICU_DIR=C:\projects\3rdparty\icu
set CMAKE_PREFIX_PATH=%QTDIR%/lib/cmake

mkdir %BUILD_DIR%
cd %BUILD_DIR%
set path=%QTDIR%/bin;%ICU_DIR%\bin64;%BOOST_DIR%\lib;%path%

echo %CD%
set cmake_call=cmake -G "Visual Studio 11 Win64" %CURRDIR%
echo %cmake_call%

cmake -G "Visual Studio 11 Win64" %CURRDIR%

set SOLUTION_FILE=%BUILD_DIR%\TestOpenCL.sln

devenv "%SOLUTION_FILE%"