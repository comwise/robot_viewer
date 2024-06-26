#ifndef __COMWISE__ROBOT_VIEWER__CUSTOM_WIDGET__H__
#define __COMWISE__ROBOT_VIEWER__CUSTOM_WIDGET__H__

#include <QDialog>
#include <QLineEdit>
#include <QDockWidget> 

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QPaintEvent;
class QMouseEvent;
class QTimer;
QT_END_NAMESPACE

class CustomWidget : public QWidget {
  Q_OBJECT
 public:
  explicit CustomWidget(QWidget *parent = 0);

 signals:

 public slots:

 protected:
  void paintEvent(QPaintEvent *);
};

///!< CustomMoveWidget 可移动窗口
class CustomMoveWidget : public CustomWidget {
  Q_OBJECT
 public:
  explicit CustomMoveWidget(QWidget *parent = 0);
  ~CustomMoveWidget();

 protected:
  QPoint mousePoint;
  bool m_mousePressed;

  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *);
};

///!< The CustomDialog class
class CustomDialog : public QDialog {
  Q_OBJECT
 public:
  explicit CustomDialog(QWidget *parent = 0);
  ~CustomDialog();

 protected:
  QPoint mousePoint;
  bool m_mousePressed;
  QSize m_nNormalSize;

  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *);
};

///!< The CBaseDialog class
class CBaseDialog : public CustomDialog {
  Q_OBJECT
 public:
  explicit CBaseDialog(QWidget *parent = 0);
  ~CBaseDialog();

  void SetWinIcon(const QPixmap &pixmap);
  void SetWinTitle(const QString &text);

 private:
  QWidget *widgetWinTitle;
  QLabel *labelWinIcon;
  QLabel *labelWinTitle;
  QPushButton *btnWinMin;
  QPushButton *btnWinClose;

 protected:
  QWidget *widgetBody;
};

///!< The CMessageBox class 弹出信息框类
class CMessageBox : public CBaseDialog {
  Q_OBJECT

 public:
  typedef enum {
    E_Information = 0x01,
    E_Warning,
    E_Question,
    E_MSGTYPE_Error,
  } E_MSGTYPE;

 public:
  explicit CMessageBox(QWidget *parent = 0);
  ~CMessageBox();

  // 显示消息
  void ShowMessage(const QString &content,
                   const quint8 &msgType = CMessageBox::E_Information,
                   const QString &title = "");

  void StartTimer();

  static int Infomation(QWidget *parent, const QString &content,
                        const QString &title = "提示");
  static int Question(QWidget *parent, const QString &content,
                      const QString &title = "询问");
  static int Warning(QWidget *parent, const QString &content,
                     const QString &title = "告警");

 protected:
 private:
  QLabel *labelIcon;
  QLabel *labelMsgContent;

  QPushButton *btnOk;
  QPushButton *btnCancel;

  QTimer *m_timer;
  int m_nTimerCnt;
 public slots:
  void SltTimerOut();
};

/// <! 弹出输入框类 
class CInputDialog : public CBaseDialog {
  Q_OBJECT

 public:
  explicit CInputDialog(QWidget *parent = 0);
  ~CInputDialog();

  static QString GetInputText(QWidget *parent, const QString &text = "",
                              const QString &title = "输入",
                              QLineEdit::EchoMode mode = QLineEdit::Normal);

  QString GetText() const;
  void SetInputText(const QString &text);

  void SetEchoMode(QLineEdit::EchoMode mode);

 private:
  static CInputDialog *self;

  QLabel *labelText;
  QLineEdit *lineEditInput;

  QPushButton *btnOk;
  QPushButton *btnCancel;
};

class CDockWidget : public QDockWidget {
  Q_OBJECT
public:
  CDockWidget(const QString &title, QWidget *parent = Q_NULLPTR);
  virtual ~CDockWidget() {}

signals:
  void sigCloseEvent(QCloseEvent *);

protected:
  virtual void closeEvent(QCloseEvent *event) override;
};

#endif  // __COMWISE__ROBOT_VIEWER__CUSTOM_WIDGET__H__
