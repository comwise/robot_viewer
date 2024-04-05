#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_SITE__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_SITE__H__

#include <QDialog>
#include "RobotViewerData.h"

namespace Ui {
class DialogSite;
}

class DialogSite : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSite(QDialog *parent = nullptr);
    explicit DialogSite(const SiteData &area, QDialog *parent = nullptr);
    ~DialogSite();

public slots:
    void setData(const SiteData &);
    SiteData getData() const;

private slots:
    void on_btn_ok_clicked();
    void on_btn_cancel_clicked();

    void on_dsbx_posx_valueChanged(double arg1);
    void on_dsbx_posy_valueChanged(double arg1);

private:
    void init();

private:
    Ui::DialogSite *ui{nullptr};
    SiteData data_;

    bool pos_changed_{false};
    int posx_changed_{0};
    int posy_changed_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_SITE__H__
