#include "DockEdge.h"
#include "RobotViewerScene.h"
#include <QStringList>
#include <QString>
#include <QHeaderView>
#include <QMessageBox>

DockEdge::DockEdge(QWidget *parent /*= 0*/)
    : QTableWidget(parent),
      scene_(NULL)
{
    qRegisterMetaType<EdgeData>("EdgeData");
    InitForm();
}

void DockEdge::InitForm()
{
    setMinimumWidth(400);
    QStringList list;
    sort_type_ = 0;
    list << tr("边ID") << tr("起点-终点") << tr("Tim激光区域") << tr("SZ激光区域") << tr("速度") << tr("超声值");
    setColumnCount(6);
    setHorizontalHeaderLabels(list);
    this->hideColumn(2);
    this->hideColumn(3);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->verticalHeader()->hide();
    // this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setHighlightSections(true);
    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SortEdge(int)));
    connect(this, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(SelectEdgeChanged(QTableWidgetItem *)));
}

void DockEdge::SetScene(RobotViewerScene *scene)
{
    disconnect(this, 0, 0, 0);
    scene_ = scene;
    connect(scene_, SIGNAL(edgeChanged(const EdgeData &, int)), this, SLOT(UpdateData(const EdgeData &, int)));
}

void DockEdge::AddEdge(const EdgeData &edge_data)
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
    tableitem = new QTableWidgetItem;
    setItem(row_count, 5, tableitem);
    UpdateRow(row_count, edge_data);
}

QString DockEdge::GetEdgeDesc(const EdgeData &edge_data)
{
    if (edge_data.direction == NEGTIVE_WAY)
    {
        return QString::number(edge_data.endid) + "->" + QString::number(edge_data.startid);
    }
    if (edge_data.direction == POSITIVE_WAY)
    {
        return QString::number(edge_data.startid) + "->" + QString::number(edge_data.endid);
    }
    if (edge_data.direction == BOTH_WAY)
    {
        return QString::number(edge_data.startid) + "<->" + QString::number(edge_data.endid);
    }
    return QString();
}

void DockEdge::RemoveEdge(const EdgeData &edge_data)
{
    int edge_row = FindEdgeRow(edge_data.id);
    if (edge_row != -1)
    {
        removeRow(edge_row);
    }
}

void DockEdge::UpdateEdge(const EdgeData &edge_data)
{
    int edge_row = FindEdgeRow(edge_data.id);
    if (edge_row != -1)
    {
        UpdateRow(edge_row, edge_data);
    }
}

void DockEdge::SortEdge(int index)
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

void DockEdge::UpdateData(const EdgeData &edge_data, int change_type)
{
    switch (change_type)
    {
    case 0:
    {
        AddEdge(edge_data);
        break;
    }
    case 1:
    {
        RemoveEdge(edge_data);
        break;
    }
    case 2:
    {
        UpdateEdge(edge_data);
        break;
    }
    }
}

void DockEdge::UpdateRow(int row, const EdgeData &edge_data)
{
    if (row >= rowCount() || row < 0)
    {
        return;
    }
    // int rowcount = rowCount();
    QTableWidgetItem *tableitem = item(row, 0);
    tableitem->setText(QString::number(edge_data.id));
    tableitem = item(row, 1);
    tableitem->setText(GetEdgeDesc(edge_data));
    tableitem = item(row, 2);
    tableitem->setText(QString::number(edge_data.tim310_laser_bank));
    tableitem = item(row, 3);
    tableitem->setText(QString::number(edge_data.sz_laser_bank));
    tableitem = item(row, 4);
    tableitem->setText(QString::number(edge_data.edge_speed));
    tableitem = item(row, 5);
    tableitem->setText(QString::number(edge_data.reference_mode));
}

int DockEdge::FindEdgeRow(int edge_id)
{
    int rowcount = rowCount();
    for (int i = 0; i < rowcount; ++i)
    {
        if (item(i, 0)->text().toInt() == edge_id)
        {
            return i;
        }
    }
    return -1;
}

void DockEdge::mouseDoubleClickEvent(QMouseEvent *event)
{
    SelectEdgeChanged(NULL);
}

void DockEdge::SelectEdgeChanged(QTableWidgetItem *)
{
    int row = currentRow();
    if (row == -1)
    {
        return;
    }
    if (scene_)
    {
        int edge_id = item(row, 0)->text().toInt();
        if (!scene_->searchEdge(edge_id))
        {
            QMessageBox::warning(this, tr("边查询"), tr("边不存在"));
            removeRow(row);
        }
    }
}
