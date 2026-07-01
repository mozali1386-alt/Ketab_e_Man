cmake_minimum_required(VERSION 3.19)
project(Ketab_e_Man LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_resources(RESOURCES "resources.qrc")
qt_add_executable(Ketab_e_Man
    WIN32 MACOSX_BUNDLE
    main.cpp
    loginwindow.cpp
    loginwindow.h
    loginwindow.ui
    .gitignore

    #resources.qrc
    ${RESOURCES}


)

target_link_libraries(Ketab_e_Man
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Ketab_e_Man
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Ketab_e_Man
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
