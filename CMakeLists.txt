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
        shared/CartItem.cpp
        shared/Notification.cpp
        shared/Wallet.cpp
        shared/Transaction.cpp
        shared/Purchase.cpp
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
        shared/CartItem.h
        shared/Notification.h
        shared/Wallet.h
        shared/Transaction.h
        shared/Purchase.h
)


if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

    foreach(TARGET_NAME)
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
