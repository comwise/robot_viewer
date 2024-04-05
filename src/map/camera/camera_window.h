#ifndef __COMWISE__ROBOT_VIEWER__CAMERA_WINDOW__H__
#define __COMWISE__ROBOT_VIEWER__CAMERA_WINDOW__H__

#include <memory>
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include "camera_data.h"

namespace Ui {
class CameraWindow;
}

class CameraWindow : public QWidget
{
    Q_OBJECT
public:
    enum WidgetId {
        kButtonNone = 0,
        kButtonConnect,
        kButtonPlay,
        kButtonCapture,
        kButtonRecord,
        kButtonFocusAuto,
        kButtonFocusIn,
        kButtonFocusOut,
        kButtonZoomIn,
        kButtonZoomOut,
        kComboboxCameraType = 100,
        kComboboxCameraTopic,
    };

    enum CameraType {
        kCameraReasense = 0,
        kCameraHik,
        kCameraDahua,
        kCameraDali,
    };

public:
    explicit CameraWindow(QWidget *parent = 0);
    virtual ~CameraWindow();

    void setButtonText(WidgetId id, const QString &text);
    QAbstractButton* getButton(WidgetId id) const;

    void setComboBoxValue(WidgetId id, int index);
    QComboBox* getComboBox(WidgetId id) const;

    void setImage(const QImage &img);

signals:
    void clicked(int, CameraWindow*);
    void currentIndexChanged(int, CameraWindow*);

public slots:
    void setCameraInfo(const CameraData&);
    bool getCameraInfo(CameraData&);

private slots:
    void on_connectPushButton_clicked();
    void on_playPushButton_clicked();
    void on_catpurePushButton_clicked();
    void on_recordPushButton_clicked();

    void on_focusInPushButton_clicked();
    void on_focusOutPushButton_clicked();
    void on_focusAutoPushButton_clicked();
    void on_zoomInpushButton_clicked();
    void on_zoomOutPushButton_clicked();

    void on_typeComboBox_currentIndexChanged(int index);
    void on_topicComboBox_currentIndexChanged(int index);

private:
    void init();
    void deinit();

private:
    Ui::CameraWindow *ui{nullptr};

    // set value
    int focus_val_{0};
    int zoom_val_{0};

    // feedback value
    int curr_focus_val_{0};
    int curr_zoom_val_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__CAMERA_WINDOW__H__
