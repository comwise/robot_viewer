#ifndef __COMWISE__ROBOT_VIEWER__DOCK_SIMULATOR__H__
#define __COMWISE__ROBOT_VIEWER__DOCK_SIMULATOR__H__

#include <memory>
#include <QDialog>

class QAction;
class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class RobotViewerSimulator;

class DockSimulator : public QDialog
{
    typedef std::shared_ptr<RobotViewerSimulator> RobotViewerSimulatorPtr;
    Q_OBJECT
public:
    DockSimulator(QWidget* widget = 0);
    ~DockSimulator();

signals:
    void notify(const QString& info);

public slots:
    void slotStartSim();
    void slotSaveSim();
    void slotShowInfo(const QString& info);

private:
    void initCodec();
    void initSetting();
    void initForm();
    void initOthers();

    void setInitPos(double x, double y, double angle);
    void getInitPos();

    void readSetting();
    void writeSetting();

    void buttonClicked(bool &, QPushButton *);

private:
    QLineEdit *leInitPos_[3];
    QPushButton *btnSaveSim_{nullptr};
    QPushButton *btnStartSim_{nullptr};

    bool isStartSim_{false};

    double initX_{0.0};
    double initY_{0.0};
    double initA_{0.0};
    int simPeriod_{0};

    
    RobotViewerSimulatorPtr simulate_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__DOCK_SIMULATOR__H__
