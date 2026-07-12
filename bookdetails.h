#ifndef BOOKDETAILS_H
#define BOOKDETAILS_H

#include <QWidget>

namespace Ui {
class Bookdetails;
}

class Bookdetails : public QWidget
{
    Q_OBJECT

public:
    explicit Bookdetails(QWidget *parent = nullptr);
    ~Bookdetails();

private:
    Ui::Bookdetails *ui;
};

#endif // BOOKDETAILS_H
