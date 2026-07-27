#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QProgressDialog>
#include <QQuickWidget>
#include <QTableWidgetItem>
#include <notification.h>

namespace Ui {
class Admindashboard;
}

class Admindashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Admindashboard(QWidget *parent = nullptr);
    ~Admindashboard();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onBellClicked();
    void onNotificationClicked(const QString &notifId);
    void onMarkAllReadClicked();

    void on_pushButton_search_clicked();
    void on_tableWidget_allusers_itemSelectionChanged();
    void on_pushButton_deletehesab_clicked();
    void on_pushButton_block_clicked();
    void on_pushButton_unblock_clicked();

    void on_tableWidget_books_itemSelectionChanged();
    void on_pushButton_deletebook_clicked();
    void on_pushButton_editbook_clicked();
    void on_pushButton_pikther_clicked();
    void on_pushButton_pdf_clicked();

    void on_tableWidget_comment_itemSelectionChanged();
    void on_pushButton_deletecomment_clicked();
    void on_pushButton_textcomelcomment_clicked();

    // برای ریل تایم بودن
    void on_tabWidget_currentChanged(int index);

private:
    Ui::Admindashboard *ui;

    Notification *notifPopup;
    void requestUserInfo(const QString &userId);
    void processServerResponse(const QString &response);
    int findUserRowById(const QString &userId);

    void requestAdminBooks();
    void requestAdminComments();
    QString translateGenreToPersian(const QString &englishGenre);
    QString translateGenreToEnglish(const QString &persianGenre);

    QProgressDialog *pdfLoadingDialog = nullptr;
    QByteArray currentPdfBuffer;
    QString currentPdfBookId;

    // Client *client;
};

#endif // ADMINDASHBOARD_H