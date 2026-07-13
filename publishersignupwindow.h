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

signals:
    void backtologinwindow();
    void signupsuccessful();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::publishersignupwindow *ui;
};

#endif // PUBLISHERSIGNUPWINDOW_H
