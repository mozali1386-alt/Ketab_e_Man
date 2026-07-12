#ifndef CARTPAGE_H
#define CARTPAGE_H

#include <QWidget>

namespace Ui {
class Cartpage;
}

class Cartpage : public QWidget
{
    Q_OBJECT

public:
    explicit Cartpage(QWidget *parent = nullptr);
    ~Cartpage();

private:
    Ui::Cartpage *ui;
};

#endif // CARTPAGE_H
