#ifndef STOREPAGE_H
#define STOREPAGE_H

#include <QString>
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

private slots:

    void on_pushButton_search_clicked();

private:
    Ui::Storepage *ui;

    QString lastsearchdisplay;
    void processServerResponse(const QString &response);
    void requestBookSummary(const QString &bookId);
};

#endif // STOREPAGE_H