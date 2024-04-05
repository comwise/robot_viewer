#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_UPLOAD_FILE__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_UPLOAD_FILE__H__

#include <string>
#include <QDialog>

namespace Ui {
class DialogUploadFile;
}

class QMenu;

class DialogUploadFile : public QDialog
{
    Q_OBJECT

    enum UploadProtocol {
        UploadShare,
        UploadFtp,
    };

    enum UploadColumn {
        UploadPath,
        UploadStatus,
    };

public:
    explicit DialogUploadFile(const QString &file, QWidget *parent = 0);
    ~DialogUploadFile();

signals:
    void upload_notify(int, int);

private slots:
    void on_pushButtonUpload_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonDefault_clicked();
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_comboBoxProtocol_currentIndexChanged(int index);

    void add_slot();
    void del_slot();

    void upload_refresh(int, int);

private:
    void init();

    void create_form();
    void create_menu();

    void fill_default();

    void clear_row();
    void add_row(const QString &file);

    void upload_thread(const std::vector<std::string>&, int type = UploadShare);

    bool upload_file(const std::string &file, int type = UploadShare);

    bool is_valid();

    void read_setting();
    void write_setting();

private:
    Ui::DialogUploadFile *ui{nullptr};

    QMenu *upload_menu_{nullptr};

    QString config_file_;
    QStringList upload_file_;
    QString default_url_;
    QString default_auth_;
    QString curr_url_;
    QString curr_auth_;
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_UPLOAD_FILE__H__
