#[=======================================================================[.rst:
FindVuzix
-----------

Find the Vuzix SDK components.

Imported Targets
^^^^^^^^^^^^^^^^

Several :ref:`imported targets <Imported targets>` are provided
if the SDK is found:

``Vuzix::iWearDriver``
  Main library for interacting with Vuzix devices.
``Vuzix::iWearStereoDriver``
  Routines for controlling stereo on Vuzix devices.
``Vuzix::TrackerMath``
  Tracker-related math utilities.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``Vuzix_FOUND``
  True if the SDK was found, false otherwise.
``Vuzix_INCLUDE_DIRS``
  Include directories needed to find Vuzix headers.
``Vuzix_LIBRARIES``
  The list of all Vuzix libraries.

It is recommended to link against the imported targets, rather than using the
latter two variables above.

Cache Variables
^^^^^^^^^^^^^^^

This module uses the following cache variables:

``Vuzix_IWEARDRV_LIBRARY``
  The location of the iWear driver library.
``Vuzix_IWRSTDRV_LIBRARY``
  The location of the iWear stereo driver library.
``Vuzix_TRACKERMATH_LIBRARY``
  The location of the Vuzix tracker math library.
``Vuzix_INCLUDE_DIR``
  The location of the Vuzix SDK include directory containing ``iWearSDK.h``.
``Vuzix_ROOT_DIR``
  An optional root to start looking for the Vuzix SDK, if it's not in its
  conventional location (x86 Program Files directory +
  ``/Vuzix Corporation/VuzixSDK/``)

The cache variables should not be used by project code.
They may be set by end users to point at VuzixSDK components, though the script
can find the current (as of this writing) version of the SDK in its default
location.

#]=======================================================================]

# Original Author:
# 2015 Kevin M. Godby <kevin@godby.org>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Vuzix_ROOT_DIR
	"${Vuzix_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search for Vuzix SDK")

if(WIN32)
    # Test 32/64 bits
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        set(Vuzix_LIBRARY_PATH_SUFFIX "lib/x64")
    else()
        set(Vuzix_LIBRARY_PATH_SUFFIX "lib")
    endif()

    # Get desired program files directory
    set(_PF86 "ProgramFiles(x86)")
    if(NOT "$ENV{${_PF86}}" STREQUAL "")
    # 32-bit dir: only set on win64
        file(TO_CMAKE_PATH "$ENV{_PF86}" _progfiles)
    else()
    # 32-bit dir on win32
        file(TO_CMAKE_PATH "$ENV{ProgramFiles}" _progfiles)
    endif()

    find_library(Vuzix_IWEARDRV_LIBRARY
        NAMES
        iweardrv
        PATHS
        "${Vuzix_ROOT_DIR}"
        "${_progfiles}/Vuzix Corporation/VuzixSDK/"
        PATH_SUFFIXES
        ${Vuzix_LIBRARY_PATH_SUFFIX})

    if(Vuzix_IWEARDRV_LIBRARY)
        get_filename_component(_libdir "${Vuzix_IWEARDRV_LIBRARY}" DIRECTORY)
    endif()

    find_library(Vuzix_IWRSTDRV_LIBRARY
        NAMES
        iwrstdrv
        HINTS
        ${_libdir}
        PATHS
        "${Vuzix_ROOT_DIR}"
        "${_progfiles}/Vuzix Corporation/VuzixSDK/"
        PATH_SUFFIXES
        ${Vuzix_LIBRARY_PATH_SUFFIX})

    find_library(Vuzix_TRACKERMATH_LIBRARY
        NAMES
        TrackerMath
        HINTS
        ${_libdir}
        PATHS
        "${Vuzix_ROOT_DIR}"
        "${_progfiles}/Vuzix Corporation/VuzixSDK/"
        PATH_SUFFIXES
        ${Vuzix_LIBRARY_PATH_SUFFIX})



    find_path(Vuzix_INCLUDE_DIR
        NAMES
        iWearSDK.h
        HINTS
        "${_libdir}/../inc"
        "${_libdir}/../../inc"
        PATHS
        "${Vuzix_ROOT_DIR}"
        "${_progfiles}/Vuzix Corporation/VuzixSDK/"
        PATH_SUFFIXES
        inc
    )
endif() # WIN32

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vuzix FOUND_VAR Vuzix_FOUND
    REQUIRED_VARS
    Vuzix_IWEARDRV_LIBRARY
    Vuzix_IWRSTDRV_LIBRARY
    Vuzix_TRACKERMATH_LIBRARY
    Vuzix_INCLUDE_DIR)

if(Vuzix_FOUND)
    set(APPEND Vuzix_LIBRARIES ${Vuzix_IWEARDRV_LIBRARY} ${Vuzix_IWRSTDRV_LIBRARY} ${Vuzix_TRACKERMATH_LIBRARY})
    set(Vuzix_INCLUDE_DIRS ${Vuzix_INCLUDE_DIR})
    mark_as_advanced(Vuzix_ROOT_DIR)
    if(NOT TARGET Vuzix::TrackerMath)
        add_library(Vuzix::TrackerMath UNKNOWN IMPORTED)
        set_target_properties(Vuzix::TrackerMath PROPERTIES
            IMPORTED_LOCATION "${Vuzix_TRACKERMATH_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${Vuzix_INCLUDE_DIRS}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C")
    endif()
    if(NOT TARGET Vuzix::iWearStereoDriver)
        add_library(Vuzix::iWearStereoDriver UNKNOWN IMPORTED)
        set_target_properties(Vuzix::iWearStereoDriver PROPERTIES
            IMPORTED_LOCATION "${Vuzix_IWRSTDRV_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${Vuzix_INCLUDE_DIRS}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C")
    endif()
    if(NOT TARGET Vuzix::iWearDriver)
        add_library(Vuzix::iWearDriver UNKNOWN IMPORTED)
        set_target_properties(Vuzix::iWearDriver PROPERTIES
            IMPORTED_LOCATION "${Vuzix_IWEARDRV_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${Vuzix_INCLUDE_DIRS}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C")
    endif()
endif()

mark_as_advanced(Vuzix_INCLUDE_DIR
    Vuzix_IWEARDRV_LIBRARY
    Vuzix_IWRSTDRV_LIBRARY
    Vuzix_TRACKERMATH_LIBRARY)
