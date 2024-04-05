#include "map/RobotViewerMainWind.h"
#include <QApplication>
#include <QTranslator>
#include "log/log.h"

int main(int argc, char **argv)
{
    int quit_code = 0;
    bool is_exit_ok = false;
    
    std::string cfg_file = "$/robot_viewer.json";
    std::shared_ptr<RobotViewerMainWind> node {nullptr};

    try {
        LOG_STREAM_INFO << " init robot_viewer node ...";

        QApplication app(argc, argv);
        QTranslator translator(0);
        translator.load("qt_zh_CN",".");
        app.installTranslator(&translator);

        node = std::make_shared<RobotViewerMainWind>(cfg_file.c_str());
        node->show();

        quit_code =  app.exec();
        is_exit_ok = true;

    } catch (const std::exception &e) {
        LOG_STREAM_ERROR << "robot_viewer run exception, " << e.what();
        quit_code = -2;
        is_exit_ok = true;
    }
    return quit_code;
}
