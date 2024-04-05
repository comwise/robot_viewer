#include "DockSite.h"
#include "RobotViewerScene.h"
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>

DockSite::DockSite(QWidget *parent /*= 0*/)
    : QTableWidget(parent)
    , scene_(NULL)
{
    InitForm();
}

void DockSite::InitForm()
{
    qRegisterMetaType<SiteData>("SiteData");

    QStringList list;
    sort_type_ = 0;
    list << tr("任务点ID") << tr("起点-终点") << tr("X坐标") << tr("Y坐标") << tr("百分比");
    setColumnCount(5);
    setHorizontalHeaderLabels(list);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->verticalHeader()->hide();
    // this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setHighlightSections(true);
    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SortSite(int)));
    // connect(this,SIGNAL(itemClicked(QTableWidgetItem * )),this,SLOT(SelectSiteChanged(QTableWidgetItem * )));
}

void DockSite::SetScene(RobotViewerScene *scene)
{
    disconnect(this, 0, 0, 0);
    scene_ = scene;
    connect(scene_, SIGNAL(siteChanged(const SiteData &, int)), this, SLOT(UpdateData(const SiteData &, int)));
}

void DockSite::AddSite(const SiteData &site_data)
{
    if (NULL == scene_)
    {
        return;
    }
    int row_count = rowCount();
    this->insertRow(row_count);
    QTableWidgetItem *tableitem = new QTableWidgetItem;
    setItem(row_count, 0, tableitem);
    tableitem = new QTableWidgetItem;
    setItem(row_count, 1, tableitem);
    tableitem = new QTableWidgetItem;
    setItem(row_count, 2, tableitem);
    tableitem = new QTableWidgetItem;
    setItem(row_count, 3, tableitem);
    tableitem = new QTableWidgetItem;
    setItem(row_count, 4, tableitem);
    UpdateRow(row_count, site_data);
}

QString DockSite::GetSiteDesc(const SiteData &site_data)
{
    if (site_data.direction == (int)NEGTIVE_WAY)
    {
        return QString::number(site_data.endid) + "->" + QString::number(site_data.startid);
    }
    if (site_data.direction == (int)POSITIVE_WAY)
    {
        return QString::number(site_data.startid) + "->" + QString::number(site_data.endid);
    }
    if (site_data.direction == (int)BOTH_WAY)
    {
        return QString::number(site_data.startid) + "<->" + QString::number(site_data.endid);
    }
    return QString();
}

void DockSite::RemoveSite(const SiteData &site_data)
{
    int site_row = FindSiteRow(site_data.id);
    if (site_row != -1)
    {
        removeRow(site_row);
    }
}

void DockSite::UpdateSite(const SiteData &site_data)
{
    int site_row = FindSiteRow(site_data.id);
    if (site_row != -1)
    {
        UpdateRow(site_row, site_data);
    }
}

void DockSite::SortSite(int index)
{
    static int last_sort(true);
    sort_type_ = index;
    if (last_sort)
    {
        last_sort = false;
        sortItems(index, Qt::AscendingOrder);
    }
    else
    {
        last_sort = true;
        sortItems(index, Qt::DescendingOrder);
    }
}

void DockSite::UpdateData(const SiteData &site_data, int change_type)
{
    switch (change_type)
    {
    case 0:
    {
        AddSite(site_data);
        break;
    }
    case 1:
    {
        RemoveSite(site_data);
        break;
    }
    case 2:
    {
        UpdateSite(site_data);
        break;
    }
    }
}

void DockSite::UpdateRow(int row, const SiteData &site_data)
{
    if (row >= rowCount() || row < 0)
    {
        return;
    }
    // int rowcount = rowCount();
    QTableWidgetItem *tableitem = item(row, 0);
    tableitem->setText(QString::number(site_data.id));
    tableitem = item(row, 1);
    tableitem->setText(GetSiteDesc(site_data));
    tableitem = item(row, 2);
    tableitem->setText(QString::number(site_data.pos.x));
    tableitem = item(row, 3);
    tableitem->setText(QString::number(site_data.pos.y));
    tableitem = item(row, 4);
    tableitem->setText(QString::number(site_data.percent));
}

int DockSite::FindSiteRow(int site_id)
{
    int rowcount = rowCount();
    for (int i = 0; i < rowcount; ++i)
    {
        if (item(i, 0)->text().toInt() == site_id)
        {
            return i;
        }
    }
    return -1;
}

void DockSite::mouseDoubleClickEvent(QMouseEvent *event)
{
    SelectSiteChanged(NULL);
}

void DockSite::SelectSiteChanged(QTableWidgetItem *)
{
    int row = currentRow();
    if (row == -1)
    {
        return;
    }
    if (scene_)
    {
        int site_id = item(row, 0)->text().toInt();
        if (!scene_->searchSite(site_id))
        {
            QMessageBox::warning(this, tr("任务点查询"), tr("任务点不存在"));
            removeRow(row);
        }
    }
}
