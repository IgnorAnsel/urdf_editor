cmake_minimum_required(VERSION 3.5)

project(urdf_editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets 3DCore 3DExtras 3DRender OpenGL OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets 3DCore 3DExtras 3DRender OpenGL OpenGLWidgets)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        actionhandler.h actionhandler.cpp
        urdf_editor.cpp urdf_editor.h urdf_editor.ui
        property.h property.cpp property.ui
        param_struct.h
        shape.h shape.cpp
        camera.h camera.cpp
        shape_relation.h shape_relation.cpp shape_relation.ui
        codemaker.h codemaker.cpp
        base_setting.h base_setting.cpp base_setting.ui
        res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(urdf_editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(urdf_editor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(urdf_editor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(urdf_editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::3DCore Qt${QT_VERSION_MAJOR}::3DExtras
    Qt${QT_VERSION_MAJOR}::3DRender Qt${QT_VERSION_MAJOR}::OpenGLWidgets Qt${QT_VERSION_MAJOR}::OpenGL
    glm::glm ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLU_LIBRARY}
)

set_target_properties(urdf_editor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS urdf_editor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(urdf_editor)
endif()
