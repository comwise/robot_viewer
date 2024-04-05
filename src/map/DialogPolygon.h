#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_POLYGON__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_POLYGON__H__

#include <QDialog>
#include "RobotViewerData.h"

namespace Ui {
class DialogPolygon;
}

class DialogPolygon : public QDialog
{
    Q_OBJECT
public:
    explicit DialogPolygon(bool enable_ok = true, QWidget *parent = 0);
    explicit DialogPolygon(const PolygonData &data, QWidget *parent = 0);
    ~DialogPolygon();

public slots:
    void setData(const PolygonData &);
    PolygonData getData() const;
    bool getData(PolygonData &data);

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();

private:
    void init();
    void remove();

    void msgWarn(const QString &title, const QString &info);

private:
    Ui::DialogPolygon *ui{nullptr};
    PolygonData data_;
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_POLYGON__H__
