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

private slots:
    void on_pushButton_managmentbook_clicked();

    void on_pushButton_statistic_clicked();

    void on_pushButton_hesabkarbar_clicked();

private:
    Ui::Publisherdashboard *ui;
};

#endif // PUBLISHERDASHBOARD_H
