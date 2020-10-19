# cmake tutorial
# https://cmake.org/cmake/help/v3.16/guide/tutorial/index.html

cmake_minimum_required(VERSION 3.10.0)
project(Asio VERSION 0.1.0)

# specify the C++ standard to c14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(INC_dir ${PROJECT_SOURCE_DIR}/src/include)
set(LIB_dir ${PROJECT_SOURCE_DIR}/libs)
set(ASIO_dir ${LIB_dir}/asio)
set(NETCOMMON_dir ${LIB_dir}/NetCommon) 

set(Asio_VERSION_MAJOR 0)
set(Asio_VERSION_MINOR 1)

configure_file(${INC_dir}/AsioConfig.h.in ${INC_dir}/AsioConfig.h)
include(CTest)
enable_testing()

add_executable(App src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Asio_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Asio_VERSION_MINOR}")
include(CPack)

# Add the NetCommon library
add_subdirectory(${NETCOMMON_dir})

# List libs and includes
list(APPEND LIBS NetCommon)

# The application
target_include_directories(App PUBLIC
    "${INC_dir}"
    "${ASIO_dir}/include"
    )

# Imports pthread for Asio
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(App PRIVATE
    NetCommon
    Threads::Threads)

install(TARGETS App DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/AsioConfig.h"
  DESTINATION include
  )
message( ">> include " ${INC_dir})
