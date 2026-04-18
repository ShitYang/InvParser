cmake_minimum_required(VERSION 3.10.0)
project(InvParser VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(
            Qt5 
            PATHS ${CMAKE_SOURCE_DIR}/../afsim-2.9.0-win64/swdev/src/build/3rd_party/qt-5.12.11-x64-vs2017/lib/cmake
            REQUIRED
            COMPONENTS Core Widgets OpenGL WinExtras
)

add_definitions(-DNOMINMAX)

add_subdirectory(util)
add_subdirectory(wsf_util/source)
add_subdirectory(wsf_parser)

FILE(GLOB HDRS *.hpp *.h)
FILE(GLOB SRCS *.cpp)

add_executable(InvParser ${HDRS} ${SRCS})

target_link_libraries(InvParser PUBLIC util wsf_parser Qt5::Widgets Qt5::Core Qt5::WinExtras)


