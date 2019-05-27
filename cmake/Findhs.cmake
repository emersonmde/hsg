# - Try to find hs
# Once done, this will define
#
#  hs_FOUND - system has hyperscan
#  hs_INCLUDE_DIRS - the hyperscan include directories
#  hs_LIBRARIES - link these to use hyperscan
include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(hs_PKGCONF hs)

# Include dir
find_path(hs_INCLUDE_DIR
  NAMES hs.h
  PATHS ${hs_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES hs
)

# Finally the library itself
find_library(hs_LIBRARY
  NAMES hs
  PATHS ${hs_PKGCONF_LIBRARY_DIRS}
  PATH_SUFFIXES hs
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(hs_PROCESS_INCLUDES hs_INCLUDE_DIR)
set(hs_PROCESS_LIBS hs_LIBRARY)
libfind_process(hs)
