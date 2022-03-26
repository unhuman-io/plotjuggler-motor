set(PlotJuggler_DIR
    "${PlotJuggler_DIR}"
    CACHE
    PATH
    "Directory to search")

# if(CMAKE_SIZEOF_VOID_P MATCHES "8")
#     set(_LIBSUFFIXES /lib64 /lib)
# else()
#     set(_LIBSUFFIXES /lib)
# endif()

find_library(PlotJuggler_LIBRARY
    NAMES
    plotjuggler_base
    PATHS
    "${PlotJuggler_DIR}"
    PATH_SUFFIXES
    "/lib")

get_filename_component(_libdir "${PlotJuggler_LIBRARY}" PATH)

find_path(PlotJuggler_INCLUDE_DIR
    NAMES
    pj_plugin.h
    plotdata.h
    plotdatabase.h
    HINTS
    "${_libdir}"
    "${_libdir}/.."
    PATHS
    "${PlotJuggler_DIR}"
    PATH_SUFFIXES
    include/PlotJuggler)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PlotJuggler
    DEFAULT_MSG
    PlotJuggler_LIBRARY
    PlotJuggler_INCLUDE_DIR
    ${_deps_check})

if(PlotJuggler_FOUND)
    set(PlotJuggler_LIBRARIES "${PlotJuggler_LIBRARY}")
    set(PlotJuggler_INCLUDE_DIRS "${PlotJuggler_INCLUDE_DIR}/..")
    mark_as_advanced(PlotJuggler_ROOT_DIR)
endif()

mark_as_advanced(
    PlotJuggler_INCLUDE_DIR
    PlotJuggler_LIBRARY
    PlotJuggler_RUNTIME_LIBRARY
    PlotJuggler_DEFINITIONS)
