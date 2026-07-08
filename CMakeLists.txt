cmake_minimum_required(VERSION 3.19)
project(Ketab_e_Man LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts)
find_package(Qt6 REQUIRED COMPONENTS Pdf PdfWidgets)

qt_standard_project_setup()

qt_add_resources(RESOURCES "resources.qrc")
qt_add_executable(Ketab_e_Man
    WIN32 MACOSX_BUNDLE
    main.cpp
    loginwindow.cpp
    loginwindow.h
    loginwindow.ui
    .gitignore

    signinwindow.h signinwindow.cpp signinwindow.ui
    #resources.qrc
    ${RESOURCES}
    usersignupwindow.h usersignupwindow.cpp usersignupwindow.ui
    publishersignupwindow.h publishersignupwindow.cpp publishersignupwindow.ui
    resetpassworddialog.h resetpassworddialog.cpp resetpassworddialog.ui
    storepage.h storepage.cpp storepage.ui
    bookdetails.h bookdetails.cpp bookdetails.ui
    cartpage.h cartpage.cpp cartpage.ui
    cartitemwidget.h cartitemwidget.cpp cartitemwidget.ui
    userprofilepage.h userprofilepage.cpp userprofilepage.ui
    storebookwidget.h storebookwidget.cpp storebookwidget.ui
    historycardwidget.h historycardwidget.cpp historycardwidget.ui
    commentcardwidget.h commentcardwidget.cpp commentcardwidget.ui
    booklibraryitem.h booklibraryitem.cpp booklibraryitem.ui
    librarywidget.h librarywidget.cpp librarywidget.ui
    userdashboard.h userdashboard.cpp userdashboard.ui
    notificationdialog.h notificationdialog.cpp notificationdialog.ui
    notificationitem.h notificationitem.cpp notificationitem.ui
    pdfviewerwidget.h pdfviewerwidget.cpp pdfviewerwidget.ui
    publisherstatistics.h publisherstatistics.cpp publisherstatistics.ui
    bookmanagement.h bookmanagement.cpp bookmanagement.ui
    publisherprofilepage.h publisherprofilepage.cpp publisherprofilepage.ui
    publisherdashboard.h publisherdashboard.cpp publisherdashboard.ui


)

target_link_libraries(Ketab_e_Man
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Pdf
        Qt6::PdfWidgets
        Qt6::Charts
)
target_include_directories(Ketab_e_Man PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
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
