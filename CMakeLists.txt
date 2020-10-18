# cmake tutorial
# https://cmake.org/cmake/help/v3.16/guide/tutorial/index.html
cmake_minimum_required(VERSION 3.10.0)
project(Asio VERSION 0.1.0)

set(INC_dir ${PROJECT_SOURCE_DIR}/src/include)
set(Asio_VERSION_MAJOR 0)
set(Asio_VERSION_MINOR 1)

configure_file(${INC_dir}/AsioConfig.h.in ${INC_dir}/AsioConfig.h)
include(CTest)
enable_testing()

add_executable(Asio src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Asio_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Asio_VERSION_MINOR}")
include(CPack)


target_include_directories(Asio PUBLIC
    "${INC_dir}"
    )

message( ">> include " ${INC_dir})
