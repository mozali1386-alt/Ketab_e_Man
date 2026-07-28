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

set_target_properties(Ketab_e_Man_Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/server")

# پوشه‌ی assets/data کنار exe در پوشه‌ی بیلد (برای اجرای مستقیم از بیلد)
add_custom_command(TARGET Ketab_e_Man_Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Ketab_e_Man_Server>/assets/data"
)

if (EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET Ketab_e_Man_Server POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Ketab_e_Man_Server>/assets"
    )
endif ()

if (WIN32)
    # کپی خودکار Qt DLL ها و پلاگین‌های لازم کنار exe
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

    if (WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET Ketab_e_Man_Server POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:Ketab_e_Man_Server>"
        )
    else ()
        message(WARNING "windeployqt not found - Qt DLLs will not be copied automatically")
    endif ()

    # کپی دستی DLL های ران‌تایم MinGW
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    foreach (RUNTIME_DLL
            libgcc_s_seh-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll)
        if (EXISTS "${MINGW_BIN_DIR}/${RUNTIME_DLL}")
            add_custom_command(TARGET Ketab_e_Man_Server POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_DIR}/${RUNTIME_DLL}"
                    "$<TARGET_FILE_DIR:Ketab_e_Man_Server>/${RUNTIME_DLL}"
            )
        endif ()
    endforeach ()
endif ()