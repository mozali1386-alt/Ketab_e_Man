#include "admindashboard.h"
#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QQuickItem>
#include <QTimer>
#include <QVBoxLayout>
#include "editbookdialog.h"
#include "pdfviewerwidget.h"
#include "ui_admindashboard.h"

Admindashboard::Admindashboard(ClientSocketManager *client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Admindashboard)
    , m_client(client)
{
    ui->setupUi(this);

    ui->statusbar->setLayoutDirection(Qt::RightToLeft);
    ui->statusbar->setStyleSheet("color: red; font-weight: bold;");

    ui->tabWidget->blockSignals(true);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->blockSignals(false);

    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/AdminStats.qml")));
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->quickWidget->setClearColor(Qt::transparent);

    notifPopup = new Notification(this);
    connect(notifPopup,
            &Notification::notificationClicked,
            this,
            &Admindashboard::onNotificationClicked);
    connect(notifPopup,
            &Notification::markAllReadClicked,
            this,
            &Admindashboard::onMarkAllReadClicked);

    QObject *qmlRoot = ui->quickWidget->rootObject();
    if (qmlRoot) {
        connect(qmlRoot, SIGNAL(bellClicked()), this, SLOT(onBellClicked()));
    }

    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &Admindashboard::processServerResponse);

    ui->tableWidget_allusers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_allusers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_allusers->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_allusers->setColumnCount(6);
    ui->tableWidget_allusers->setHorizontalHeaderLabels(
        {"کاربر/انتشار", "نام کاربری", "نقش", "ایمیل", "تاریخ ثبت نام", "وضعیت"});

    QString commonStyle = "QTableWidget::item:selected { background-color: #D6EAF8; color: black; }"
                          "QHeaderView::section:checked { background-color: #E74C3C; color: white; "
                          "font-weight: bold; }";
    ui->tableWidget_allusers->setStyleSheet(commonStyle);

    ui->pushButton_deletehesab->setEnabled(false);
    ui->pushButton_block->setEnabled(false);
    ui->pushButton_unblock->setEnabled(false);
    ui->pushButton_unblock->setVisible(false);

    ui->tableWidget_books->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_books->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_books->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_books->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_books->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_books->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_books->setColumnCount(6);
    ui->tableWidget_books->setHorizontalHeaderLabels(
        {"نام کتاب", "نام نویسنده", "ناشر", "ژانر", "توضیحات کتاب", "وضعیت انتشار"});
    ui->tableWidget_books->setStyleSheet(commonStyle);

    ui->pushButton_deletebook->setEnabled(false);
    ui->pushButton_editbook->setEnabled(false);
    ui->pushButton_pikther->setEnabled(false);
    ui->pushButton_pdf->setEnabled(false);

    ui->tableWidget_comment->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_comment->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_comment->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_comment->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_comment->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_comment->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_comment->setColumnCount(4);
    ui->tableWidget_comment->setHorizontalHeaderLabels(
        {"نام کاربر", "نام کتاب", "متن نظر", "تاریخ ثبت"});
    ui->tableWidget_comment->setStyleSheet(commonStyle);

    ui->pushButton_deletecomment->setEnabled(false);
    ui->pushButton_textcomelcomment->setEnabled(false);

    on_tabWidget_currentChanged(0);
}

Admindashboard::~Admindashboard()
{
    delete ui;
}

QString Admindashboard::translateGenreToPersian(const QString &englishGenre)
{
    if (englishGenre == "ROMANCE")
        return "عاشقانه";
    if (englishGenre == "CRIME")
        return "جنایی";
    if (englishGenre == "SCIFI")
        return "علمی تخیلی";
    if (englishGenre == "HORROR")
        return "ترسناک";
    if (englishGenre == "CLASSIC")
        return "کلاسیک";
    if (englishGenre == "ART")
        return "هنری";
    if (englishGenre == "COMEDY")
        return "طنز (کمدی)";
    if (englishGenre == "HISTORY")
        return "تاریخی";
    return englishGenre;
}

