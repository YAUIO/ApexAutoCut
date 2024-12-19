cmake_minimum_required(VERSION 3.30)
project(ApexAutoCut)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})

include_directories(/usr/include)
link_directories(/usr/lib)

find_package(fmt)

add_executable(ApexAutoCut main.cpp
        Detection.cpp
        Detection.h)

target_link_libraries(ApexAutoCut ${OpenCV_LIBS} ${FFMPEG_LIBRARIES} tesseract fmt)
target_compile_definitions(ApexAutoCut PRIVATE ${FFMPEG_CFLAGS_OTHER})