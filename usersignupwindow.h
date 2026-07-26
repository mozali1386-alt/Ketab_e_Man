#ifndef USERSIGNUPWINDOW_H
#define USERSIGNUPWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

namespace Ui {
class Usersignupwindow;
}

class Usersignupwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Usersignupwindow(QWidget *parent = nullptr);
    ~Usersignupwindow();

signals:
    void backtologinwindow();
    void signupsuccessful();
private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_confirm_clicked();

    void on_listWidget_genre_itemChanged(QListWidgetItem *item);
    void processServerResponse(const QString &response);

private:
    Ui::Usersignupwindow *ui;
};

#endif // USERSIGNUPWINDOW_H
