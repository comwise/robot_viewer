#include "camera/camera_window.h"
#include "ui_camera_window.h"
#include <QImage>
#include <QDebug>
#include <thread>

CameraWindow::CameraWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CameraWindow)
    , focus_val_(0)
    , zoom_val_(0)
    , curr_focus_val_(0)
    , curr_zoom_val_(0)
{
    init();
}

CameraWindow::~CameraWindow()
{
    deinit();
    delete ui;
}

void CameraWindow::init()
{
    ui->setupUi(this);
    setMinimumWidth(400);
    qRegisterMetaType<CameraData>("CameraData");
}

void CameraWindow::deinit()
{

}

void CameraWindow::setButtonText(WidgetId id, const QString &text)
{
    QAbstractButton* btn = getButton(id);
    if (nullptr != btn) {
        btn->setText(text);
    }
}

QAbstractButton* CameraWindow::getButton(WidgetId id) const
{
    QAbstractButton* btn = nullptr;
    switch(id)
    {
    case kButtonConnect:
        btn = ui->connectPushButton;
        break;
    case kButtonPlay:
        btn = ui->playPushButton;
        break;
    case kButtonCapture:
        btn = ui->catpurePushButton;
        break;
    case kButtonRecord:
        btn = ui->recordPushButton;
        break;
    case kButtonFocusAuto:
        btn = ui->focusAutoPushButton;
        break;
    case kButtonFocusIn:
        btn = ui->focusInPushButton;
        break;
    case kButtonFocusOut:
        btn = ui->focusOutPushButton;
        break;
    case kButtonZoomIn:
        btn = ui->zoomInpushButton;
        break;
    case kButtonZoomOut:
        btn = ui->zoomOutPushButton;
        break;
    default:
        break;
    }
    return btn;
}

void CameraWindow::setComboBoxValue(WidgetId id, int index)
{
    QComboBox* box = getComboBox(id);
    if(nullptr != box) {
        box->setCurrentIndex(index);
    }
}

QComboBox* CameraWindow::getComboBox(WidgetId id) const
{
    QComboBox *box = nullptr;
    switch (id) {
    case kComboboxCameraType:
        box = ui->typeComboBox;
        break;
    case kComboboxCameraTopic:
        box = ui->topicComboBox;
        break;
    default:
        break;
    }
    return box;
}

void CameraWindow::setCameraInfo(const CameraData &data)
{
    curr_focus_val_ = data.focus_val;
    curr_zoom_val_ = data.zoom_val;
}

bool CameraWindow::getCameraInfo(CameraData &data)
{
    data.focus_val = curr_focus_val_;
    data.zoom_val = curr_zoom_val_;
    return true;
}

void CameraWindow::setImage(const QImage &image)
{
    if(nullptr == ui) {
        return;
    }
    ui->hdLabel->setPixmap(QPixmap::fromImage(image).scaled(
        ui->hdLabel->width(), ui->hdLabel->height()));
}

void CameraWindow::on_connectPushButton_clicked()
{
    emit clicked(kButtonConnect, this);
}

void CameraWindow::on_playPushButton_clicked()
{
    emit clicked(kButtonPlay, this);
}

void CameraWindow::on_catpurePushButton_clicked()
{
    emit clicked(kButtonCapture, this);
}

void CameraWindow::on_recordPushButton_clicked()
{
    emit clicked(kButtonRecord, this);
}

void CameraWindow::on_focusInPushButton_clicked()
{
    emit clicked(kButtonFocusIn, this);
}

void CameraWindow::on_focusOutPushButton_clicked()
{
    emit clicked(kButtonFocusOut, this);
}

void CameraWindow::on_focusAutoPushButton_clicked()
{
    emit clicked(kButtonFocusAuto, this);
}

void CameraWindow::on_zoomInpushButton_clicked()
{
    emit clicked(kButtonZoomIn, this);
}

void CameraWindow::on_zoomOutPushButton_clicked()
{
    emit clicked(kButtonZoomOut, this);
}

void CameraWindow::on_typeComboBox_currentIndexChanged(int index)
{
    emit currentIndexChanged(kComboboxCameraType, this);
}

void CameraWindow::on_topicComboBox_currentIndexChanged(int index)
{
    emit currentIndexChanged(kComboboxCameraTopic, this);
}


