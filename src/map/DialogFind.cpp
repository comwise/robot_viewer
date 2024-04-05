#include "DialogFind.h"
#include "ui_DialogFind.h"

DialogFind::DialogFind(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogFind)
    , is_search_(false)
    , sel_type_(FindSite)
    , search_id_(-1)
{
    ui->setupUi(this);
    ui->siteButton->setChecked(true);
}

DialogFind::~DialogFind()
{
    delete ui;
    ui = nullptr;
}

bool DialogFind::search()
{
    return is_search_;
}

int DialogFind::getSearchID()
{
    return search_id_;
}

int DialogFind::getSelectType()
{
    return sel_type_;
}

void DialogFind::on_siteButton_clicked()
{
    sel_type_ = FindSite;
}

void DialogFind::on_nodeButton_clicked()
{
    sel_type_ = FindNode;
}

void DialogFind::on_edgeButton_clicked()
{
    sel_type_ = FindEdge;
}

void DialogFind::on_okButton_clicked()
{
    is_search_ = true;
    search_id_ = ui->findIDLineEdit->text().toInt();
    ui->findIDLineEdit->clear();
    close();
}

void DialogFind::on_cancelButton_clicked()
{
    is_search_ = false;
    close();
}
