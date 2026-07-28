#include "editbookdialog.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include "ui_editbookdialog.h"

EditbookDialog::EditbookDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditbookDialog)
{
    ui->setupUi(this);
    ui->comboBox_genre->clear();
    QStringList persianGenres
        = {"عاشقانه", "جنایی", "علمی تخیلی", "ترسناک", "کلاسیک", "هنری", "طنز (کمدی)", "تاریخی"};
    ui->comboBox_genre->addItems(persianGenres);
}

EditbookDialog::~EditbookDialog()
{
    delete ui;
}

void EditbookDialog::setBookData(const QString &name,
                                 const QString &author,
                                 const QString &genreFa,
                                 const QString &desc)
{
    ui->lineEdit_namebook->setText(name);
    ui->lineEdit_nameauthor->setText(author);
    ui->textEdit_tozih->setText(desc);

    int index = ui->comboBox_genre->findText(genreFa);
    if (index >= 0) {
        ui->comboBox_genre->setCurrentIndex(index);
    }
}

QString EditbookDialog::getBookName() const
{
    return ui->lineEdit_namebook->text().trimmed();
}

QString EditbookDialog::getAuthorName() const
{
    return ui->lineEdit_nameauthor->text().trimmed();
}

QString EditbookDialog::getGenreFa() const
{
    return ui->comboBox_genre->currentText();
}

QString EditbookDialog::getDescription() const
{
    return ui->textEdit_tozih->toPlainText().trimmed();
}

void EditbookDialog::on_pushButton_sabt_clicked()
{
    if (getBookName().isEmpty() || getAuthorName().isEmpty()) {
        QMessageBox::warning(
            this,
            "اخطار",
            "فیلدهای نام کتاب و نویسنده نمی‌توانند خالی "
            "باشند.");
        return;
    }
    this->accept();
}

void EditbookDialog::on_pushButton_enseraf_clicked()
{
    this->reject();
}
