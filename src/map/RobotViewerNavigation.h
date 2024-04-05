#ifndef __COMWISE__ROBOT_VIEWER__NAVIGATION__H__
#define __COMWISE__ROBOT_VIEWER__NAVIGATION__H__

#include <QObject>
#include <QVector>
#include <memory>
#include <thread>
#include <mutex>
#include "RobotViewerData.h"

class GNavigationUPL;
class BGLTopology;

class RobotViewerNavigation : public QObject
{
public:
    enum TaskErrorType {
        NaviagtionOK,
        NaviagtionStartError,
        NaviagtionEndError,
        NaviagtionPathError,
        NaviagtionClose,
    };

    Q_OBJECT
public:
    RobotViewerNavigation();
    ~RobotViewerNavigation();

    bool LoadFile(const QString &file_name);
    void SetCurrentUpl(const UPL &current_upl);
    bool GotoDest(const UPL &dest_upl);
    void BindNavigationTaskFinish(bool finish);
    bool StartTarget(const std::vector<UPL> &dests);
    bool PauseTarget();
    bool ResumeTarget();
    bool CancelTarget();
    bool IsRun() { return is_run_; }

protected:
    bool ClientForNavigation(const std::vector<UPL> &dest_upl);
    GNavigationUPL UPL2GNavigationUPL(const UPL &upl);

signals:
    void TaskFinish(bool);
    void TaskRoute(QVector<UPL>);
    void TaskError(QVector<UPL>, int);

private:
    void MainLoop();
    bool ClientForNavigation(int status);

private:
    bool is_run_{false};
    bool naviagtion_finish_{true};
    bool cancel_{false};
    
    std::mutex mutex_;
    std::mutex upl_mutex_;
    UPL current_upl_;
    std::vector<UPL> dest_upls_;
    UPL dest_upl_;
    
    BGLTopology *bgl_topology_{nullptr};

    bool is_loop_{false};
    std::shared_ptr<std::thread> thread_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__NAVIGATION__H__