QString Admindashboard::translateGenreToEnglish(const QString &persianGenre)
{
    if (persianGenre == "عاشقانه")
        return "ROMANCE";
    if (persianGenre == "جنایی")
        return "CRIME";
    if (persianGenre == "علمی تخیلی")
        return "SCIFI";
    if (persianGenre == "ترسناک")
        return "HORROR";
    if (persianGenre == "کلاسیک")
        return "CLASSIC";
    if (persianGenre == "هنری")
        return "ART";
    if (persianGenre == "طنز (کمدی)")
        return "COMEDY";
    if (persianGenre == "تاریخی")
        return "HISTORY";
    return persianGenre;
}
void Admindashboard::processServerResponse(const QString &response)
{
    const QStringList parts = response.split("||", Qt::KeepEmptyParts);
    if (parts.isEmpty())
        return;
    QString cmd = parts[0];

    if (cmd == "USERS_LIST_RESULT") {
        if (parts.size() >= 3 && parts[1].toInt() > 0) {
            QString idString = parts[2].trimmed();
            if (!idString.isEmpty()) {
                const QStringList ids = idString.split(",", Qt::SkipEmptyParts);
                for (const QString &id : ids) {
                    requestUserInfo(id);
                }
            }
        }
    } else if (cmd == "USER_INFO_RESULT") {
        if (parts.size() < 8)
            return;
        QString userId = parts[1], displayName = parts[2], username = parts[3];

        QString role = parts[4], email = parts[5], regDate = parts[6], statusCode = parts[7];

        int row = ui->tableWidget_allusers->rowCount();
        ui->tableWidget_allusers->insertRow(row);

        QString displayRole = (role == "PUBLISHER") ? "ناشر" : "کاربر عادی";
        QString displayStatus = (statusCode == "UNBLOCK") ? "فعال" : "مسدود";

        QTableWidgetItem *itemDisplayName = new QTableWidgetItem(displayName);
        itemDisplayName->setData(Qt::UserRole, userId);
        QTableWidgetItem *itemUsername = new QTableWidgetItem(username);
        QTableWidgetItem *itemRole = new QTableWidgetItem(displayRole);
        QTableWidgetItem *itemEmail = new QTableWidgetItem(email);
        QTableWidgetItem *itemRegDate = new QTableWidgetItem(regDate);
        QTableWidgetItem *itemStatus = new QTableWidgetItem(displayStatus);
        itemStatus->setData(Qt::UserRole, statusCode);

        ui->tableWidget_allusers->setItem(row, 0, itemDisplayName);
        ui->tableWidget_allusers->setItem(row, 1, itemUsername);
        ui->tableWidget_allusers->setItem(row, 2, itemRole);
        ui->tableWidget_allusers->setItem(row, 3, itemEmail);
        ui->tableWidget_allusers->setItem(row, 4, itemRegDate);
        ui->tableWidget_allusers->setItem(row, 5, itemStatus);
    } else if (cmd == "DELETE_USER_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            int row = findUserRowById(parts[1]);
            if (row != -1) {
                ui->tableWidget_allusers->removeRow(row);
                ui->tableWidget_allusers->clearSelection();
            }
        }
    } else if (cmd == "BLOCK_USER_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            int row = findUserRowById(parts[1]);
            if (row != -1) {
                ui->tableWidget_allusers->item(row, 5)->setText("مسدود");
                ui->tableWidget_allusers->item(row, 5)->setData(Qt::UserRole, "BLOCK");
                on_tableWidget_allusers_itemSelectionChanged();
            }
        }
    } else if (cmd == "UNBLOCK_USER_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            int row = findUserRowById(parts[1]);
            if (row != -1) {
                ui->tableWidget_allusers->item(row, 5)->setText("فعال");
                ui->tableWidget_allusers->item(row, 5)->setData(Qt::UserRole, "UNBLOCK");
                on_tableWidget_allusers_itemSelectionChanged();
            }
        }
    } else if (cmd == "ALL_BOOKS_ADMIN_RESULT") {
        if (parts.size() >= 3 && parts[1].toInt() > 0) {
            QString idString = parts[2].trimmed();
            if (!idString.isEmpty()) {
                const QStringList ids = idString.split(",", Qt::SkipEmptyParts);
                for (const QString &id : ids) {
                    QString req = "GET_BOOK_INFO_ADMIN||" + id;
                    m_client->sendMessage(req);
                }
            }
        }
    } else if (cmd == "BOOK_INFO_ADMIN_RESULT") {
        if (parts.size() < 8)
            return;
        QString bookId = parts[1], bookName = parts[2], authorName = parts[3];
        QString publisherName = parts[4], genreEn = parts[5], description = parts[6].trimmed();
        QString statusEn = parts[7].trimmed();
        QString statusFa = (statusEn == "ACTIVE") ? "فعال" : "غیر فعال";
        QString genreFa = translateGenreToPersian(genreEn);

        QString displayDesc = description;
        if (displayDesc.isEmpty()) {
            displayDesc = "بدون توضیح";
        } else if (displayDesc.length() > 25) {
            displayDesc = displayDesc.left(25) + "...";
        }

        int row = ui->tableWidget_books->rowCount();
        ui->tableWidget_books->insertRow(row);

        QTableWidgetItem *itemBookName = new QTableWidgetItem(bookName);
        itemBookName->setData(Qt::UserRole, bookId);
        QTableWidgetItem *itemAuthor = new QTableWidgetItem(authorName);
        QTableWidgetItem *itemPub = new QTableWidgetItem(publisherName);
        QTableWidgetItem *itemGenre = new QTableWidgetItem(genreFa);
        itemGenre->setData(Qt::UserRole + 1, genreEn);
        QTableWidgetItem *itemDesc = new QTableWidgetItem(displayDesc);
        itemDesc->setData(Qt::UserRole + 1, description);
        QTableWidgetItem *itemStatus = new QTableWidgetItem(statusFa);
        itemStatus->setData(Qt::UserRole + 1, statusEn);

        ui->tableWidget_books->setItem(row, 0, itemBookName);
        ui->tableWidget_books->setItem(row, 1, itemAuthor);
        ui->tableWidget_books->setItem(row, 2, itemPub);
        ui->tableWidget_books->setItem(row, 3, itemGenre);
        ui->tableWidget_books->setItem(row, 4, itemDesc);
        ui->tableWidget_books->setItem(row, 5, itemStatus);
    } else if (cmd == "DELETE_BOOK_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            for (int i = 0; i < ui->tableWidget_books->rowCount(); ++i) {
                if (ui->tableWidget_books->item(i, 0)->data(Qt::UserRole).toString() == parts[1]) {
                    ui->tableWidget_books->removeRow(i);
                    ui->tableWidget_books->clearSelection();
                    break;
                }
            }
        }
    } else if (cmd == "EDIT_BOOK_ADMIN_RESULT") {
        if (parts.size() >= 7 && parts[2] == "SUCCESS") {
            QString targetId = parts[1];
            QString newName = parts[3];
            QString newAuthor = parts[4];
            QString newGenreEn = parts[5];
            QString newDesc = parts[6];

            QString newGenreFa = translateGenreToPersian(newGenreEn);
            QString displayDesc = newDesc.isEmpty()
                                      ? "بدون توضیح"
                                      : (newDesc.length() > 25 ? newDesc.left(25) + "..." : newDesc);

            for (int i = 0; i < ui->tableWidget_books->rowCount(); ++i) {
                if (ui->tableWidget_books->item(i, 0)->data(Qt::UserRole).toString() == targetId) {
                    ui->tableWidget_books->item(i, 0)->setText(newName);
                    ui->tableWidget_books->item(i, 1)->setText(newAuthor);
                    ui->tableWidget_books->item(i, 3)->setText(newGenreFa);
                    ui->tableWidget_books->item(i, 3)->setData(Qt::UserRole + 1, newGenreEn);
                    ui->tableWidget_books->item(i, 4)->setText(displayDesc);
                    ui->tableWidget_books->item(i, 4)->setData(Qt::UserRole + 1, newDesc);
                    on_tableWidget_books_itemSelectionChanged();
                    break;
                }
            }
            QMessageBox::information(this, "موفق", "ویرایش کتاب با موفقیت انجام شد.");
        }
    } else if (cmd == "ALL_COMMENTS_ADMIN_RESULT") {
        if (parts.size() >= 3 && parts[1].toInt() > 0) {
            QString idString = parts[2].trimmed();
            if (!idString.isEmpty()) {
                const QStringList ids = idString.split(",", Qt::SkipEmptyParts);
                for (const QString &id : ids) {
                    QString req = "GET_COMMENT_INFO||" + id;
                    m_client->sendMessage(req);
                }
            }
        }
    } else if (cmd == "COMMENT_INFO_RESULT") {
        if (parts.size() < 6)
            return;
        QString cId = parts[1], uName = parts[2], bName = parts[3];
        QString text = parts[4].trimmed();
        QString date = parts[5];

        QString displayText = text.isEmpty() ? "بدون متن"
                                             : (text.length() > 25 ? text.left(25) + "..." : text);

        int row = ui->tableWidget_comment->rowCount();
        ui->tableWidget_comment->insertRow(row);

        QTableWidgetItem *iUser = new QTableWidgetItem(uName);
        iUser->setData(Qt::UserRole, cId);
        QTableWidgetItem *iBook = new QTableWidgetItem(bName);
        QTableWidgetItem *iText = new QTableWidgetItem(displayText);
        iText->setData(Qt::UserRole + 1, text);
        QTableWidgetItem *iDate = new QTableWidgetItem(date);

        ui->tableWidget_comment->setItem(row, 0, iUser);
        ui->tableWidget_comment->setItem(row, 1, iBook);
        ui->tableWidget_comment->setItem(row, 2, iText);
        ui->tableWidget_comment->setItem(row, 3, iDate);
    } else if (cmd == "DELETE_COMMENT_ADMIN_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            for (int i = 0; i < ui->tableWidget_comment->rowCount(); ++i) {
                if (ui->tableWidget_comment->item(i, 0)->data(Qt::UserRole).toString() == parts[1]) {
                    ui->tableWidget_comment->removeRow(i);
                    ui->tableWidget_comment->clearSelection();
                    break;
                }
            }
        }
    } else if (cmd == "BOOK_COVER_RESULT") {
        if (parts.size() < 3)
            return;

        QString bookId = parts[1];
        QByteArray imageData = QByteArray::fromBase64(parts[2].toUtf8());

        if (imageData.isEmpty()) {
            QMessageBox::warning(this, "خطا", "داده تصویر جلد خالی یا نامعتبر است.");
            return;
        }

        QPixmap pixmap;
        if (!pixmap.loadFromData(imageData)) {
            QMessageBox::warning(this, "خطا", "تصویر جلد قابل نمایش نیست.");
            return;
        }
        QDialog *dlg = new QDialog(this);
        dlg->setWindowTitle("جلد کتاب");
        dlg->setAttribute(Qt::WA_DeleteOnClose);

        QLabel *label = new QLabel(dlg);
        label->setPixmap(pixmap.scaled(400, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *layout = new QVBoxLayout(dlg);
        layout->addWidget(label);

        dlg->resize(450, 650);
        dlg->show();
    }

    else if (cmd == "PDF_START") {
        if (parts.size() >= 3) {
            currentPdfBookId = parts[1];
            int totalChunks = parts[2].toInt();

            pdfLoadingDialog = new QProgressDialog("در حال دریافت فایل PDF...",
                                                   "لغو",
                                                   0,
                                                   totalChunks,
                                                   this);
            pdfLoadingDialog->setWindowTitle("لطفاً صبر کنید");
            pdfLoadingDialog->setWindowModality(Qt::WindowModal);
            pdfLoadingDialog->setValue(0);
            pdfLoadingDialog->show();

            currentPdfBuffer.clear();
            //  مفید برای فایل‌های بزرگ
            currentPdfBuffer.reserve(totalChunks * 500 * 1024);
        }
    } else if (cmd == "PDF_CHUNK") {
        if (parts.size() >= 3) {
            // هر تکه را جداگانه دیکد کن و باینری را append کن
            QByteArray binaryChunk = QByteArray::fromBase64(parts[2].toUtf8());
            if (!binaryChunk.isEmpty()) {
                currentPdfBuffer.append(binaryChunk);
            }

            if (pdfLoadingDialog)
                pdfLoadingDialog->setValue(pdfLoadingDialog->value() + 1);
        }
    } else if (cmd == "PDF_END") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            if (pdfLoadingDialog) {
                pdfLoadingDialog->close();
                delete pdfLoadingDialog;
                pdfLoadingDialog = nullptr;
            }

            QByteArray finalPdfData = currentPdfBuffer;

            if (finalPdfData.isEmpty()) {
                QMessageBox::critical(this, "خطا", "داده‌های PDF خالی است.");
                currentPdfBuffer.clear();
                currentPdfBookId.clear();
                return;
            }

            pdfviewerWidget *viewer = new pdfviewerWidget();
            viewer->setAttribute(Qt::WA_DeleteOnClose);

            if (viewer->loadPdfFromData(finalPdfData)) {
                viewer->showMaximized();
                // صفحه اول (۱-based)
                QTimer::singleShot(50, viewer, [viewer]() { viewer->jumpToPage(1); });
            } else {
                delete viewer;
            }

            currentPdfBuffer.clear();
            currentPdfBookId.clear();
        }
    } else if (cmd == "NOTIFICATION_INFO_RESULT") {
        if (parts.size() >= 5) {
            QString notifId = parts[1];
            QString msg = parts[2];
            QString date = parts[3];
            QString status = parts[4];
            bool isRead = (status == "READ");

            if (notifPopup)
                notifPopup->addNotification(notifId, msg, date, isRead);

            QObject *qmlRoot = ui->quickWidget->rootObject();
            if (qmlRoot && !isRead) {
                int currentUnread = qmlRoot->property("unreadCount").toInt();
                qmlRoot->setProperty("unreadCount", currentUnread + 1);
            }
        }
    } else if (cmd == "NEW_NOTIFICATION_PUSH") {
        if (parts.size() >= 5) {
            QString notifId = parts[1];
            QString msg = parts[2];
            QString date = parts[3];
            QString status = parts[4];
            bool isRead = (status == "READ");

            if (notifPopup)
                notifPopup->addNotification(notifId, msg, date, isRead);

            QObject *qmlRoot = ui->quickWidget->rootObject();
            if (qmlRoot && !isRead) {
                int currentUnread = qmlRoot->property("unreadCount").toInt();
                qmlRoot->setProperty("unreadCount", currentUnread + 1);
                ui->statusbar->showMessage("اعلان جدید: " + msg, 5000);
                QApplication::beep();
            }
        }
    } else if (cmd == "UPDATE_ADMIN_STATS") {
        if (parts.size() >= 3) {
            QObject *qmlRoot = ui->quickWidget->rootObject();
            if (qmlRoot) {
                qmlRoot->setProperty("onlineUsers", parts[1].toInt());
                qmlRoot->setProperty("totalUsers", parts[2].toInt());
            }
        }
    } else if (cmd == "MARK_READ_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            QString notifId = parts[1];

            QObject *qmlRoot = ui->quickWidget->rootObject();
            if (qmlRoot) {
                int currentUnread = qmlRoot->property("unreadCount").toInt();
                if (currentUnread > 0)
                    qmlRoot->setProperty("unreadCount", currentUnread - 1);
            }

            if (notifPopup) {
                notifPopup->markAsReadUI(notifId);
            }
        }
    } else if (cmd == "MARK_ALL_READ_RESULT") {
        if (parts.size() >= 2 && parts[1] == "SUCCESS") {
            QObject *qmlRoot = ui->quickWidget->rootObject();
            if (qmlRoot) {
                qmlRoot->setProperty("unreadCount", 0);
            }
            if (notifPopup) {
                notifPopup->markAllAsReadUI();
            }
        }
    }
}

