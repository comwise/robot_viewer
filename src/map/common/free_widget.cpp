#include "common/free_widget.h"
#include <QtWidgets>
#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QDebug>
#include "common/json_readerwriter.h"

#define LOG_DEBUG qDebug()<<__FUNCTION__<<":"

static const int kFormMaxWidth = 350;
static const int kFormFixedHeight = 32;
static const int kSpacerItemMaxWidth = -1;
static const int kSpacerItemMaxHeight = 18;
static const char kFormPropValueKey[] = "form_value";
static const char kFormPropCommentKey[] = "comment_value";

FreeWidget:: FreeWidget(const std::string &file, QWidget *parent)
    :  QWidget(parent)
    , cfg_file_(file)
{
    init(file);
}

FreeWidget::~FreeWidget()
{
    
}

bool FreeWidget::init(const std::string &file)
{
    bool ret = true;

    ret &= init_widget();

    ret &= read_json_from_file(file);

    return ret;
}

bool FreeWidget::init_widget()
{
    main_lyt_ = new QGridLayout();
    main_lyt_->setContentsMargins(1, 1, 1, 1);
    main_lyt_->setSpacing(20);

    QVBoxLayout *vblyt = new QVBoxLayout();
    main_lyt_->setContentsMargins(0, 0, 0, 0);
    vblyt->addLayout(main_lyt_);
    vblyt->addStretch();

    free_wgt_ = new QWidget();
    free_wgt_->setLayout(vblyt);

    QScrollArea *scroll_area_ = new QScrollArea;
    scroll_area_->setFrameStyle(QFrame::NoFrame);
    scroll_area_->setContentsMargins(0, 0, 0, 0);
    scroll_area_->setMaximumHeight(800);
    //scroll_area_->setMaximumSize(480, 640);
    scroll_area_->setWidget(free_wgt_);
    scroll_area_->setWidgetResizable(true);

    QHBoxLayout *test_lyt = new QHBoxLayout;
    test_lyt->setContentsMargins(0, 0, 0, 0);
    load_btn_ = new QPushButton(tr("加载"));
    load_btn_->setMinimumSize(100, 32);
    save_btn_ = new QPushButton(tr("保存"));
    save_btn_->setMinimumSize(100, 32);
    connect(load_btn_, SIGNAL(clicked(bool)), this, SLOT(load_config(bool)));
    connect(save_btn_, SIGNAL(clicked(bool)), this, SLOT(save_config(bool)));
    test_lyt->addStretch();
    test_lyt->addWidget(load_btn_);
    //test_lyt->addStretch();
    test_lyt->addWidget(save_btn_);
    //test_lyt->addStretch();

    QVBoxLayout *main_lyt = new QVBoxLayout(this);
    main_lyt->setContentsMargins(0, 0, 0, 0);
    main_lyt->addLayout(test_lyt);
    main_lyt->addWidget(scroll_area_);
    
    parentWidget()->setLayout(main_lyt);

    return true;
}

