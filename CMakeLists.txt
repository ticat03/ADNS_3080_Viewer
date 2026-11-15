cmake_minimum_required(VERSION 3.15)
project(ADNS_3080_Viewer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSERIALPORT REQUIRED IMPORTED_TARGET libserialport)

add_executable(ADNS_3080_Viewer
    src/main.cpp
    src/Image.cpp
)

target_include_directories(ADNS_3080_Viewer PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ADNS_3080_Viewer PRIVATE
    ${OpenCV_LIBS}
    PkgConfig::LIBSERIALPORT
)

target_compile_definitions(ADNS_3080_Viewer PRIVATE ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/docs")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