void Admindashboard::on_pushButton_search_clicked()
{
    ui->tableWidget_allusers->setRowCount(0);
    QString nameFilter = ui->lineEdit_name->text().trimmed();
    if (nameFilter.isEmpty())
        nameFilter = "EMPTY";

    QString roleText = ui->comboBox_filterusers->currentText();
    QString roleFilter = "ALL";
    if (roleText == "ناشران")
        roleFilter = "PUBLISHER";
    else if (roleText == "کاربران عادی")
        roleFilter = "NORMAL";

    QString statusText = ui->comboBox_filterstatus->currentText();
    QString statusFilter = "ALL";
    if (statusText == "فعال")
        statusFilter = "UNBLOCK";
    else if (statusText == "مسدود")
        statusFilter = "BLOCK";

    QString message = "SEARCH_USERS||" + nameFilter + "||" + roleFilter + "||" + statusFilter;
    m_client->sendMessage(message);
}

void Admindashboard::requestUserInfo(const QString &userId)
{
    QString message = "GET_USER_INFO||" + userId;
    m_client->sendMessage(message);
}

int Admindashboard::findUserRowById(const QString &userId)
{
    for (int i = 0; i < ui->tableWidget_allusers->rowCount(); ++i) {
        if (ui->tableWidget_allusers->item(i, 0)->data(Qt::UserRole).toString() == userId)
            return i;
    }
    return -1;
}

