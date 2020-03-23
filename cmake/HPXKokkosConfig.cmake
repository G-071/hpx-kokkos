get_filename_component(HPXKokkos_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

find_dependency(HPX REQUIRED)
find_dependency(Kokkos REQUIRED)

if(NOT TARGET HPXKokkos::hpx_kokkos)
  include("${HPXKokkos_CMAKE_DIR}/HPXKokkosTargets.cmake")
endif()
