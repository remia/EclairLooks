cmake_minimum_required(VERSION 3.0)

project(looks)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------

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

set(SOURCES main.cpp mainwindow.cpp imagewidget.cpp logwidget.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)