void Admindashboard::on_tableWidget_allusers_itemSelectionChanged()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget_allusers->selectedItems();
    if (selectedItems.isEmpty()) {
        ui->pushButton_deletehesab->setEnabled(false);
        ui->pushButton_block->setEnabled(false);
        ui->pushButton_block->setVisible(true);
        ui->pushButton_unblock->setEnabled(false);
        ui->pushButton_unblock->setVisible(false);
        return;
    }
    int row = selectedItems.first()->row();
    QString statusCode = ui->tableWidget_allusers->item(row, 5)->data(Qt::UserRole).toString();

    ui->pushButton_deletehesab->setEnabled(true);
    if (statusCode == "UNBLOCK") {
        ui->pushButton_block->setVisible(true);
        ui->pushButton_block->setEnabled(true);
        ui->pushButton_unblock->setVisible(false);
        ui->pushButton_unblock->setEnabled(false);
    } else if (statusCode == "BLOCK") {
        ui->pushButton_unblock->setVisible(true);
        ui->pushButton_unblock->setEnabled(true);
        ui->pushButton_block->setVisible(false);
        ui->pushButton_block->setEnabled(false);
    }
}

void Admindashboard::on_pushButton_deletehesab_clicked()
{
    int row = ui->tableWidget_allusers->currentRow();
    if (row < 0)
        return;
    QString userId = ui->tableWidget_allusers->item(row, 0)->data(Qt::UserRole).toString();
    QString message = "DELETE_USER||" + userId;
    m_client->sendMessage(message);
}

