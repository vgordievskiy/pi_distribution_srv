SET(project_name pi_distribution_srv)

project(${project_name})
cmake_minimum_required(VERSION 3.1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options("-W" "-Wall" "-Wextra" "-Werror" "-pedantic" "-O3")

SET(SRC
     src/RndData.cpp
     src/RndGenerator.cpp
     src/srv.cpp
)

add_executable(${project_name} ${SRC})
target_link_libraries(${project_name}
    CONAN_PKG::boost
    CONAN_PKG::pistache
    CONAN_PKG::nlohmann_json
)
target_include_directories(${project_name}
    PRIVATE
    ${CONAN_INCLUDE_DIRS_BOOST}
    ${CONAN_INCLUDE_DIRS_PISTACHE}
    ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON}
)