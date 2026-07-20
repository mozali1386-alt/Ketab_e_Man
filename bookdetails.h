#ifndef BOOKDETAILS_H
#define BOOKDETAILS_H

#include <QLabel>
#include <QString>
#include <QWidget>

namespace Ui {
class Bookdetails;
}

class Bookdetails : public QWidget
{
    Q_OBJECT

public:
    explicit Bookdetails(QWidget *parent = nullptr, QString bookId = "");
    ~Bookdetails();

    void processServerResponse(const QString &response);

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

    void loadAndSetImage(QLabel *imageLabel, const QString &base64Data);
};
#endif // BOOKDETAILS_H