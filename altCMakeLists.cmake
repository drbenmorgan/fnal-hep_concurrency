cmake_minimum_required(VERSION 3.3)
project(hep_concurrency VERSION 1.0.2)

# - Cetbuildtools, version2
find_package(cetbuildtools2 0.2.0 REQUIRED)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${cetbuildtools2_MODULE_PATH})
set(CET_COMPILER_CXX_STANDARD_MINIMUM 14)
include(CetInstallDirs)
include(CetCMakeSettings)
include(CetCompilerSettings)

# TBB
find_package(TBB REQUIRED)

# Build
add_subdirectory(hep_concurrency)
