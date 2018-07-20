cmake_minimum_required(VERSION 3.0)

project(looks)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_PREFIX_PATH /usr/local/Cellar/qt/5.10.1/lib/cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "-fcolor-diagnostics -Wall -Werror -Wfatal-errors")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

#set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk")
#set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Qt5Widgets)
find_package(OpenImageIO)
find_package(OpenColorIO)
find_package(CTL)
find_package(IlmBase)
find_package(OpenEXR)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/image.cpp
    src/imagepipeline.cpp
    src/widget/imagewidget.cpp
    src/widget/logwidget.cpp
    src/widget/transformationwidget.cpp
    src/widget/transformationlistwidget.cpp
    src/operator/imageoperator.cpp
    src/operator/imageoperatorparameterlist.cpp
    src/operator/ociofiletransform_operator.cpp
    src/operator/ociocolorspace_operator.cpp
    src/operator/ociomatrix_operator.cpp
    src/operator/ctl_operator.cpp
    src/operator/ctl_transform.cpp
    src/scope/waveformwidget.cpp
    src/utils/chrono.cpp
)

qt5_add_resources(RESOURCES src/resources/res.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${OIIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${OCIO_INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CTL_INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenEXR_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OIIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OCIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CTL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${IlmBase_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${OpenEXR_LIBRARY})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
