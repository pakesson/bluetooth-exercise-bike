cmake_minimum_required(VERSION 3.14)

project(ftms-simulator-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Bluetooth REQUIRED)

set(SOURCES
  main.cpp
  ftmssimulator.cpp
  fitnessmachineservice.cpp
)
set(HEADERS
  ftmssimulator.h
  fitnessmachineservice.h
)

add_executable(ftms-simulator-qt
  ${SOURCES}
  ${HEADERS}
)
target_link_libraries(ftms-simulator-qt Qt::Core Qt::Bluetooth)
