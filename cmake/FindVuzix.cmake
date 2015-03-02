# - try to find the Vuzix SDK
#
# Cache Variables: (probably not for direct use in your scripts)
#  VUZIX_INCLUDE_DIR
#  VUZIX_LIBRARY
#
# Non-cache variables you might use in your CMakeLists.txt:
#  VUZIX_FOUND
#  VUZIX_INCLUDE_DIRS
#  VUZIX_LIBRARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2015 Kevin M. Godby <kevin@godby.org>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(VUZIX_ROOT_DIR
	"${VUZIX_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search for Vuzix SDK")

# Test 32/64 bits
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set(VUZIX_LIBRARY_PATH_SUFFIX "x64")
else()
	set(VUZIX_LIBRARY_PATH_SUFFIX "lib")
endif()

find_library(VUZIX_IWEARDRV_LIBRARY
	NAMES
	iweardrv
	PATHS
	"${VUZIX_ROOT_DIR}"
	"C:/Program Files (x86)/Vuzix Corporation/VuzixSDK/"
	PATH_SUFFIXES
	${VUZIX_LIBRARY_PATH_SUFFIX}
)

find_library(VUZIX_IWRSTDRV_LIBRARY
	NAMES
	iwrstdrv
	PATHS
	"${VUZIX_ROOT_DIR}"
	"C:/Program Files (x86)/Vuzix Corporation/VuzixSDK/"
	PATH_SUFFIXES
	${VUZIX_LIBRARY_PATH_SUFFIX}
)

find_library(VUZIX_TRACKERMATH_LIBRARY
	NAMES
	TrackerMath
	PATHS
	"${VUZIX_ROOT_DIR}"
	"C:/Program Files (x86)/Vuzix Corporation/VuzixSDK/"
	PATH_SUFFIXES
	${VUZIX_LIBRARY_PATH_SUFFIX}
)

find_path(VUZIX_INCLUDE_DIR
	NAMES
	iWearSDK.h
	HINTS
	"${_libdir}"
	"${_libdir}/.."
	PATHS
	"${VUZIX_ROOT_DIR}"
	"C:/Program Files (x86)/Vuzix Corporation/VuzixSDK/"
	PATH_SUFFIXES
	inc
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VUZIX
	DEFAULT_MSG
	VUZIX_IWEARDRV_LIBRARY
	VUZIX_IWRSTDRV_LIBRARY
	VUZIX_TRACKERMATH_LIBRARY
	VUZIX_INCLUDE_DIR
)

if(VUZIX_FOUND)
	list(APPEND VUZIX_LIBRARIES ${VUZIX_IWEARDRV_LIBRARY} ${VUZIX_IWRSTDRV_LIBRARY} ${VUZIX_TRACKERMATH_LIBRARY})
	list(APPEND VUZIX_INCLUDE_DIRS ${VUZIX_INCLUDE_DIR})
	mark_as_advanced(VUZIX_ROOT_DIR)
endif()

mark_as_advanced(VUZIX_INCLUDE_DIR
	VUZIX_IWEARDRV_LIBRARY
	VUZIX_IWRSTDRV_LIBRARY
	VUZIX_TRACKERMATH_LIBRARY
)
