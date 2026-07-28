#in the name of ALLAH
#YA MAHDI

cmake_minimum_required(VERSION 3.30)
project(Ketab_e_Man)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

set(SHARED_SOURCES
        shared/Protocol.cpp
        shared/Enums.cpp
        shared/BaseEntity.cpp
        shared/User.cpp
        shared/NormalUser.cpp
        shared/Publisher.cpp
        shared/Admin.cpp
        shared/Author.cpp
        shared/Book.cpp
        shared/Library.cpp
        shared/Shelf.cpp
        shared/Review.cpp
        shared/Cart.cpp
        shared/Notification.cpp
        shared/Wallet.cpp
        shared/Transaction.cpp
        shared/Purchase.cpp
        shared/PipeEscape.cpp
)

set(SHARED_HEADERS
        shared/Enums.h
        shared/Protocol.h
        shared/BaseEntity.h
        shared/User.h
        shared/NormalUser.h
        shared/Publisher.h
        shared/Admin.h
        shared/Author.h
        shared/Book.h
        shared/Library.h
        shared/Shelf.h
        shared/Review.h
        shared/Cart.h
        shared/Notification.h
        shared/Wallet.h
        shared/Transaction.h
        shared/Purchase.h
        shared/PipeEscape.h
)

add_executable(Ketab_e_Man_Server
        server/main.cpp
        server/ServerCore.cpp
        server/ServerCore.h
        server/ServerDataStore.cpp
        server/ServerDataStore.h
        server/ServerCore_Auth.cpp
        server/ServerCore_Books.cpp
        server/ServerCore_Shopping.cpp
        server/ServerCore_Library.cpp
        server/ServerCore_Publisher.cpp
        server/ServerCore_Notification.cpp
        server/ServerCore_Pdf.cpp
        server/ServerCore_Comments.cpp
        server/ServerCore_Admin.cpp
        server/ServerCore_AdminBooks.cpp
        server/ServerCore_Profile.cpp
        server/ServerStats.cpp
        server/ServerStats.h
        server/network/ServerNetwork.cpp
        server/network/ServerNetwork.h
        server/network/ClientHandler.cpp
        server/network/ClientHandler.h
        server/database/FileManager.cpp
        server/database/FileManager.h
        server/gui/ServerDashboard.cpp
        server/gui/ServerDashboard.h
        server/gui/ServerLogModel.cpp
        server/gui/ServerLogModel.h
        ${SHARED_SOURCES}
        ${SHARED_HEADERS}
)

target_include_directories(Ketab_e_Man_Server PRIVATE shared)
target_link_libraries(Ketab_e_Man_Server Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
target_compile_definitions(Ketab_e_Man_Server PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
set_target_properties(Ketab_e_Man_Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/server")

if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

    foreach (TARGET_NAME Ketab_e_Man_Server)
        if (WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${WINDEPLOYQT_EXECUTABLE}
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:${TARGET_NAME}>"
            )
        endif ()
    endforeach ()
endif ()

# ============= CPack Settings for Inno Setup =============
set(CPACK_PACKAGE_NAME "Ketab_e_Man")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ketab_e_Man - Digital Library Management System")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})

# Install the server executable
install(TARGETS Ketab_e_Man_Server
        RUNTIME DESTINATION ..
        BUNDLE DESTINATION ..
        LIBRARY DESTINATION lib)

# Install required Qt DLLs and other runtime libraries
include(InstallRequiredSystemLibraries)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ..)

# Set the generator for Windows
if (WIN32)
    set(CPACK_GENERATOR "INNOSETUP")

    # Optional: Inno Setup specific settings
    set(CPACK_INNOSETUP_ARCHITECTURE "x64")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win64")

    # If you have an icon file, uncomment and set the path
    # set(CPACK_INNOSETUP_SETUP_SetupIconFile "${CMAKE_SOURCE_DIR}/icon.ico")
    # set(CPACK_INNOSETUP_SETUP_UninstallDisplayIcon "${CMAKE_SOURCE_DIR}/icon.ico")

    # If you have a license file
    # set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
endif()

# Include CPack module (must be at the end)
include(CPack)