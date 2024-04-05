#ifndef __COMWISE__ROBOT_VIEWER__CAMERA_WIDGET__H__
#define __COMWISE__ROBOT_VIEWER__CAMERA_WIDGET__H__

#include <memory>
#include <map>
#include <QWidget>
#include "camera_data.h"

namespace Ui {
class CameraWidget;
}

class CameraWindow;

class CameraWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraWidget(const QString &file, QWidget *parent = 0);
    ~CameraWidget();

    bool setCameraCmd(int cmd, std::string &response);
    bool setCaptureDir(const std::string &dir);

signals:
    bool subscribeImage(int, const QString&, bool);

public slots:
    void setCameraInfo(int, const CameraData&);
    bool getCameraInfo(int, CameraData&);

    void showImage(int, const QImage&);
    void showTopicList(const QVector<QString>&);

private slots:
    void currentIndexChanged(int, CameraWindow*);
    void buttonClicked(int, CameraWindow*);
    void on_addCameraPushButton_clicked();
    void on_delCameraPushButton_clicked();
    void on_tabWidget_tabCloseRequested(int index);

private:
    void init();
    void deinit();

    void removeTab(int index);

    void updateForm(int index, CameraWindow *wnd);

    void updateSubscribe(int frame = -1, const QString &topic = "");

private:
    Ui::CameraWidget *ui{nullptr};

    std::map<int, CameraData> cam_val_;
    std::map<int, CameraWindow*> cam_wnd_;

    QVector<QString> topics_;

};

#endif //__COMWISE__ROBOT_VIEWER__CAMERA_WIDGET__H__
