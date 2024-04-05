#ifndef __COMWISE__ROBOT_VIEWER__FREE_TABLEWIDGET__H__
#define __COMWISE__ROBOT_VIEWER__FREE_TABLEWIDGET__H__

#include <QWidget>

namespace Ui {
class FreeTabWidget;
}

class QLabel;
class QListWidget;
class QListWidgetItem;
class FreeWidget;
class QProcess;

class FreeTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FreeTabWidget(QWidget *parent = 0);
    ~FreeTabWidget();

    bool addWidget(QWidget *widget, const QString &txt = "");
    bool addWidget(int index, QWidget *widget, const QString &txt = "");
    bool removeWidget(int index);
    bool removeWidget(QWidget *widget);

signals:
    void notifyError(int, const QString &);

private slots:
    void pageChanged(QListWidgetItem *curr, QListWidgetItem *prev);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void init();

    void initForm();
    void initMessage();

    void addTabItem(int index, QListWidget* listWiget, const QString &txt = "");
    void addTabItem(QListWidget* listWiget, const QString &txt = "");

private:
    Ui::FreeTabWidget *ui_;

    std::string config_file_;

    QWidget* status_wgt_;
    FreeWidget* setting_wgt_;
};

#endif // __COMWISE__ROBOT_VIEWER__FREE_TABLEWIDGET_H
