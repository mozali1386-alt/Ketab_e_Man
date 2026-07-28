#ifndef BOOKDETAILS_H
#define BOOKDETAILS_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include "clientsocketmanager.h"

namespace Ui {
class Bookdetails;
}

class Bookdetails : public QWidget
{
    Q_OBJECT

public:
    explicit Bookdetails(ClientSocketManager *client,
                         QWidget *parent = nullptr,
                         QString bookId = "");
    ~Bookdetails();

private slots:
    void on_pushButton_cartadd_clicked();
    void on_pushButton_cartremove_clicked();
    void on_pushButton_savelibrary_clicked();
    void on_pushButton_removelibrary_clicked();

    void on_toolButton_star1_clicked();
    void on_toolButton_star2_clicked();
    void on_toolButton_star3_clicked();
    void on_toolButton_star4_clicked();
    void on_toolButton_star5_clicked();

    void on_pushButton_commentadd_clicked();
    void on_pushButton_commentedit_clicked();
    void on_pushButton_commentremove_clicked();
    void processServerResponse(const QString &response);

private:
    Ui::Bookdetails *ui;

    QString currentBookId;
    QString currentUsername;
    bool isInCart;
    bool isSaved;
    bool isOwned;

    int currentUserRating;
    int originalUserRating;
    QString originalUserComment;

    void requestBookDetails();
    void requestCommentsList();
    void requestCommentDetails(const QString &commentId);

    void updateActionButtonsUI();
    void updateCommentFormUI(bool hasCommented, bool isEditing);
    void updateStarsUI(int score);
    QString englishToPersianGenre(const QString &english);

    void loadAndSetImage(QLabel *imageLabel, const QString &base64Data);
    ClientSocketManager *m_client = nullptr;
};
#endif // BOOKDETAILS_H