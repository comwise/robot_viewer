#include "common/free_tabwidget.h"
#include "ui_free_tabwidget.h"
#include <QKeyEvent>
#include <QDebug>

FreeTabWidget::FreeTabWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::FreeTabWidget)
{
    ui_->setupUi(this);
    init();
}

FreeTabWidget::~FreeTabWidget()
{
    delete ui_;
}

void FreeTabWidget::init()
{
    initForm();
    initMessage();
}

void FreeTabWidget::initForm()
{
    ui_->listWidget->clear();
    ui_->stackedWidget->update();
    ui_->listWidget->setCurrentRow(0);
}

void FreeTabWidget::initMessage()
{
    connect(ui_->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(pageChanged(QListWidgetItem*, QListWidgetItem*)));

    qRegisterMetaType<QString>("QString");
}

void FreeTabWidget::addTabItem(QListWidget* wgt, const QString &txt)
{
    QListWidgetItem* item = new QListWidgetItem;
#ifndef DISENABLE_BGR
    wgt->setViewMode(QListView::IconMode);
    wgt->setIconSize(QSize(80, 60));
    //item->setIcon(QIcon(QString(":/image/") + APP_MODULE[index][2]));
#else
    wgt->setViewMode(QListView::ListMode);
#endif
    item->setText(txt);
    item->setStatusTip(txt);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    wgt->addItem(item);
}

void FreeTabWidget::addTabItem(int index, QListWidget* wgt, const QString &txt)
{
    QListWidgetItem* item = new QListWidgetItem;
#ifndef DISENABLE_BGR
    wgt->setViewMode(QListView::IconMode);
    wgt->setIconSize(QSize(80, 60));
    //item->setIcon(QIcon(QString(":/image/") + APP_MODULE[index][2]));
#else
    wgt->setViewMode(QListView::ListMode);
#endif
    item->setText(txt);
    item->setStatusTip(txt);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    wgt->insertItem(index, item);
}

bool FreeTabWidget::addWidget(int index, QWidget *widget, const QString &txt)
{
    addTabItem(index, ui_->listWidget, txt);
    QStackedWidget* pagesWidget = ui_->stackedWidget;
    if(pagesWidget) {
        pagesWidget->insertWidget(index,  widget);
        pagesWidget->update();
    }
    return true;
}

bool FreeTabWidget::addWidget(QWidget *widget, const QString &txt)
{
    addTabItem(ui_->listWidget, txt);
    QStackedWidget* pagesWidget = ui_->stackedWidget;
    if(pagesWidget) {
        pagesWidget->addWidget(widget);
        pagesWidget->update();
    }
    return true;
}

bool FreeTabWidget::removeWidget(int index)
{
    QWidget *wgt = ui_->stackedWidget->widget(index);
    if(wgt) {
        ui_->stackedWidget->removeWidget(wgt);
    }
    return true;
}

bool FreeTabWidget::removeWidget(QWidget *widget)
{
    ui_->stackedWidget->removeWidget(widget);
    return true;
}

void FreeTabWidget::pageChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (NULL == current)
        current = previous;

    int index = ui_->listWidget->row(current);
    ui_->stackedWidget->setCurrentIndex(index);
}

void FreeTabWidget::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void FreeTabWidget::closeEvent( QCloseEvent *event )
{
    QWidget::closeEvent(event);
}

