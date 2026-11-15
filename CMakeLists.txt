cmake_minimum_required(VERSION 3.30)
project(ADNS_3080_Viewer VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSERIALPORT REQUIRED IMPORTED_TARGET libserialport)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ADNS_3080_Viewer src/main.cpp)

target_compile_definitions(ADNS_3080_Viewer PRIVATE ASIO_STANDALONE)
target_link_libraries(ADNS_3080_Viewer PRIVATE ${OpenCV_LIBS})
target_link_libraries(ADNS_3080_Viewer PRIVATE PkgConfig::LIBSERIALPORT)
if(WIN32)
    target_link_libraries(ADNS_3080_Viewer PRIVATE ws2_32 mswsock)
endif()

target_compile_definitions(ADNS_3080_Viewer PRIVATE ASSET_DIR="${CMAKE_SOURCE_DIR}")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
