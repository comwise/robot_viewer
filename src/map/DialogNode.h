#ifndef __COMWISE__ROBOT_VIEWER__DIALOG_NODE__H__
#define __COMWISE__ROBOT_VIEWER__DIALOG_NODE__H__

#include <QDialog>

namespace Ui {
class DialogNode;
}

class ItemNode;
class DialogNode : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNode(ItemNode *node);
    ~DialogNode();

private slots:
    void on_btn_ok_clicked();
    void on_btn_cancel_clicked();

private:
    void init();

private:
    Ui::DialogNode *ui{nullptr};
    ItemNode *node_item_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__DIALOG_NODE__H__
