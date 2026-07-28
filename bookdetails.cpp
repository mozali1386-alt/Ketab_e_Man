#include "bookdetails.h"
#include <QByteArray>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <QStringList>
#include "commentcardwidget.h"
#include "ui_bookdetails.h"

Bookdetails::Bookdetails(ClientSocketManager *client, QWidget *parent, QString bookId)
    : QWidget(parent)
    , ui(new Ui::Bookdetails)
    , m_client(client)
{
    ui->setupUi(this);

    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &Bookdetails::processServerResponse);

    isInCart = false;
    isSaved = false;
    isOwned = false;
    currentUserRating = 5;
    originalUserRating = 5;
    originalUserComment = "";

    updateCommentFormUI(false, false);
    updateStarsUI(5);

    currentBookId = bookId;

    requestBookDetails();
    requestCommentsList();
}

Bookdetails::~Bookdetails()
{
    delete ui;
}


void Bookdetails::requestBookDetails()
{
    QString message = "GET_BOOK_DETAILS||" + currentBookId;
    m_client->sendMessage(message);

    // ================== شروع تست (بعداً پاک کن) ==================
    // if (currentBookId == "1") {
    //     processServerResponse("BOOK_DETAILS_RESULT||1||ali123||NO_IMAGE||"
    //                           "سمفونی مردگان||عباس معروفی||نشر ققنوس||کلاسیک||150000||20000||4.8||"
    //                           "این کتاب داستان یک خانواده اردبیلی را روایت "
    //                           "می‌کند...||0||1||1");

    //     processServerResponse("COMMENTS_LIST||C_1,C_2");
    // } else if (currentBookId == "3") {
    //     processServerResponse(
    //         "BOOK_DETAILS_RESULT||3||ali123||NO_IMAGE||"
    //         "شازده کوچولو||آنتوان دو سنت اگزوپری||نشر...||کلاسیک||120000||0||4.9||"
    //         "داستان شازده کوچولو...||1||0||0");

    //     processServerResponse("COMMENTS_LIST||C_3_1,C_3_2,C_3_3");
    // }

    // پایان تست------------------------------------------
}
void Bookdetails::requestCommentsList()
{
    QString message = "GET_COMMENTS_LIST||" + currentBookId;
    m_client->sendMessage(message);
}

void Bookdetails::requestCommentDetails(const QString &commentId)
{
    QString message = "GET_COMMENT_DETAILS||" + commentId;
    m_client->sendMessage(message);

    // ================== شروع تست (بعداً پاک کن) ==================

    // if (commentId == "C_1") {
    //     processServerResponse(
    //         "COMMENT_DETAILS||C_1||reza99||رضا کریمی||4||خیلی خوب بود||1402/05/10");
    // } else if (commentId == "C_2") {
    //     processServerResponse("COMMENT_DETAILS||C_2||ali123||علی محمدی||5||شاهکار بود||1402/06/12");
    // }

    // else if (commentId == "C_3_1") {
    //     processServerResponse(
    //         "COMMENT_DETAILS||C_3_1||user1||امیر حسینی||1||اصلا جالب نبود||1402/07/01");
    // } else if (commentId == "C_3_2") {
    //     processServerResponse("COMMENT_DETAILS||C_3_2||user2||سارا احمدی||2||معمولی بود، انتظار "
    //                           "بیشتری داشتم||1402/07/05");
    // } else if (commentId == "C_3_3") {
    //     processServerResponse(
    //         "COMMENT_DETAILS||C_3_3||user3||محمد رضایی||4||کتاب خوبی بود، لذت بردم||1402/07/10");
    // }

    // پایان تست--------------------------------------------------------------------
}

