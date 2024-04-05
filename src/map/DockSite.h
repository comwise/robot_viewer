#ifndef __COMWISE__ROBOT_VIEWER__DOCK_SITE__H__
#define __COMWISE__ROBOT_VIEWER__DOCK_SITE__H__

#include <QTableWidget>
#include "RobotViewerData.h"

class RobotViewerScene;
class QMouseEvent;

class DockSite : public QTableWidget
{
    Q_OBJECT
public:
    DockSite(QWidget *parent = 0);
    ~DockSite() {}

    void SetScene(RobotViewerScene *scene);
    
    void AddSite(const SiteData &site_data);
    void RemoveSite(const SiteData &site_data);
    void UpdateSite(const SiteData &site_data);

public slots:
    void SortSite(int);
    void UpdateData(const SiteData &site_data, int);
    void SelectSiteChanged(QTableWidgetItem *);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void InitForm();

    void UpdateRow(int row, const SiteData &site_data);
    QString GetSiteDesc(const SiteData &site_data);
    int FindSiteRow(int edge_id);

private:
    RobotViewerScene *scene_{nullptr};
    int sort_type_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__DOCK_SITE__H__
