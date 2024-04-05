#ifndef __COMWISE__ROBOT_VIEWER__DOCK_EDGE__H__
#define __COMWISE__ROBOT_VIEWER__DOCK_EDGE__H__

#include <QTableWidget>
#include "RobotViewerData.h"

class RobotViewerScene;

QT_BEGIN_NAMESPACE
class QMouseEvent;
QT_END_NAMESPACE

class DockEdge : public QTableWidget
{
    Q_OBJECT
public:
    DockEdge(QWidget *parent = 0);

    void SetScene(RobotViewerScene *scene);

    void AddEdge(const EdgeData &edge_data);
    void RemoveEdge(const EdgeData &edge_data);
    void UpdateEdge(const EdgeData &edge_data);

public slots:
    void SortEdge(int);
    void UpdateData(const EdgeData &edge_data, int);
    void SelectEdgeChanged(QTableWidgetItem *);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void InitForm();
    void UpdateRow(int row, const EdgeData &edge_data);
    QString GetEdgeDesc(const EdgeData &edge_data);
    int FindEdgeRow(int edge_id);

private:
    RobotViewerScene *scene_{nullptr};
    int sort_type_{0};
};

#endif //__COMWISE__ROBOT_VIEWER__DOCK_EDGE__H__