void Admindashboard::on_pushButton_block_clicked()
{
    int row = ui->tableWidget_allusers->currentRow();
    if (row < 0)
        return;
    QString userId = ui->tableWidget_allusers->item(row, 0)->data(Qt::UserRole).toString();
    QString message = "BLOCK_USER||" + userId;
    m_client->sendMessage(message);
}

void Admindashboard::on_pushButton_unblock_clicked()
{
    int row = ui->tableWidget_allusers->currentRow();
    if (row < 0)
        return;
    QString userId = ui->tableWidget_allusers->item(row, 0)->data(Qt::UserRole).toString();
    QString message = "UNBLOCK_USER||" + userId;
    m_client->sendMessage(message);
}

void Admindashboard::requestAdminBooks()
{
    QString req = "GET_ALL_BOOKS_ADMIN";
    m_client->sendMessage(req);
}

void Admindashboard::requestAdminComments()
{
    QString req = "GET_ALL_COMMENTS_ADMIN";
    m_client->sendMessage(req);
}

void Admindashboard::on_tableWidget_books_itemSelectionChanged()
{
    bool hasSelection = !ui->tableWidget_books->selectedItems().isEmpty();
    ui->pushButton_deletebook->setEnabled(hasSelection);
    ui->pushButton_editbook->setEnabled(hasSelection);
    ui->pushButton_pikther->setEnabled(hasSelection);
    ui->pushButton_pdf->setEnabled(hasSelection);
}

