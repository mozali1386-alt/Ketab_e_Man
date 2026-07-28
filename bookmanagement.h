#ifndef BOOKMANAGEMENT_H
#define BOOKMANAGEMENT_H

#include <QByteArray>
#include <QString>
#include <QWidget>
#include "clientsocketmanager.h"

namespace Ui {
class BookManagement;
}

class BookManagement : public QWidget
{
    Q_OBJECT

public:
    explicit BookManagement(QWidget *parent = nullptr);
    ~BookManagement();
    void refreshBooksList();
    void setClient(ClientSocketManager *client);

private slots:
    void on_comboBox_tipe_currentIndexChanged(int index);
    void on_pushButton_pikcher_clicked();
    void on_pushButton_pdf_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_edit_clicked();
    void on_pushButton_disable_clicked();
    void on_pushButton_enable_clicked();
    void processServerResponse(const QString &message);

private:
    Ui::BookManagement *ui;

    QString m_base64Image;
    QString m_base64Pdf;
    QString currentSelectedBookId;

    QString orig_bookName;
    QString orig_authorName;
    QString orig_genre;
    int orig_price;
    int orig_discount;
    QString orig_explanation;
    bool orig_hasNewImage;
    ClientSocketManager *m_client = nullptr;

    // توابع کمکی
    void resetForm();
    void sendPdfChunks(const QString &bookId);
    void setButtonImageFit(const QString &imagePath);
    bool hasFormChanged();
};

#endif // BOOKMANAGEMENT_H