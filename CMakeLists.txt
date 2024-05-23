cmake_minimum_required(VERSION 3.5)

project(TaxiCmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TaxiCmake main.cpp)

include(GNUInstallDirs)
install(TARGETS TaxiCmake
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