void Admindashboard::on_pushButton_deletebook_clicked()
{
    int row = ui->tableWidget_books->currentRow();
    if (row < 0)
        return;
    QString bookId = ui->tableWidget_books->item(row, 0)->data(Qt::UserRole).toString();
    QString req = "DELETE_BOOK||" + bookId;
    m_client->sendMessage(req);
}

void Admindashboard::on_pushButton_editbook_clicked()
{
    int row = ui->tableWidget_books->currentRow();
    if (row < 0)
        return;
    QString bookId = ui->tableWidget_books->item(row, 0)->data(Qt::UserRole).toString();
    QString oldName = ui->tableWidget_books->item(row, 0)->text();
    QString oldAuthor = ui->tableWidget_books->item(row, 1)->text();
    QString oldGenreFa = ui->tableWidget_books->item(row, 3)->text();
    QString oldDesc = ui->tableWidget_books->item(row, 4)->data(Qt::UserRole + 1).toString();

    EditbookDialog dialog(this);
    dialog.setBookData(oldName, oldAuthor, oldGenreFa, oldDesc);

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.getBookName();
        QString newAuthor = dialog.getAuthorName();
        QString newGenreFa = dialog.getGenreFa();
        QString newDesc = dialog.getDescription();
        QString newGenreEn = translateGenreToEnglish(newGenreFa);

        if (newName == oldName && newAuthor == oldAuthor && newDesc == oldDesc
            && newGenreFa == oldGenreFa) {
            QMessageBox::information(
                this, "بدون تغییر", "شما هیچ فیلدی را تغییر نداده‌اید.");
            return;
        }

        QString req = "EDIT_BOOK_ADMIN||" + bookId + "||" + newName + "||" + newAuthor + "||"
                      + newGenreEn + "||" + newDesc;
        m_client->sendMessage(req);
    }
}

