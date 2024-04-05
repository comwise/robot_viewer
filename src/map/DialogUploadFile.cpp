#include "DialogUploadFile.h"
#include "ui_DialogUploadFile.h"
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore/QtCore>
#include <deque>
#include <thread>
#include "RobotViewerLog.h"

static const char *UploadRet[] = {"false", "true"};
static const char *UploadShow[] = {"×", "√"};

DialogUploadFile::DialogUploadFile(const QString &file, QWidget *parent)
    : config_file_(file)
    , QDialog(parent)
    , ui(new Ui::DialogUploadFile)
    , upload_menu_(new QMenu)
{
    init();
}

DialogUploadFile::~DialogUploadFile()
{
    // write_setting();
    delete ui;
}

void DialogUploadFile::init()
{
    create_form();
    create_menu();
    fill_default();
}

void DialogUploadFile::create_form()
{
    ui->setupUi(this);
    ui->listWidget->setColumnWidth(0, 460);
    ui->listWidget->setColumnWidth(1, 20);
    connect(this, SIGNAL(upload_notify(int, int)), this, SLOT(upload_refresh(int, int)));
}

void DialogUploadFile::create_menu()
{
    QAction *add = new QAction(tr("Add"), this);
    add->setCheckable(false);
    connect(add, SIGNAL(triggered()), this, SLOT(add_slot()));
    QAction *del = new QAction(tr("Delete"), this);
    del->setCheckable(false);
    connect(del, SIGNAL(triggered()), this, SLOT(del_slot()));

    upload_menu_->addAction(add);
    upload_menu_->addAction(del);
}

void DialogUploadFile::fill_default()
{
    read_setting();

    ui->lineEditUploadUrl->setText(default_url_);
    ui->lineEditUploadAuth->setText(default_auth_);
    clear_row();
    foreach (QString item, upload_file_) {
        QString file = QCoreApplication::applicationDirPath() + "/" + item;
        add_row(file);
    }
}

void DialogUploadFile::add_row(const QString &file)
{
    QTableWidget *lst = ui->listWidget;
    int row = lst->rowCount();
    lst->setRowCount(row + 1);
    lst->setItem(row, UploadPath, new QTableWidgetItem(file));
    lst->setItem(row, UploadStatus, new QTableWidgetItem(""));
}

void DialogUploadFile::clear_row()
{
    ui->listWidget->clearContents();
    ui->listWidget->setRowCount(0);
}

void DialogUploadFile::read_setting()
{
    QSettings settings(config_file_, QSettings::IniFormat);
    settings.beginGroup("Defalut_Parameter");
    QString upload_file = settings.value("upload_file", "").toString();
    upload_file_ = upload_file.split(" ");
    default_url_ = settings.value("upload_url", "").toString();
    default_auth_ = settings.value("upload_auth", "").toString();
    settings.endGroup();

    curr_url_ = default_url_;
    curr_auth_ = default_auth_;
}

void DialogUploadFile::write_setting()
{
    QString upload_file;
    int row = ui->listWidget->rowCount();
    for (int i = 0; i < row; i++) {
        QTableWidgetItem *item = ui->listWidget->item(i, UploadPath);
        if (NULL == item)
            continue;

        std::string text = item->text().toStdString();
        std::size_t index = text.find_last_of("/");
        if (index < 0 || index >= text.length())
            continue;

        upload_file += text.c_str();
        if (i != row - 1)
            upload_file += "";
    }

    QSettings settings(config_file_, QSettings::IniFormat);
    settings.beginGroup("Defalut_Parameter");
    settings.setValue("upload_url", curr_url_);
    settings.setValue("upload_auth", curr_auth_);
    settings.setValue("upload_file", "");
    settings.endGroup();
}

bool DialogUploadFile::is_valid()
{
    return !ui->lineEditUploadUrl->text().isEmpty() && 
           !ui->lineEditUploadAuth->text().isEmpty() && 
           (ui->listWidget->rowCount() > 0);
}

void DialogUploadFile::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    if (upload_menu_)
        upload_menu_->exec(QCursor::pos());
}

