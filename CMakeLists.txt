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
        src/shared/BaseEntity.cpp
        src/shared/User.cpp
        src/shared/NormalUser.cpp
        src/shared/Publisher.cpp
        src/shared/Admin.cpp
        src/shared/Author.cpp
        src/shared/Book.cpp
        src/shared/Library.cpp
        src/shared/Shelf.cpp
        src/shared/Review.cpp
        src/shared/Cart.cpp
        src/shared/Notification.cpp
        src/shared/Wallet.cpp
        src/shared/Transaction.cpp
        src/shared/Purchase.cpp
)

set(SHARED_HEADERS
        src/shared/Enums.h
        src/shared/Protocol.h
        src/shared/BaseEntity.h
        src/shared/User.h
        src/shared/NormalUser.h
        src/shared/Publisher.h
        src/shared/Admin.h
        src/shared/Author.h
        src/shared/Book.h
        src/shared/Library.h
        src/shared/Shelf.h
        src/shared/Review.h
        src/shared/Cart.h
        src/shared/Notification.h
        src/shared/Wallet.h
        src/shared/Transaction.h
        src/shared/Purchase.h
)

add_executable(Ketab_e_Man_Server
        #server/main.cpp
#        server/ServerCore.cpp
#        server/ServerCore.h
#        server/ServerStats.cpp
#        server/ServerStats.h
#        server/network/ServerNetwork.cpp
#        server/network/ServerNetwork.h
#        server/network/ClientHandler.cpp
#        server/network/ClientHandler.h
#        server/database/FileManager.cpp
#        server/database/FileManager.h
#        server/logic/SearchEngine.cpp
#        server/logic/SearchEngine.h
#        server/logic/TrieNode.cpp
#        server/logic/TrieNode.h
#        server/gui/ServerDashboard.cpp
#        server/gui/ServerDashboard.h
#        server/gui/ServerLogModel.cpp
#        server/gui/ServerLogModel.h
        ${SHARED_SOURCES}
        ${SHARED_HEADERS}
)

target_include_directories(Ketab_e_Man_Server PRIVATE src/shared)
target_link_libraries(Ketab_e_Man_Server Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
target_compile_definitions(Ketab_e_Man_Server PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
set_target_properties(Ketab_e_Man_Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/server")

add_executable(Ketab_e_Man_Client
        #client/main.cpp
#        client/network/ClientNetwork.cpp
#        client/network/ClientNetwork.h
        ${SHARED_HEADERS}
)

target_include_directories(Ketab_e_Man_Client PRIVATE src/shared)
target_link_libraries(Ketab_e_Man_Client Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
set_target_properties(Ketab_e_Man_Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/client")

if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

    foreach(TARGET_NAME Ketab_e_Man_Server Ketab_e_Man_Client)
        if (WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${WINDEPLOYQT_EXECUTABLE}
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:${TARGET_NAME}>"
            )
        endif()
    endforeach()
endif()