void Admindashboard::on_pushButton_pikther_clicked()
{
    int row = ui->tableWidget_books->currentRow();
    if (row < 0)
        return;
    QString bookId = ui->tableWidget_books->item(row, 0)->data(Qt::UserRole).toString();
    QString req = "GET_BOOK_COVER||" + bookId;
    m_client->sendMessage(req);
}

void Admindashboard::on_pushButton_pdf_clicked()
{
    int row = ui->tableWidget_books->currentRow();
    if (row < 0)
        return;

    QString bookId = ui->tableWidget_books->item(row, 0)->data(Qt::UserRole).toString();
    QString req = "DOWNLOAD_PDF_ADMIN||" + bookId;
    m_client->sendMessage(req);
}
void Admindashboard::on_tableWidget_comment_itemSelectionChanged()
{
    bool hasSelection = !ui->tableWidget_comment->selectedItems().isEmpty();
    ui->pushButton_deletecomment->setEnabled(hasSelection);
    ui->pushButton_textcomelcomment->setEnabled(hasSelection);
}

void Admindashboard::on_pushButton_deletecomment_clicked()
{
    int row = ui->tableWidget_comment->currentRow();
    if (row < 0)
        return;
    QString cId = ui->tableWidget_comment->item(row, 0)->data(Qt::UserRole).toString();

    QString req = "DELETE_COMMENT_ADMIN||" + cId;
    m_client->sendMessage(req);
}

void Admindashboard::on_pushButton_textcomelcomment_clicked()
{
    int row = ui->tableWidget_comment->currentRow();
    if (row < 0)
        return;
    QString fullText = ui->tableWidget_comment->item(row, 2)->data(Qt::UserRole + 1).toString();
    QMessageBox::information(this,
                             "متن کامل نظر",
                             fullText.isEmpty() ? "این نظر متنی ندارد." : fullText);
}

void Admindashboard::onBellClicked()
{
    if (notifPopup->isVisible()) {
        notifPopup->close();
        return;
    }
    QPoint globalPos = this->mapToGlobal(QPoint(20, 80));
    notifPopup->move(globalPos);
    notifPopup->show();
}

void Admindashboard::onNotificationClicked(const QString &notifId)
{
    QString req = "MARK_NOTIFICATION_READ||" + notifId;
    m_client->sendMessage(req);
}

void Admindashboard::onMarkAllReadClicked()
{
    QString req = "MARK_ALL_NOTIFICATIONS_READ";
    m_client->sendMessage(req);
}

void Admindashboard::closeEvent(QCloseEvent *event)
{
    m_client->sendMessage("LOGOUT");
    // اجازه می‌دهیم برنامه روال عادی بسته شدن خود را طی کند
    event->accept();
}

void Admindashboard::on_tabWidget_currentChanged(int index)
{
    if (index == 0) {
        on_pushButton_search_clicked();
    } else if (index == 1) {
        ui->tableWidget_books->setRowCount(0);
        ui->tableWidget_comment->setRowCount(0);
        // درخواست اطلاعات جدید برای هر دو جدول
        requestAdminBooks();
        requestAdminComments();
    }
}