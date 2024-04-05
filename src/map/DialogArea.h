#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_AREA__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_AREA__H__

#include <QDialog>
#include "RobotViewerData.h"

namespace Ui {
class DialogArea;
}

class DialogArea : public QDialog
{
    Q_OBJECT

public:
    explicit DialogArea(QDialog *parent = nullptr);
    explicit DialogArea(const AreaData &area, QDialog *parent = nullptr);
    ~DialogArea();

public slots:
    void setData(const AreaData &);
    AreaData getData() const;

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();

private:
    void init();

private:
    Ui::DialogArea *ui{nullptr};
    AreaData data_;
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_TRAFFIC_AREA__H__