void DialogUploadFile::add_slot()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open File"), "./", tr("Config Files (*.*)"));
    if (!fileName.isEmpty()) {
        add_row(fileName);
    }
}

void DialogUploadFile::del_slot()
{
    QTableWidget *wgt = ui->listWidget;
    QList<QTableWidgetItem *> items;
    while (!(items = wgt->selectedItems()).empty()) {
        int row = wgt->row(items.at(0));
        wgt->removeRow(row);
        items.clear();
    }
}

void DialogUploadFile::on_pushButtonUpload_clicked()
{
    if (!is_valid()) {
        QMessageBox::warning(this, tr("info"), tr("the field is empty!"));
        return;
    }
    curr_url_ = ui->lineEditUploadUrl->text();
    curr_auth_ = ui->lineEditUploadAuth->text();

    std::vector<std::string> lst;
    int row = ui->listWidget->rowCount();
    for (int i = 0; i < row; i++) {
        QTableWidgetItem *item = ui->listWidget->item(i, UploadPath);
        if (item) {
            lst.push_back(item->text().toStdString());
        }
        emit upload_notify(i, 3);
    }

    int type = ui->comboBoxProtocol->currentIndex();

    std::thread thr(std::bind(&DialogUploadFile::upload_thread, this, lst, type));
}

void DialogUploadFile::on_pushButtonClear_clicked()
{
    ui->lineEditUploadUrl->setText("");
    ui->lineEditUploadAuth->setText("");
    clear_row();
}

void DialogUploadFile::on_pushButtonDefault_clicked()
{
    fill_default();
}

void DialogUploadFile::on_comboBoxProtocol_currentIndexChanged(int index)
{
    switch (index)
    {
    case UploadShare:
        ui->lineEditUploadAuth->setEnabled(false);
        break;
    case UploadFtp:
        ui->lineEditUploadAuth->setEnabled(true);
        break;
    default:
        break;
    }
}

void DialogUploadFile::upload_thread(const std::vector<std::string> &lst, int type)
{
    for (std::size_t i = 0; i < lst.size(); i++) {
        std::string file(lst[i]);
        bool ret = upload_file(file, type);
        emit upload_notify(i, ret);
        TRACE_INFO("upload file %s, file = %s\n", UploadRet[ret], file.c_str());
    }
}

bool DialogUploadFile::upload_file(const std::string &file, int type)
{
    if (file.empty()) {
        return false;
    }

    std::size_t index = file.find_last_of("/");
    if (index < 0 || index >= file.length()) {
        return false;
    }
#if 0
    FTP_STATE state = FTP_UPLOAD_FAILED;
    std::string uploadfile = file;
    std::string uploadurl = curr_url_.toStdString() + "/" + file.substr(index+1);
    std::string uploadauth = curr_auth_.toStdString();

    if (type == UploadFtp) {
        uploadurl = "ftp:" + uploadurl;
    } else if(type == UploadShare) {
        uploadurl = "file://" + uploadurl;
    }

    try {
        FTP_OPT opt;
        opt.user_key = (char*)uploadauth.c_str();
        opt.file = (char*)uploadfile.c_str();
        opt.url = (char*)uploadurl.c_str();

        state = (type == UploadFtp) ? ftp_upload(opt) : file_upload(opt);
        if(FTP_UPLOAD_SUCCESS == state) {
            TRACE_INFO("Upload file success, file = %s\n", opt.file);
        } else {
            TRACE_ERROR("Upload file failed, file = %s\n", opt.file);
        }
    } catch (...) {
        TRACE_ERROR("Upload file exception, file = %s\n", uploadfile.c_str());
        state = FTP_UPLOAD_FAILED;
    }

    return FTP_UPLOAD_SUCCESS == state;
#else
    return true;
#endif
}

void DialogUploadFile::upload_refresh(int row, int result)
{
    QTableWidgetItem *item = ui->listWidget->item(row, UploadStatus);
    QString text = (result <= 1) ? UploadShow[result] : " ";
    if (item) {
        item->setText(text);
    }
}
