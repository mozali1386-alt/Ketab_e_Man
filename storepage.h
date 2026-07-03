#ifndef STOREPAGE_H
#define STOREPAGE_H

#include <QWidget>

namespace Ui {
class Storepage;
}

class Storepage : public QWidget
{
    Q_OBJECT

public:
    explicit Storepage(QWidget *parent = nullptr);
    ~Storepage();

private:
    Ui::Storepage *ui;
};

#endif // STOREPAGE_H
