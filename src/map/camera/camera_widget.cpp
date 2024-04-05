#include "camera/camera_widget.h"
#include "ui_camera_widget.h"
#include <QMessageBox>
#include <QDebug>
#include <thread>
#include "camera/camera_window.h"

namespace {
static const char* kConnectText[] = { "登录", "注销" };
static const char* kPlayText[]    = { "播放", "暂停" };
static const char* kCaptureText[] = { "截图", "截图" };
static const char* kRecordText[]  = { "开始录制", "暂停录制" };
}

CameraWidget::CameraWidget(const QString &file, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CameraWidget)
{
    init();
}

CameraWidget::~CameraWidget()
{
    deinit();
    delete ui;
}

void CameraWidget::init()
{
    ui->setupUi(this);
    qRegisterMetaType<CameraData>("CameraData");
    qRegisterMetaType<QImage>("QImage");
    qRegisterMetaType<QVector<QString>>("QVector<QString>");
}

void CameraWidget::deinit()
{
    updateSubscribe(-1, "");
}

void CameraWidget::on_addCameraPushButton_clicked()
{
    int cnt = ui->tabWidget->count();
    QString info = QString("camera%1").arg(cnt);
    CameraWindow *new_tab = new CameraWindow(this);
    if(new_tab) {
        ui->tabWidget->insertTab(cnt, new_tab, info);
        cam_wnd_[cnt] = new_tab;
        connect(new_tab, &CameraWindow::clicked, this, &CameraWidget::buttonClicked);
        connect(new_tab, &CameraWindow::currentIndexChanged, this, &CameraWidget::currentIndexChanged);
    }
}

void CameraWidget::on_delCameraPushButton_clicked()
{
    int index = ui->tabWidget->currentIndex();
    removeTab(index);
}

void CameraWidget::on_tabWidget_tabCloseRequested(int index)
{
    removeTab(index);
}

void CameraWidget::removeTab(int index)
{
    if(index >= 0) {
        emit subscribeImage(index, cam_val_[index].topic.c_str(), false);
        auto iter = cam_wnd_.find(index);
        if(iter != cam_wnd_.end()) {
            delete iter->second;
            cam_wnd_.erase(iter);
        }
        cam_val_[index] = CameraData();
        ui->tabWidget->removeTab(index);
    }
}

bool CameraWidget::setCameraCmd(int cmd, std::string &response)
{
    //todo
    return false;
}

bool CameraWidget::setCaptureDir(const std::string &dir)
{
    //todo
    return false;
}

void CameraWidget::setCameraInfo(int, const CameraData &data)
{
    //todo
}

bool CameraWidget::getCameraInfo(int, CameraData &data)
{
    //todo
    return false;
}

void CameraWidget::showImage(int frame, const QImage &img)
{
    if(cam_wnd_.find(frame) == cam_wnd_.end()) {
        return;
    }
    CameraWindow *wnd = cam_wnd_[frame];
    if(wnd) {
        if(cam_val_[frame].is_play) {
            wnd->setImage(img);
        }
    }
}

void CameraWidget::showTopicList(const QVector<QString> &topics)
{
    topics_ = topics;
    for(auto &wnd_item : cam_wnd_) {
        CameraWindow *wnd = wnd_item.second;
        if(nullptr == wnd) {
            continue;
        }
        auto cbx = wnd->getComboBox(CameraWindow::kComboboxCameraTopic);
        if(nullptr == cbx) {
            continue;
        }
        QString sel_item = cbx->currentText();
        cbx->clear();
        for (auto &item : topics) {
            cbx->addItem(item);
        }
        cbx->setCurrentText(sel_item);
    }
}

 void CameraWidget::currentIndexChanged(int id, CameraWindow *wnd)
 {
#if 0
    int frame = ui->tabWidget->currentIndex();
    if(frame < 0) {
        qDebug() << "current camera widget index is invalid";
        return;
    }

    CameraData& val = cam_val_[frame];

    switch (id)
    {
    case CameraWindow::kComboboxCameraType:
        {
            break;
        }
    case CameraWindow::kComboboxCameraTopic:
        {
            QString curr_topic;
            auto cbx = wnd->getComboBox(CameraWindow::kComboboxCameraTopic);
            if(cbx) {
               curr_topic = cbx->currentText();
            }
            for (auto &topic : topics_) {          
                if(topic != curr_topic) {
                    qDebug() << frame << topic << curr_topic;
                    emit subscribeImage(frame, topic, false);
                }
            }
            break;
        }
    default:
        return;
    }
#endif
 }

void CameraWidget::buttonClicked(int btn, CameraWindow *wnd)
{
    int frame = ui->tabWidget->currentIndex();
    if(frame < 0) {
        qDebug() << "current camera widget index is invalid";
        return;
    }

    CameraData& val = cam_val_[frame];
    //qDebug() << "camera:" << frame << ", button:" << btn;

    switch (btn)
    {
    case CameraWindow::kButtonConnect:
        {
            if (false == val.is_login) {
                val.is_play = false;
                val.is_record = false;
            }
            break;
        }
    case CameraWindow::kButtonPlay:
        {
            QString topic;
            auto cbx = wnd->getComboBox(CameraWindow::kComboboxCameraTopic);
            if(cbx) {
               topic = cbx->currentText();
            }
            emit subscribeImage(frame, val.topic.c_str(), false);
            bool is_sub = !val.is_play;
            bool is_ok = emit subscribeImage(frame, topic, is_sub);
            if(is_ok) {
                val.is_play = !val.is_play;
            } else {
                val.is_record = false;
            }
            val.topic = topic.toStdString();
            break;
        }
    case CameraWindow::kButtonCapture:
        {
            //todo
            break;
        }
    case CameraWindow::kButtonRecord:
        {
            //todo
            break;
        }
    default:
        return;
    }

    updateForm(frame, wnd);
}

void CameraWidget::updateForm(int frame, CameraWindow *wnd)
{
    if (nullptr == wnd)
        return;

    QString showText;
    CameraData& val = cam_val_[frame];

    showText = kConnectText[val.is_login];
    wnd->setButtonText(CameraWindow::kButtonConnect, showText);

    showText = kPlayText[val.is_play];
    wnd->setButtonText(CameraWindow::kButtonPlay, showText);

    showText = kCaptureText[true];
    wnd->setButtonText(CameraWindow::kButtonCapture, showText);

    showText = kRecordText[val.is_record];
    wnd->setButtonText(CameraWindow::kButtonRecord, showText);
}

void CameraWidget::updateSubscribe(int curr_frame, const QString &curr_topic)
{
    for (auto &item : cam_wnd_) {
        int frame = item.first;
        if(curr_frame > -1 && curr_frame != frame) {
            continue;
        }
        for (auto &topic : topics_) {
            if (curr_topic.isEmpty() || (!curr_topic.isEmpty() && curr_topic != topic)) {
                //qDebug() << curr_topic << frame << topic << false;
                emit subscribeImage(frame, topic, false);
            }
        }
    }
}