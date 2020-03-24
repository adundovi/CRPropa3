# Find FFTW3/FFTW3F with single/double precision. Sets
# FFTW3_INCLUDE_DIR = fftw3.h
# FFTW3_FOUND = true if fftw3f is found
# FFTW3_LIBRARY = libfftw3f.a .so
# FFTW3F_FOUND = true if fftw3f is found
# FFTW3F_LIBRARY = libfftw3f.a .so

find_path(FFTW3_INCLUDE_DIR fftw3.h)
find_library(FFTW3F_LIBRARY fftw3f)
find_library(FFTW3_LIBRARY fftw3)
find_library(FFTW3F_THREADS_LIBRARY fftw3f_threads)
find_library(FFTW3_THREADS_LIBRARY fftw3_threads)

if(FFTW3_INCLUDE_DIR)
    MESSAGE(STATUS "FFTW3 header: Found!")
else()
    MESSAGE(STATUS "FFTW3 header: NOT Found!")
endif()
MESSAGE(STATUS "  Include:     ${FFTW3_INCLUDE_DIR}")

set(FFTW3F_FOUND FALSE)
if(FFTW3F_LIBRARY)
    set(FFTW3F_FOUND TRUE)
    MESSAGE(STATUS "FFTW3 with single precision (FFTW3F): Found!")
else()
    MESSAGE(STATUS "FFTW3 with single precision (FFTW3F): NOT Found!")
endif()
MESSAGE(STATUS "  Library:     ${FFTW3F_LIBRARY}")

set(FFTW3_FOUND FALSE)
if(FFTW3_LIBRARY)
    set(FFTW3_FOUND TRUE)
    MESSAGE(STATUS "FFTW3 with double precision (FFTW3): Found!")
else()
    MESSAGE(STATUS "FFTW3 with double precision (FFTW3): NOT Found!")
endif()
MESSAGE(STATUS "  Library:     ${FFTW3_LIBRARY}")

set(FFTW3F_THREADS_FOUND FALSE)
if(FFTW3F_THREADS_LIBRARY)
    set(FFTW3F_THREADS_FOUND TRUE)
    MESSAGE(STATUS "FFTW3 with threads support (FFTW3F_THREADS): Found!")
else()
    MESSAGE(STATUS "FFTW3 with threads support (FFTW3F_THREADS): NOT Found!")
endif()
MESSAGE(STATUS "  Library:     ${FFTW3F_THREADS_LIBRARY}")

set(FFTW3_THREADS_FOUND FALSE)
if(FFTW3_THREADS_LIBRARY)
    set(FFTW3_THREADS_FOUND TRUE)
    MESSAGE(STATUS "FFTW3 with threads support (FFTW3_THREADS): Found!")
else()
    MESSAGE(STATUS "FFTW3 with threads support (FFTW3_THREADS): NOT Found!")
endif()
MESSAGE(STATUS "  Library:     ${FFTW3_THREADS_LIBRARY}")

mark_as_advanced(FFTW3_INCLUDE_DIR FFTW3_LIBRARY FFTW3F_LIBRARY FFTW3F_THREADS_LIBRARY FFTW3_THREADS_LIBRARY FFTW3_FOUND FFTW3F_FOUND)
