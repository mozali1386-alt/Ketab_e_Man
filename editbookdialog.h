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

private:
    Ui::EditbookDialog *ui;
};

#endif // EDITBOOKDIALOG_H
