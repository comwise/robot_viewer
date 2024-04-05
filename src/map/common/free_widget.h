#ifndef __COMWISE__ROBOT_VIEWER__FREE_WIDGET__H__
#define __COMWISE__ROBOT_VIEWER__FREE_WIDGET__H__

#include <QtWidgets/QWidget>
#include <string>

namespace Json {
    class Value;
}

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLayout;
class QVBoxLayout;
class QPushButton;
class QScrollArea;
QT_END_NAMESPACE

class FreeWidget : public QWidget
{
public:
    using json_value_t = Json::Value;

    Q_OBJECT
public:
    explicit FreeWidget(const std::string &file, QWidget *parent = 0);
    ~FreeWidget();

    bool init(const std::string &file);

    bool read_json_from_file(const std::string &file);
    bool write_json_to_file(const std::string &file);

    bool set_read_only(bool enable);

public slots:
    bool load_config(const QString &file);
    bool save_config(const QString &file);

    void load_config(bool clicked);
    void save_config(bool clicked);

private:
    bool init_widget();
    bool init_free_ui(const json_value_t &data, const QString &key,
            QGridLayout *lyt, int &row, int col = 0);
    void remove_all_widget(QLayout* lyt);

    bool parse_json(json_value_t &obj);
    bool pack_json(json_value_t &obj);

private:
    std::string cfg_file_;
    QGridLayout *main_lyt_{nullptr};
    QVBoxLayout *parent_lyt_{nullptr};
    QPushButton *load_btn_{nullptr};
    QPushButton *save_btn_{nullptr};
    QWidget *free_wgt_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__FREE_WIDGET__H__
