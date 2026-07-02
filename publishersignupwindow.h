#ifndef PUBLISHERSIGNUPWINDOW_H
#define PUBLISHERSIGNUPWINDOW_H

#include <QMainWindow>

namespace Ui {
class publishersignupwindow;
}

class publishersignupwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit publishersignupwindow(QWidget *parent = nullptr);
    ~publishersignupwindow();

private:
    Ui::publishersignupwindow *ui;
};

#endif // PUBLISHERSIGNUPWINDOW_H
