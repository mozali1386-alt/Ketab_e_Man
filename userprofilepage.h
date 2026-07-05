#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include <QWidget>

namespace Ui {
class Userprofilepage;
}

class Userprofilepage : public QWidget
{
    Q_OBJECT

public:
    explicit Userprofilepage(QWidget *parent = nullptr);
    ~Userprofilepage();

private:
    Ui::Userprofilepage *ui;
};

#endif // USERPROFILEPAGE_H