bool FreeWidget::init_free_ui(const json_value_t &json_val,
                              const QString &key, QGridLayout *lyt, int &row, int col)
{
    bool ret = true;

    if(nullptr == lyt) {
        LOG_DEBUG << "layout is nullptr";
        return false;
    }
    switch(json_val.type()) {
    case Json::objectValue:
    {
        if(!key.isEmpty() && col == 0) {
            QFrame* frame = new QFrame(this);
            frame->setFrameShape(QFrame::HLine);
            lyt->addWidget(frame, row++, col, 1, -1);
        }
        bool is_first = false;
        if(!key.isEmpty()) {
            QLabel *key_form = new QLabel(key + ": ");
            key_form->setProperty(kFormPropValueKey, QVariant(key));
            lyt->addWidget(key_form, row++, col);
        } else {
            is_first = true;
        }
        Json::Value::Members mem = json_val.getMemberNames();
        for (auto iter = mem.begin(); iter != mem.end(); iter++) {
            std::string key = *iter;
            auto& val = json_val[*iter];
            init_free_ui(val, key.c_str(), lyt, row, is_first?col:col+1);
        }
        break;
    }
    case Json::arrayValue:
    {
        if(col == 0) {
            QFrame* frame = new QFrame(this);
            frame->setFrameShape(QFrame::HLine);
            lyt->addWidget(frame, row++, col, 1, -1);
        }
        if(!key.isEmpty()) {
            QLabel *key_form = new QLabel(key + ": ");
            key_form->setProperty(kFormPropValueKey, QVariant(key));
            lyt->addWidget(key_form, row++, col);
        }

        std::size_t cnt = json_val.size();
        for (uint32_t i = 0; i < cnt; i++) {
            json_value_t val = json_val[i];
            QString new_key = QString("%1").arg(i);
            init_free_ui(val, new_key, lyt, row, col);
        }
        break;
    }
    case Json::stringValue:
    {
        QString val = tr(json_val.asString().c_str());
        QString key_comment = key + tr("?");
        QString key_comment2 = key + tr("？");
        QString val_comment;

        QLabel *key_form = new QLabel(key + ": ", this);
        key_form->setProperty(kFormPropValueKey, QVariant(key));
        key_form->setProperty(kFormPropCommentKey, val_comment);

        QLineEdit *val_form = new QLineEdit(val, this);
        val_form->setProperty(kFormPropValueKey, QVariant(val));
        val_form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        val_form->setMaximumSize(kFormMaxWidth, kFormFixedHeight);
        val_form->setMinimumHeight(26);
        if(key.contains("?") || key.contains(tr("？"))) {
            val_form->setReadOnly(true);
        }

        QSpacerItem* item = new QSpacerItem(kSpacerItemMaxWidth, kSpacerItemMaxHeight);

        lyt->addWidget(key_form, row, col);
        lyt->addWidget(val_form, row, col+1);
        lyt->addItem(item, row, col+2);
        row++;

        break;
    }
    case Json::realValue:
    {
        double val = json_val.asDouble();

        QString key_comment = key + tr("?");
        QString key_comment2 = key + tr("？");
        QString val_comment;

        QLabel *key_form = new QLabel(key + ":", this);
        key_form->setProperty(kFormPropValueKey, QVariant(key));

        QString info = QString("%1").arg(val);
        QLineEdit *val_form = new QLineEdit(info, this);
        val_form->setProperty(kFormPropValueKey, QVariant(val));
        val_form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        val_form->setMaximumSize(kFormMaxWidth, kFormFixedHeight);
        val_form->setMinimumHeight(26);
        if(key.contains("?") || key.contains(tr("？"))) {
            val_form->setReadOnly(true);
        }

        QSpacerItem* item = new QSpacerItem(kSpacerItemMaxWidth, kSpacerItemMaxHeight);

        lyt->addWidget(key_form, row, col);
        lyt->addWidget(val_form, row, col+1);
        lyt->addItem(item, row, col+2);
        row++;

        break;
    }
    case Json::uintValue:
    case Json::intValue:
    {
        int val = json_val.asInt();

        QString key_comment = key + tr("?");
        QString key_comment2 = key + tr("？");
        QString val_comment;

        QLabel *key_form = new QLabel(key + ":", this);
        key_form->setProperty(kFormPropValueKey, QVariant(key));

        QString info = QString("%1").arg(val);
        QLineEdit *val_form = new QLineEdit(info, this);
        val_form->setProperty(kFormPropValueKey, QVariant(val));
        val_form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        val_form->setMaximumSize(kFormMaxWidth, kFormFixedHeight);
        val_form->setMinimumHeight(26);
        if(key.contains("?") || key.contains(tr("？"))) {
            val_form->setReadOnly(true);
        }

        QSpacerItem* item = new QSpacerItem(kSpacerItemMaxWidth, kSpacerItemMaxHeight);

        lyt->addWidget(key_form, row, col);
        lyt->addWidget(val_form, row, col+1);
        lyt->addItem(item, row, col+2);
        row++;

        break;
    }
    case Json::booleanValue:
    {
        bool val = json_val.asBool();
        QLabel *key_form = new QLabel(key + ": ", this);
        key_form->setProperty(kFormPropValueKey, QVariant(key));

        QCheckBox *val_form = new QCheckBox("", this);
        val_form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        val_form->setMaximumSize(kFormMaxWidth, kFormFixedHeight);
        val_form->setMinimumHeight(26);
        val_form->setCheckable(true);
        val_form->setCheckState(val? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        val_form->setProperty(kFormPropValueKey, QVariant(val));

        QSpacerItem* item = new QSpacerItem(kSpacerItemMaxWidth, kSpacerItemMaxHeight);

        lyt->addWidget(key_form, row, col);
        lyt->addWidget(val_form, row, col+1);
        lyt->addItem(item, row, col+2);
        row++;

        break;
    }
    default:
        ret = false;
        break;
    }
    return ret;
}

bool FreeWidget::set_read_only(bool enable)
{
    if(load_btn_) load_btn_->setEnabled(!enable);
    if(save_btn_) save_btn_->setEnabled(!enable);
    if(free_wgt_) free_wgt_->setEnabled(!enable);
    return true;
}

void FreeWidget::load_config(bool clicked)
{
    LOG_DEBUG << "load config clicked" << clicked;

    QString file = QFileDialog::getOpenFileName(this,
        tr("Open Json File"), "~/config", tr("Json Files (*.json)"));
    if(!file.isEmpty()) {
        load_config(file);
    }
}

void FreeWidget::save_config(bool clicked)
{
    LOG_DEBUG << "save config clicked" << clicked;
    QString file = cfg_file_.c_str();
    QString info;
    bool ret = save_config(file);
    info = QString("save config file\n%1\n %2").arg(file).arg(ret? "successed": "failed");
    if(ret) {
        QMessageBox::information(this, "save config", info);
    } else {
        QMessageBox::critical(this, "save config", info);
    }
}

bool FreeWidget::load_config(const QString &file)
{
    if(file.isEmpty()) {
        LOG_DEBUG << "file is empty, file = " << file;
        return false;
    }
    remove_all_widget(main_lyt_);
    main_lyt_->update();
    return read_json_from_file(file.toStdString());
}

bool FreeWidget::save_config(const QString &file)
{
    if(file.isEmpty()) {
        LOG_DEBUG << "file is empty, file = " << file;
        return false;
    }
    Json::Value obj;
    if(!pack_json(obj)) {
        LOG_DEBUG << "pack json to file failed, file = " << file;
        return false;
    }

#ifdef STYLE_DEBUG
    std::string write_str = obj.toStyledString();
    qDebug() << write_str.c_str();
#endif
    
    return JsonReaderWriter::write_json(file.toStdString(), obj);
}

bool FreeWidget::parse_json(json_value_t &obj)
{
    if(obj.isNull()) {
        return false;
    }
    return true;
}

bool FreeWidget::pack_json(json_value_t &obj)
{
    if(nullptr == main_lyt_) {
        return false;
    }

    int row = main_lyt_->rowCount();
    int col = main_lyt_->columnCount();

    std::string new_key[col] = {};

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            QLayoutItem* item = main_lyt_->itemAtPosition(i, j);
            if(NULL == item) {
                continue;
            }
            QWidget *wgt = item->widget();
            if(NULL == wgt) {
                continue;
            }
            QString wgt_type = wgt->metaObject()->className();
            QVariant var = wgt->property(kFormPropValueKey);
            qDebug() << "[" << i << "," << j << "]" << wgt_type << var;

            if(wgt_type == "QFrame") {
                continue;
            }

            if(wgt_type == "QLabel") {
                QLabel *lbl = dynamic_cast<QLabel*>(wgt);
                if(nullptr == lbl) {
                    continue;
                }
                QString key = lbl->text();
                if(var.type() == QVariant::String) {
                    key = var.toString();
                }
                new_key[j] = key.toStdString();

            } else if(wgt_type == "QLineEdit") {
                QLineEdit *form = dynamic_cast<QLineEdit*>(wgt);
                if(nullptr == form) {
                    continue;
                }
                QString txt = form->text();
                Json::Value* find_iter = &obj;
                for(int k = 0; k < j-1; k++) {
                    if(!find_iter->isMember(new_key[k])) {
                        find_iter->operator [](new_key[k]) = Json::Value();
                    }
                    find_iter = &(find_iter->operator [](new_key[k]));
                }
                if(j > 0) {
                    QVariant::Type type = var.type();
                    switch(type)
                    {
                    case QVariant::Bool:
                        find_iter->operator [](new_key[j-1]) = (txt.toInt() != 0);
                        break;
                    case QVariant::UInt:
                    case QVariant::Int:
                    case QVariant::LongLong:
                    case QVariant::ULongLong:
                        find_iter->operator [](new_key[j-1]) = txt.toInt();
                        break;
                    case QVariant::Double:
                        find_iter->operator [](new_key[j-1]) = txt.toDouble();
                        break;
                    case QVariant::String:
                        find_iter->operator [](new_key[j-1]) = txt.toStdString();
                        break;
                     default:
                        break;
                    }
                }

            } else if(wgt_type == "QCheckBox") {
                QCheckBox *form = dynamic_cast<QCheckBox*>(wgt);
                if(nullptr == form) {
                    continue;
                }
                bool is_checked = form->checkState() == Qt::CheckState::Checked;
                Json::Value* find_iter = &obj;
                for(int k = 0; k < j-1; k++) {
                    if(!find_iter->isMember(new_key[k])) {
                        find_iter->operator [](new_key[k]) = Json::Value();
                    }
                    find_iter = &(find_iter->operator [](new_key[k]));
                }
                if(j > 0) {
                    find_iter->operator [](new_key[j-1]) = is_checked;
                }

            } else if(wgt_type == "QComboBox") {
                QComboBox *form = dynamic_cast<QComboBox*>(wgt);
                if(nullptr == form) {
                    continue;
                }
                std::string form_txt = form->currentText().toStdString();
                Json::Value* find_iter = &obj;
                for(int k = 0; k < j-1; k++) {
                    if(!find_iter->isMember(new_key[k])) {
                        find_iter->operator [](new_key[k]) = Json::Value();
                    }
                    find_iter = &(find_iter->operator [](new_key[k]));
                }
                if(j > 0) {
                    find_iter->operator [](new_key[j-1]) = form_txt;
                }

            } else {
                //todo
            }
        }
    }
    return true;
}

bool FreeWidget::read_json_from_file(const std::string &file)
{
    bool ret = true;

    Json::Value root;
    if (!JsonReaderWriter::read_json(file, root)) {
        LOG_DEBUG << "read json file error, file = " << file.c_str();
        return false;
    }

    int row = 0;
    ret = init_free_ui(root, "", main_lyt_, row, 0);
    main_lyt_->update();

    return ret;
}

bool FreeWidget::write_json_to_file(const std::string &file)
{
    bool ret = false;
    try {
        Json::Value root;
        if(pack_json(root)) {
            LOG_DEBUG << "pack data to json object error, file = " << file.c_str();
            return false;
        }
        ret = JsonReaderWriter::write_json(file, root);

    } catch (std::exception &e) {
        LOG_DEBUG << "write json file exception, " << e.what();
        ret = false;
    }
    return ret;
}

void FreeWidget::remove_all_widget(QLayout* lyt)
{
    QLayoutItem *item = nullptr;
    while((item = lyt->takeAt(0)) != 0) {
        if(item->widget()) {
            item->widget()->setParent(NULL);
            delete item->widget();
        }
        QLayout *childlyt = item->layout();
        if(childlyt) {
            remove_all_widget(childlyt);
        }
        delete item;
    }
}