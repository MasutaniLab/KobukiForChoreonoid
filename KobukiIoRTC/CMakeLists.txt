cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(KobukiIoRTC)

find_package(OpenRTM)
find_package(Choreonoid)

if (DEFINED OPENRTM_INCLUDE_DIRS)
  string(REGEX REPLACE "-I" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
endif (DEFINED OPENRTM_INCLUDE_DIRS)

if (DEFINED OPENRTM_LIBRARY_DIRS)
  string(REGEX REPLACE "-L" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
endif (DEFINED OPENRTM_LIBRARY_DIRS)

if (DEFINED OPENRTM_LIBRARIES)
  string(REGEX REPLACE "-l" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
endif (DEFINED OPENRTM_LIBRARIES)

add_definitions(${OPENRTM_DEFINITIONS})
add_definitions(${CHOREONOID_DEFINITIONS})
include_directories(${OPENRTM_INCLUDE_DIRS})
include_directories(${CHOREONOID_INCLUDE_DIRS})
link_directories(${OPENRTM_LIBRARY_DIRS})
link_directories(${CHOREONOID_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
  add_compile_options(${OPENRTM_CFLAGS})
  add_definitions(
    -D__WIN32__ -D__x86__ -D__NT__ -D__OSVERSION__=4
    -D_CRT_SECURE_NO_DEPRECATE -D_WIN32_WINNT=0x0500 -DRTC_CORBA_CXXMAPPING11)
endif()

function(add_cnoid_body_io_rtc)
  set(target ${ARGV0})
  list(REMOVE_AT ARGV 0)

  add_library(${target} SHARED ${ARGV})
  target_link_libraries(${target} CnoidBodyIoRTC ${OPENRTM_CAMERA_LIBRARY})
  
  if(MSVC)
    add_compile_options(${OPENRTM_CFLAGS})
  endif()

  set_target_properties(${target} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}/rtc
    PREFIX "")
  if(ENABLE_INSTALL_RPATH)
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/../..")
  endif()

  install(TARGETS ${target}
    RUNTIME DESTINATION ${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug
    LIBRARY DESTINATION ${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug)
endfunction()

add_cnoid_body_io_rtc(KobukiIoRTC KobukiIoRTC.cpp)
