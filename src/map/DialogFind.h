#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_FIND__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_FIND__H__

#include <QDialog>

namespace Ui {
class DialogFind;
}

class DialogFind : public QDialog
{
    Q_OBJECT

public:
    enum FindType {
        FindNode,
        FindSite,
        FindEdge,
    };

public:
    explicit DialogFind(QWidget *parent = 0);
    ~DialogFind();

    int getSelectType();
    int getSearchID();
    bool search();

private slots:
    void on_siteButton_clicked();
    void on_nodeButton_clicked();
    void on_edgeButton_clicked();
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::DialogFind *ui{nullptr};

    int sel_type_{0};
    bool is_search_{false};
    int search_id_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_FIND__H__