void Bookdetails::processServerResponse(const QString &response)
{
    QStringList mainParts = response.split("||");
    if (mainParts.size() < 2)
        return;

    QString command = mainParts[0].trimmed();

    if (command == "BOOK_DETAILS_RESULT") {
        if (mainParts.size() >= 15) {
            currentUsername = mainParts[2].trimmed();
            QString imageBase64 = mainParts[3].trimmed();

            ui->label_bookname->setText(mainParts[4].trimmed());
            ui->label_anothername->setText(mainParts[5].trimmed());
            ui->label_publishername->setText(mainParts[6].trimmed());
            ui->label_genrename->setText(mainParts[7].trimmed());

            int mainPrice = mainParts[8].toInt();
            int discount = mainParts[9].toInt();
            int finalPrice = mainPrice - discount;

            ui->label_score->setText(mainParts[10].trimmed());
            ui->textBrowser_tozih->setText(mainParts[11].trimmed());

            if (discount == 0) {
                ui->label_priceasli->hide();
                ui->label_discount->hide();
                ui->label_10->hide();
                ui->label_12->hide();

                ui->label_pricenahaii->setText(QString::number(finalPrice) + " تومان");
            } else {
                ui->label_priceasli->show();
                ui->label_discount->show();
                ui->label_10->show();
                ui->label_12->show();

                ui->label_priceasli->setText(QString::number(mainPrice) + " تومان");
                ui->label_discount->setText(QString::number(discount) + " تومان تخفیف");
                ui->label_pricenahaii->setText(QString::number(finalPrice) + " تومان");
            }

            isInCart = (mainParts[12] == "1");
            isSaved = (mainParts[13] == "1");
            isOwned = (mainParts[14] == "1");

            loadAndSetImage(ui->label_bookpikcher, imageBase64);
            updateActionButtonsUI();
        }
    }

    else if (command == "COMMENTS_LIST") {
        QString data = mainParts[1].trimmed();
        if (data == "EMPTY" || data.isEmpty()) {
            return;
        }

        QStringList commentIds = data.split(",", Qt::SkipEmptyParts);
        for (int i = 0; i < commentIds.size(); ++i) {
            requestCommentDetails(commentIds[i].trimmed());
        }
    }

    else if (command == "COMMENT_DETAILS") {
        if (mainParts.size() >= 7) {
            QString commentUsername = mainParts[2].trimmed();
            QString displayName = mainParts[3].trimmed();
            int score = mainParts[4].toInt();
            QString text = mainParts[5].trimmed();
            QString date = mainParts[6].trimmed();

            if (commentUsername == currentUsername) {
                originalUserRating = score;
                originalUserComment = text;
                updateStarsUI(score);
                ui->textEdit_yourcomment->setText(text);
                updateCommentFormUI(true, false);
            } else {
                CommentcardWidget *commentCard = new CommentcardWidget(this);
                commentCard->setCommentData(displayName, score, text, date);
                ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1,
                                                 commentCard,
                                                 0,
                                                 Qt::AlignTop);
            }
        }
    }
}

void Bookdetails::loadAndSetImage(QLabel *imageLabel, const QString &base64Data)
{
    if (base64Data == "NO_IMAGE" || base64Data.isEmpty()) {
        imageLabel->setText("تصویری موجود نیست");
        return;
    }
    QByteArray byteArray = QByteArray::fromBase64(base64Data.toUtf8());
    QPixmap pixmap;
    if (pixmap.loadFromData(byteArray)) {
        imageLabel->setPixmap(pixmap);
        imageLabel->setScaledContents(true);
    } else {
        imageLabel->setText("خطا در عکس");
    }
}

void Bookdetails::updateCommentFormUI(bool hasCommented, bool isEditing)
{
    if (!hasCommented || isEditing) {
        ui->pushButton_commentadd->show();
        ui->pushButton_commentedit->hide();
        ui->pushButton_commentremove->setVisible(isEditing);

        ui->textEdit_yourcomment->setReadOnly(false);
    } else {
        ui->pushButton_commentadd->hide();
        ui->pushButton_commentedit->show();
        ui->pushButton_commentremove->show();

        ui->textEdit_yourcomment->setReadOnly(true);
    }
}

