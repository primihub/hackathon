get_filename_component(ENCRYPTO_utils_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

list(APPEND CMAKE_MODULE_PATH "${ENCRYPTO_utils_CMAKE_DIR}")

include(CMakeFindDependencyMacro)

find_dependency(GMP)
find_dependency(GMPXX)
find_dependency(OpenSSL)

if(NOT TARGET ENCRYPTO_utils::encrypto_utils)
    include("${ENCRYPTO_utils_CMAKE_DIR}/ENCRYPTO_utilsTargets.cmake")
endif()
