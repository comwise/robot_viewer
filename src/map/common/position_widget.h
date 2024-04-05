#ifndef __COMWISE__ROBOT_VIEWER__POSITION_WIDGET__H__
#define __COMWISE__ROBOT_VIEWER__POSITION_WIDGET__H__

#include <QWidget>

namespace Ui {
class PositionWidget;
}

class PositionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PositionWidget(const QPointF &pt = QPointF(), QWidget *parent = 0);
    ~PositionWidget();

    void setPos(const QPointF &pt);
    QPointF getPos();

private:
    Ui::PositionWidget *ui;
};

#endif // __COMWISE__ROBOT_VIEWER__POSITION_WIDGET__H__
