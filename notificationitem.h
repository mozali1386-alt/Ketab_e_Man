#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QWidget>

namespace Ui {
class Notificationitem;
}

class Notificationitem : public QWidget
{
    Q_OBJECT

public:
    explicit Notificationitem(QWidget *parent = nullptr);
    ~Notificationitem();

private:
    Ui::Notificationitem *ui;
};

#endif // NOTIFICATIONITEM_H
