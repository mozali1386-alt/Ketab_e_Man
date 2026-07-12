#ifndef PUBLISHERDASHBOARD_H
#define PUBLISHERDASHBOARD_H

#include <QMainWindow>

namespace Ui {
class Publisherdashboard;
}

class Publisherdashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Publisherdashboard(QWidget *parent = nullptr);
    ~Publisherdashboard();

private:
    Ui::Publisherdashboard *ui;
};

#endif // PUBLISHERDASHBOARD_H
