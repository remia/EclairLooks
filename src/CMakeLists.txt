
find_package(Qt5Widgets)
find_package(Qt5UiTools)

set(SOURCES
    main.cpp
    mainwindow.cpp
    image.cpp
    imagepipeline.cpp
    settings.cpp
    widget/common/slider.cpp
    widget/uiloader.cpp
    widget/devwidget.cpp
    widget/imagewidget.cpp
    widget/logwidget.cpp
    widget/textureview.cpp
    widget/pipelinewidget.cpp
    widget/operatorlistwidget.cpp
    widget/operatorwidget.cpp
    widget/lookwidget.cpp
    widget/lookbrowserwidget.cpp
    widget/lookviewtabwidget.cpp
    widget/lookviewwidget.cpp
    widget/lookdetailwidget.cpp
    widget/lookselectionwidget.cpp
    widget/parameterwidget.cpp
    widget/settingwidget.cpp
    operator/imageoperator.cpp
    operator/imageoperatorlist.cpp
    operator/ociofiletransform_operator.cpp
    operator/ociocolorspace_operator.cpp
    operator/ociomatrix_operator.cpp
    operator/ctl_operator.cpp
    operator/ctl_transform.cpp
    scope/cubewidget.cpp
    scope/neutralwidget.cpp
    scope/waveformwidget.cpp
    utils/chrono.cpp
    parameter/parameterlist.cpp
)

qt5_add_resources(RESOURCES resources/res.qrc)

set(MACOSX_BUNDLE_ICON_FILE hexa.icns)
set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hexa.icns)
set_source_files_properties(${ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCES} ${ICON})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        opencolorio
        openimageio
        openexr
        ctl
        Qt5::Widgets
        Qt5::UiTools
        ${CXX_FILESYSTEM_LIB}
)