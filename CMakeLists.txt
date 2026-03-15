cmake_minimum_required(VERSION 3.21)

project(calc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 QUIET COMPONENTS Widgets)
if(TARGET Qt6::Widgets)
  set(QT_PACKAGE Qt6)
else()
  find_package(Qt5 QUIET COMPONENTS Widgets)
  if(TARGET Qt5::Widgets)
    set(QT_PACKAGE Qt5)
  endif()
endif()

add_library(calculator_engine
  src/calculator_engine.cpp
  src/calculator_engine.h
)

target_include_directories(calculator_engine PUBLIC src)

if(QT_PACKAGE)
  add_executable(calc
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
  )

  set_target_properties(calc PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
  )

  target_link_libraries(calc PRIVATE calculator_engine ${QT_PACKAGE}::Widgets)
else()
  message(WARNING "Qt Widgets development files were not found. GUI sources are present, but only engine tests will be built in this environment.")
endif()

enable_testing()

add_executable(calculator_engine_tests
  tests/calculator_engine_tests.cpp
)

target_link_libraries(calculator_engine_tests PRIVATE calculator_engine)

add_test(NAME calculator_engine_tests COMMAND calculator_engine_tests)
