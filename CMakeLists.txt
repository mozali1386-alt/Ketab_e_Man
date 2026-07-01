cmake_minimum_required(VERSION 3.21)
project(Ketab_e_Man LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)

# Portable Qt lookup:
# Pass one of these at configure time on each machine:
#   -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/<compiler>"
# or
#   -DQt6_DIR="/path/to/Qt/6.x.x/<compiler>/lib/cmake/Qt6"
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# --- libbcrypt discovery ---
# Expected location in repo: additnal/lib/libbcrypt or aditnal/lib/libbcrypt
set(LIBBCRYPT_CANDIDATES
    "${CMAKE_CURRENT_SOURCE_DIR}/additnal/lib/libbcrypt"
    "${CMAKE_CURRENT_SOURCE_DIR}/aditnal/lib/libbcrypt"
    "${CMAKE_CURRENT_SOURCE_DIR}/additnal/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/aditnal/lib"
)

set(LIBBCRYPT_DIR "")
foreach(p IN LISTS LIBBCRYPT_CANDIDATES)
    if(EXISTS "${p}/CMakeLists.txt")
        set(LIBBCRYPT_DIR "${p}")
        break()
    endif()
endforeach()

if(NOT LIBBCRYPT_DIR)
    message(FATAL_ERROR
        "libbcrypt not found. Put it in additnal/lib/libbcrypt (or aditnal/lib/libbcrypt).")
endif()

add_subdirectory("${LIBBCRYPT_DIR}" _deps_libbcrypt_build)

qt_add_executable(Ketab_e_Man
    WIN32 MACOSX_BUNDLE
    main.cpp
    loginwindow.cpp
    loginwindow.h
    loginwindow.ui
)

target_link_libraries(Ketab_e_Man
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

# Link libbcrypt target (target name can vary by upstream CMakeLists)
if(TARGET bcrypt)
    target_link_libraries(Ketab_e_Man PRIVATE bcrypt)
elseif(TARGET libbcrypt)
    target_link_libraries(Ketab_e_Man PRIVATE libbcrypt)
elseif(TARGET BCrypt)
    target_link_libraries(Ketab_e_Man PRIVATE BCrypt)
else()
    message(FATAL_ERROR
        "Could not find bcrypt target from libbcrypt. Check add_library(...) name in libbcrypt/CMakeLists.txt")
endif()

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
