#-------------------------------------------------
#
# Project created by QtCreator 2024-03-26T16:16:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RobotViewer
TEMPLATE = app

SOURCES += main.cpp\
    common/custom_widget.cpp \
    common/position_widget.cpp \
    common/dash_board.cpp \
    common/free_tabwidget.cpp \
    common/free_widget.cpp \
    common/json_readerwriter.cpp \
    camera/camera_window.cpp \
    camera/camera_widget.cpp \
    ItemAgvPos.cpp \
    ItemControlPoint.cpp \
    ItemInitPos.cpp \
    ItemAgv.cpp \
    ItemMap.cpp \
    ItemReflector.cpp \
    ItemNode.cpp \
    ItemEdge.cpp \
    ItemRouter.cpp \
    ItemSite.cpp \
    ItemArea.cpp \
    ItemPolygon.cpp \
    DialogNode.cpp \
    DialogSite.cpp \
    DialogEdge.cpp \
    DialogArea.cpp \
    DialogPolygon.cpp \
    DialogForkControl.cpp \
    DialogFind.cpp \
    DialogUploadFile.cpp \
    DockEdge.cpp \
    DockSite.cpp \
    DockSimulator.cpp \
    ManageAgvItem.cpp \
    ManageAreaItem.cpp \
    ManageEdgeItem.cpp \
    ManageLaserItem.cpp \
    ManageMapItem.cpp \
    ManageNodeItem.cpp \
    ManageRouteItem.cpp \
    ManageSiteItem.cpp \
    ManagePolygonItem.cpp \
    RobotViewerConfig.cpp \
    RobotViewerScene.cpp \
    RobotViewerView.cpp \
    RobotViewerMainWind.cpp \
    RobotViewerSimulator.cpp \
    RobotViewerStick.cpp \
    RobotViewerNavigation.cpp

HEADERS  += \
    common/algorithm.h \
    common/msg.h \
    common/count_time.hpp \
    common/singleton.h \
    common/json_readerwriter.h \
    common/custom_widget.h \
    common/position_widget.h \
    common/dash_board.h \
    common/free_tabwidget.h \
    common/free_widget.h \
    camera/camera_data.h \
    camera/camera_window.h \
    camera/camera_widget.h \
    ItemBase.h \
    ItemNode.h \
    ItemEdge.h \
    ItemRouter.h \
    ItemSite.h \
    ItemArea.h \
    ItemPolygon.h \
    ItemAgvPos.h \
    ItemAgv.h \
    ItemControlPoint.h \
    ItemInitPos.h \
    ItemMap.h \
    ItemReflector.h \
    DialogNode.h \
    DialogEdge.h \
    DialogSite.h \
    DialogArea.h \
    DialogForkControl.h \
    DialogFind.h \
    DialogUploadFile.h \
    DockSimulator.h \
    DockEdge.h \
    DockSite.h \
    ManageItemBase.h \
    ManageNodeItem.h \
    ManageEdgeItem.h \
    ManageRouteItem.h \
    ManageSiteItem.h \
    ManageAreaItem.h \
    ManagePolygonItem.h \
    ManageAgvItem.h \
    ManageMapItem.h \
    ManageLaserItem.h \
    RobotViewerLog.h \
    RobotViewerData.h \
    RobotViewerConfig.h \
    RobotViewerView.h \
    RobotViewerScene.h \
    RobotViewerMainWind.h \
    RobotViewerSimulator.h \
    RobotViewerStick.h \
    RobotViewerNavigation.h \
    RobotViewer.rc

FORMS    += DialogNode.ui \
    DialogEdge.ui \
    DialogSite.ui \
    DialogArea.ui \
    DialogPolygon.ui \
    DialogFind.ui \
    DialogUploadFile.ui \
    common/position_widget.ui \
    common/free_tabwidget.ui \
    camera/camera_widget.ui \
    camera/camera_window.ui

DISTFILES += \
    robot_viewer.yaml \
    robot_viewer.json \
    CMakeLists.txt 

RESOURCES += \
    RobotViewer.qrc
