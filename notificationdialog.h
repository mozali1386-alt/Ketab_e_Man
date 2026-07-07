#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>

namespace Ui {
class Notificationdialog;
}

class Notificationdialog : public QDialog
{
    Q_OBJECT

public:
    explicit Notificationdialog(QWidget *parent = nullptr);
    ~Notificationdialog();

private:
    Ui::Notificationdialog *ui;
};

#endif // NOTIFICATIONDIALOG_H
