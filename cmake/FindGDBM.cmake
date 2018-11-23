# - Find gdbm
# Find the native gdbm headers and libraries.
#
# GDBM_INCLUDE_DIRS	- where to find gdbm.h
# GDBM_LIBRARIES	- List of libraries when using gdbm.
# GDBM_FOUND	        - True if gdbm found.

# Look for the header file.
FIND_PATH(GDBM_INCLUDE_DIR NAMES gdbm.h)

# Look for the library.
FIND_LIBRARY(GDBM_LIBRARY NAMES gdbm)

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GDBM DEFAULT_MSG GDBM_LIBRARY GDBM_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GDBM_FOUND)
	SET(GDBM_LIBRARIES ${GDBM_LIBRARY})
	SET(GDBM_INCLUDE_DIRS ${GDBM_INCLUDE_DIR})
ELSE(GDBM_FOUND)
	SET(GDBM_LIBRARIES)
	SET(GDBM_INCLUDE_DIRS)
ENDIF(GDBM_FOUND)

MARK_AS_ADVANCED(GDBM_INCLUDE_DIRS GDBM_LIBRARIES)
