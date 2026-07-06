#ifndef BOOKLIBRARYITEM_H
#define BOOKLIBRARYITEM_H

#include <QWidget>

namespace Ui {
class Booklibraryitem;
}

class Booklibraryitem : public QWidget
{
    Q_OBJECT

public:
    explicit Booklibraryitem(QWidget *parent = nullptr);
    ~Booklibraryitem();

private:
    Ui::Booklibraryitem *ui;
};

#endif // BOOKLIBRARYITEM_H
