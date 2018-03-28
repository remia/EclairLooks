cmake_minimum_required(VERSION 3.0)

project(looks)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH /usr/local/Qt-5.7.0/5.7/clang_64/lib/cmake)
set(CMAKE_CXX_FLAGS "-fcolor-diagnostics -Wall -Werror -Wfatal-errors -g -fno-omit-frame-pointer -fsanitize=address")

#set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk")
#set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")

# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

find_package(Qt5Widgets)
find_package(OpenImageIO)
find_package(OpenColorIO)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/imagewidget.cpp
    src/logwidget.cpp
    src/image.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${OIIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OCIO_INCLUDES})
target_link_libraries(${PROJECT_NAME} ${OIIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OCIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
