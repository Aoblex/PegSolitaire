cmake_minimum_required(VERSION 3.16)

project(solitaire VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        models/BoardSetup.cpp
        models/Board.cpp
        models/Board.h
        views/BoardView.cpp
        views/BoardView.h
        views/GameView.cpp
        views/GameView.h
        views/HomePageView.cpp
        views/HomePageView.h
        views/StartPageView.cpp
        views/StartPageView.h
        views/SettingsPageView.cpp
        views/SettingsPageView.h
        views/LoadingCircle.cpp
        views/LoadingCircle.h
        controllers/BoardController.cpp
        controllers/BoardController.h
        controllers/HomePageController.cpp
        controllers/HomePageController.h
        controllers/StartPageController.cpp
        controllers/StartPageController.h
        controllers/SettingsPageController.cpp
        controllers/SettingsPageController.h
        controllers/StrategyWorker.cpp
        controllers/StrategyWorker.h
        utils/ButtonStyles.cpp
        utils/ButtonStyles.h
        resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(solitaire
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET solitaire APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(solitaire SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(solitaire
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(solitaire PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(solitaire PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.pegsolitaire.game)
endif()
set_target_properties(solitaire PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/packaging/Info.plist.in
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "PegSolitaire"
)

include(GNUInstallDirs)
install(TARGETS solitaire
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(solitaire)
endif()
