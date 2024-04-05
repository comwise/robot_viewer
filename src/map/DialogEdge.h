#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_EDGE__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_EDGE__H__

#include <QDialog>
#include "RobotViewerData.h"

namespace Ui {
    class DialogEdge;
}

class ItemEdge;

class DialogEdge : public QDialog
{
    Q_OBJECT
    enum EdgeColumn {
        EdgeColEnable,
        EdgeColDirection,
        EdgeColSlow,
        EdgeColStop
    };

public:
    explicit DialogEdge(ItemEdge *edge);
    ~DialogEdge();

    void setEdgeAttribute(const EdgeData &edgedata);
    void getEdgeAttribute(EdgeData &edgedata) const;

private slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();

private:
    void setSensorAttribute(const EdgeData &edgedata);
    void getSensorAttribute(EdgeData &edgedata) const;

private:
    Ui::DialogEdge *ui_{nullptr};
    ItemEdge *item_edge_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_EDGE__H__
