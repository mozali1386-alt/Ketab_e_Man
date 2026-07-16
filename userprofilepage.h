#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include <QList>
#include <QListWidgetItem>
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

    void simulateServerData();

private slots:
    void on_pushButton_topUpbalance_clicked();

    void on_pushButton_editpassword_clicked();

    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_pushButton_sabt_clicked();

    void on_pushButton_enseraf_clicked();

private:
    Ui::Userprofilepage *ui;

    QString originalName;
    QString originalEmail;
    QString originalGenres;
    QMap<QString, QString> genreMap;
    void information(QString dataString);
};

#endif // USERPROFILEPAGE_H
