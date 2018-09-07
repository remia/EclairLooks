cmake_minimum_required(VERSION 3.0)

project(ELook)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------

set(QT_PREFIX /usr/local/opt/qt)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_PREFIX_PATH ${QT_PREFIX}/lib/cmake)

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
find_package(Qt5UiTools)
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
    src/settings.cpp
    src/widget/uiloader.cpp
    src/widget/devwidget.cpp
    src/widget/imagewidget.cpp
    src/widget/logwidget.cpp
    src/widget/textureview.cpp
    src/widget/pipelinewidget.cpp
    src/widget/operatorlistwidget.cpp
    src/widget/operatorwidget.cpp
    src/widget/lookwidget.cpp
    src/widget/lookbrowserwidget.cpp
    src/widget/lookviewtabwidget.cpp
    src/widget/lookviewwidget.cpp
    src/widget/lookdetailwidget.cpp
    src/widget/lookselectionwidget.cpp
    src/widget/parameterwidget.cpp
    src/widget/settingwidget.cpp
    src/operator/imageoperator.cpp
    src/operator/imageoperatorlist.cpp
    src/operator/ociofiletransform_operator.cpp
    src/operator/ociocolorspace_operator.cpp
    src/operator/ociomatrix_operator.cpp
    src/operator/ctl_operator.cpp
    src/operator/ctl_transform.cpp
    src/scope/curvewidget.cpp
    src/scope/waveformwidget.cpp
    src/utils/chrono.cpp
    src/utils/parameterlist.cpp
)

qt5_add_resources(RESOURCES src/resources/res.qrc)

set(MACOSX_BUNDLE_ICON_FILE hexa.icns)
set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icons/hexa.icns)
set_source_files_properties(${ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCES} ${ICON})

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
target_link_libraries(${PROJECT_NAME} Qt5::UiTools)

# -----------------------------------------------------------------------------
# Packaging
# -----------------------------------------------------------------------------

add_custom_target(package
    COMMAND ${QT_PREFIX}/bin/macdeployqt ELook.app -dmg
    DEPENDS ${PROJECT_NAME}
)
