#ifndef EDITBOOKDIALOG_H
#define EDITBOOKDIALOG_H

#include <QDialog>

namespace Ui {
class EditbookDialog;
}

class EditbookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditbookDialog(QWidget *parent = nullptr);
    ~EditbookDialog();

    void setBookData(const QString &name,
                     const QString &author,
                     const QString &genreFa,
                     const QString &desc);

    QString getBookName() const;
    QString getAuthorName() const;
    QString getGenreFa() const;
    QString getDescription() const;

private slots:
    void on_pushButton_sabt_clicked();
    void on_pushButton_enseraf_clicked();

private:
    Ui::EditbookDialog *ui;
};

#endif // EDITBOOKDIALOG_H