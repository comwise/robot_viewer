cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-std=c++11 -g ${CMAKE_CXX_FLAGS}")

#### qt ####
set(ModuleList
  Core
  Gui
  Widgets
)
foreach(QtModule ${ModuleList})
  set(Qt5${QtModule}_DIR ${CMAKE_PREFIX_PATH}/lib/cmake/Qt5${QtModule})
  message("foreach-> Qt5${QtModule}_DIR : ${Qt5${QtModule}_DIR}")
  set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::${QtModule})
endforeach()
message("${QT_LIBRARIES}")
find_package(Qt5 REQUIRED ${ModuleList})

### map ###
file(GLOB_RECURSE QT_UIS RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR} *.ui
    ${CMAKE_CURRENT_SOURCE_DIR} common/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR} map/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR} camera/*.ui
)
#file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ui/*.ui)
file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qrc)
file(GLOB_RECURSE QT_MOC_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS include/*.hpp *.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

QT5_WRAP_CPP(QT_MOC ${QT_MOC_SRCS})
QT5_ADD_RESOURCES(QT_QSCSS_SRCS ${QT_RESOURCES})
QT5_WRAP_UI(QT_UIS_H ${QT_UIS})
file(GLOB_RECURSE QT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.c* common/*.c* camera/*.c* map/*.c*)

add_library(map 
  ${QT_SRCS}
  ${QT_UIS_H}
  ${QT_MOC}
  ${QT_QSCSS_SRCS}
)

target_link_libraries(map
  ${MAP_LIBS}
  ${QT_LIBRARIES}
  jsoncpp
  -lpthread
)

#add_executable(map_node main.cpp)
#target_link_libraries(map_node map)