void Bookdetails::updateStarsUI(int score)
{
    currentUserRating = score;

    ui->toolButton_star1->setIcon(
        QIcon(score >= 1 ? ":/images/icons8-star-48 (1).png" : ":/images/icons8-star-48 (2).png"));
    ui->toolButton_star2->setIcon(
        QIcon(score >= 2 ? ":/images/icons8-star-48 (1).png" : ":/images/icons8-star-48 (2).png"));
    ui->toolButton_star3->setIcon(
        QIcon(score >= 3 ? ":/images/icons8-star-48 (1).png" : ":/images/icons8-star-48 (2).png"));
    ui->toolButton_star4->setIcon(
        QIcon(score >= 4 ? ":/images/icons8-star-48 (1).png" : ":/images/icons8-star-48 (2).png"));
    ui->toolButton_star5->setIcon(
        QIcon(score >= 5 ? ":/images/icons8-star-48 (1).png" : ":/images/icons8-star-48 (2).png"));
}
void Bookdetails::on_toolButton_star1_clicked()
{
    if (ui->textEdit_yourcomment->isReadOnly())
        return;
    updateStarsUI(1);
}
void Bookdetails::on_toolButton_star2_clicked()
{
    if (ui->textEdit_yourcomment->isReadOnly())
        return;
    updateStarsUI(2);
}
void Bookdetails::on_toolButton_star3_clicked()
{
    if (ui->textEdit_yourcomment->isReadOnly())
        return;
    updateStarsUI(3);
}
void Bookdetails::on_toolButton_star4_clicked()
{
    if (ui->textEdit_yourcomment->isReadOnly())
        return;
    updateStarsUI(4);
}
void Bookdetails::on_toolButton_star5_clicked()
{
    if (ui->textEdit_yourcomment->isReadOnly())
        return;
    updateStarsUI(5);
}

void Bookdetails::on_pushButton_commentadd_clicked()
{
    QString currentText = ui->textEdit_yourcomment->toPlainText().trimmed();

    if (currentUserRating == originalUserRating && currentText == originalUserComment) {
        updateCommentFormUI(true, false);
        return;
    }

    QString message = QString("UPDATE_COMMENT||%1||%2||%3")
                          .arg(currentBookId, QString::number(currentUserRating), currentText);
    m_client->sendMessage(message);

    originalUserRating = currentUserRating;
    originalUserComment = currentText;
    updateCommentFormUI(true, false);
}

void Bookdetails::on_pushButton_commentedit_clicked()
{
    updateCommentFormUI(true, true);
}

void Bookdetails::on_pushButton_commentremove_clicked()
{
    QString message = "DELETE_COMMENT||" + currentBookId;
    m_client->sendMessage(message);

    originalUserRating = 5;
    originalUserComment = "";
    ui->textEdit_yourcomment->clear();
    updateStarsUI(5);
    updateCommentFormUI(false, false);
}

void Bookdetails::updateActionButtonsUI()
{
    if (isOwned) {
        ui->pushButton_cartadd->hide();
        ui->pushButton_cartremove->hide();
    } else {
        if (isInCart) {
            ui->pushButton_cartadd->hide();
            ui->pushButton_cartremove->show();
        } else {
            ui->pushButton_cartadd->show();
            ui->pushButton_cartremove->hide();
        }
    }
    if (isSaved) {
        ui->pushButton_savelibrary->hide();
        ui->pushButton_removelibrary->show();
    } else {
        ui->pushButton_savelibrary->show();
        ui->pushButton_removelibrary->hide();
    }
}

void Bookdetails::on_pushButton_cartadd_clicked()
{
    QString message = "ADD_CART||" + currentBookId;
    m_client->sendMessage(message);

    isInCart = true;
    updateActionButtonsUI();
}

void Bookdetails::on_pushButton_cartremove_clicked()
{
    QString message = "REMOVE_CART||" + currentBookId;
    m_client->sendMessage(message);

    isInCart = false;
    updateActionButtonsUI();
}

void Bookdetails::on_pushButton_savelibrary_clicked()
{
    QString message = "SAVE_BOOK||" + currentBookId;
    m_client->sendMessage(message);

    isSaved = true;
    updateActionButtonsUI();
}

void Bookdetails::on_pushButton_removelibrary_clicked()
{
    QString message = "UNSAVE_BOOK||" + currentBookId;
    m_client->sendMessage(message);

    isSaved = false;
    updateActionButtonsUI();
}