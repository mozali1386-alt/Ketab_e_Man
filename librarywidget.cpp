#include "librarywidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QTimer>
#include "bookdetails.h"
#include "pdfviewerwidget.h"
#include "ui_librarywidget.h"

LibraryWidget::LibraryWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
    setupLayouts();

    requestShelves();
    requestMyBooks();
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::setupLayouts()
{
    if (ui->scrollAreaWidgetContents->layout())
        delete ui->scrollAreaWidgetContents->layout();
    if (ui->scrollAreaWidgetContents_2->layout())
        delete ui->scrollAreaWidgetContents_2->layout();
    if (ui->scrollAreaWidgetContents_3->layout())
        delete ui->scrollAreaWidgetContents_3->layout();

    myBooksLayout = new QGridLayout(ui->scrollAreaWidgetContents);
    myBooksLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    savedBooksLayout = new QGridLayout(ui->scrollAreaWidgetContents_2);
    savedBooksLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    shelvesBooksLayout = new QGridLayout(ui->scrollAreaWidgetContents_3);
    shelvesBooksLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void LibraryWidget::clearLayout(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
            delete child->widget();
        }
        delete child;
    }
}

void LibraryWidget::addBookToGrid(BookLibraryItem *item, QGridLayout *layout, int maxColumns)
{
    int count = layout->count();
    int row = count / maxColumns;
    int col = count % maxColumns;
    layout->addWidget(item, row, col);
}

void LibraryWidget::on_tabWidget_currentChanged(int index)
{
    if (index == 0) {
        requestShelves();
        requestMyBooks();
    } else if (index == 1) {
        requestSavedBooks();
    } else if (index == 2) {
        requestShelves();
    }
}

// ==========================================
// ارسال درخواست‌های سرور (کلاینت)
// ==========================================
void LibraryWidget::requestMyBooks()
{
    QString message = "GET_MY_BOOKS";
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() { processServerResponse("MY_BOOKS_RESULT||2||101,102"); });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::requestSavedBooks()
{
    QString message = "GET_SAVED_BOOKS";
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() {
        processServerResponse("SAVED_BOOKS_RESULT||2||103,104");
    });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::requestShelves()
{
    QString message = "GET_SHELVES";
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() {
        processServerResponse("SHELVES_RESULT||2||1:اکشن,2:علمی");
    });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::requestShelfBooks(const QString &shelfId)
{
    if (shelfId.isEmpty())
        return;
    QString message = "GET_SHELF_BOOKS||" + shelfId;
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() {
        processServerResponse("SHELF_BOOKS_RESULT||" + shelfId + "||101");
    });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::requestLibraryBookInfo(const QString &bookId, BookLibraryItem::TabMode mode)
{
    QString message;
    QString mockCmd;

    if (mode == BookLibraryItem::MyBooks) {
        message = "GET_MY_BOOK_INFO||" + bookId;
        mockCmd = "MY_BOOK_INFO_RESULT";
    } else if (mode == BookLibraryItem::SavedBooks) {
        message = "GET_SAVED_BOOK_INFO||" + bookId;
        mockCmd = "SAVED_BOOK_INFO_RESULT";
    } else if (mode == BookLibraryItem::MyShelves) {
        message = "GET_SHELF_BOOK_INFO||" + bookId;
        mockCmd = "SHELF_BOOK_INFO_RESULT";
    }

    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QString mockResponse;
    if (mode == BookLibraryItem::SavedBooks) {
        // پروتکل جدید ذخیره‌شده‌ها: بدون ShelfID، فقط شامل IsPurchased
        QString isPurchased = (bookId == "103") ? "TRUE" : "FALSE";
        mockResponse = mockCmd + "||" + bookId + "||BASE64_IMAGE||کتاب تستی " + bookId + "||"
                       + isPurchased;
    } else {
        // پروتکل کتاب‌ها و قفسه‌ها: شامل ShelfID
        mockResponse = mockCmd + "||" + bookId + "||BASE64_IMAGE||کتاب تستی " + bookId + "||1";
    }

    QTimer::singleShot(100, this, [=]() { processServerResponse(mockResponse); });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

// ==========================================
// پردازش پیام‌های سرور
// ==========================================
void LibraryWidget::processServerResponse(const QString &response)
{
    const QStringList parts = response.split("||");
    if (parts.isEmpty())
        return;
    QString cmd = parts[0];

    if (cmd == "MY_BOOKS_RESULT") {
        clearLayout(myBooksLayout);
        if (parts.size() >= 3 && parts[1] != "0") {
            const QStringList ids = parts[2].split(",", Qt::SkipEmptyParts);
            for (const QString &id : ids)
                requestLibraryBookInfo(id, BookLibraryItem::MyBooks);
        }
    } else if (cmd == "SAVED_BOOKS_RESULT") {
        clearLayout(savedBooksLayout);
        if (parts.size() >= 3 && parts[1] != "0") {
            const QStringList ids = parts[2].split(",", Qt::SkipEmptyParts);
            for (const QString &id : ids)
                requestLibraryBookInfo(id, BookLibraryItem::SavedBooks);
        }
    } else if (cmd == "SHELVES_RESULT") {
        userShelves.clear();
        if (parts.size() >= 3 && parts[1] != "0") {
            const QStringList shelves = parts[2].split(",", Qt::SkipEmptyParts);
            for (const QString &s : shelves) {
                QStringList sData = s.split(":");
                if (sData.size() == 2)
                    userShelves[sData[0]] = sData[1];
            }
        }
        updateComboBox();
    } else if (cmd == "SHELF_BOOKS_RESULT") {
        clearLayout(shelvesBooksLayout);
        if (parts.size() >= 3 && parts[1] != "EMPTY") {
            const QStringList ids = parts[2].split(",", Qt::SkipEmptyParts);
            for (const QString &id : ids)
                requestLibraryBookInfo(id, BookLibraryItem::MyShelves);
        }
    } else if (cmd == "MY_BOOK_INFO_RESULT" || cmd == "SHELF_BOOK_INFO_RESULT") {
        if (parts.size() < 5)
            return;

        QString bookId = parts[1];
        QString image = parts[2];
        QString title = parts[3];
        QString shelfId = parts[4];

        BookLibraryItem::TabMode mode = (cmd == "MY_BOOK_INFO_RESULT") ? BookLibraryItem::MyBooks
                                                                       : BookLibraryItem::MyShelves;

        BookLibraryItem *item = new BookLibraryItem(this);
        item->setMode(mode);
        item->setBookData(bookId, image, title, shelfId);
        item->updateShelvesMenu(userShelves);

        connect(item, &BookLibraryItem::detailsRequested, this, &LibraryWidget::handleBookDetails);
        connect(item, &BookLibraryItem::studyRequested, this, &LibraryWidget::handleBookStudy);
        connect(item,
                &BookLibraryItem::removeFromShelfRequested,
                this,
                &LibraryWidget::handleRemoveFromShelf);
        connect(item,
                &BookLibraryItem::shelfAssignmentChanged,
                this,
                &LibraryWidget::handleShelfAssignment);

        if (mode == BookLibraryItem::MyBooks)
            addBookToGrid(item, myBooksLayout, 3);
        else
            addBookToGrid(item, shelvesBooksLayout, 3);

    } else if (cmd == "SAVED_BOOK_INFO_RESULT") {
        if (parts.size() < 5)
            return;

        QString bookId = parts[1];
        QString image = parts[2];
        QString title = parts[3];
        bool isPurchased = (parts[4] == "TRUE");

        BookLibraryItem *item = new BookLibraryItem(this);
        item->setMode(BookLibraryItem::SavedBooks);
        item->setBookData(bookId, image, title, "NONE");
        item->setIsPurchased(isPurchased);

        connect(item, &BookLibraryItem::detailsRequested, this, &LibraryWidget::handleBookDetails);
        connect(item, &BookLibraryItem::studyRequested, this, &LibraryWidget::handleBookStudy);
        connect(item,
                &BookLibraryItem::removeFromSavedRequested,
                this,
                &LibraryWidget::handleRemoveFromSaved);

        addBookToGrid(item, savedBooksLayout, 2);

    } else if (cmd == "ADD_SHELF_RESULT" || cmd == "EDIT_SHELF_RESULT"
               || cmd == "DELETE_SHELF_RESULT" || cmd == "REMOVE_SAVED_RESULT"
               || cmd == "ASSIGN_SHELF_RESULT" || cmd == "REMOVE_FROM_SHELF_RESULT") {
        on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    } else if (cmd == "FILE_START") {
        if (parts.size() >= 4) {
            int totalChunks = parts[2].toInt();
            currentBookLastPage = parts[3].toInt();
            if (pdfLoadingDialog) {
                pdfLoadingDialog->setMaximum(totalChunks);
                pdfLoadingDialog->setValue(0);
            }
        }
    } else if (cmd == "FILE_CHUNK") {
        if (parts.size() >= 3) {
            QString base64Data = parts[2];
            currentPdfBuffer.append(base64Data.toUtf8());
            if (pdfLoadingDialog) {
                pdfLoadingDialog->setValue(pdfLoadingDialog->value() + 1);
            }
        }
    } else if (cmd == "FILE_END") {
        if (pdfLoadingDialog) {
            pdfLoadingDialog->setValue(pdfLoadingDialog->maximum());
            pdfLoadingDialog->close();
            delete pdfLoadingDialog;
            pdfLoadingDialog = nullptr;
        }

        QByteArray finalPdfData = QByteArray::fromBase64(currentPdfBuffer);
        pdfviewerWidget *viewer = new pdfviewerWidget();
        viewer->setAttribute(Qt::WA_DeleteOnClose);
        viewer->setBookId(currentReadingBookId);

        connect(viewer, &pdfviewerWidget::lastPageSaved, this, &LibraryWidget::handleLastPageSave);

        if (viewer->loadPdfFromData(finalPdfData)) {
            viewer->showMaximized();
            viewer->jumpToPage(currentBookLastPage);
        } else {
            delete viewer;
        }
        currentPdfBuffer.clear();
    }
}

// ==========================================
// مدیریت قفسه‌ها
// ==========================================
void LibraryWidget::updateComboBox()
{
    ui->comboBox_Ghafaseh->blockSignals(true);
    QString currentSelectedId = ui->comboBox_Ghafaseh->currentData().toString();
    ui->comboBox_Ghafaseh->clear();

    for (auto it = userShelves.constBegin(); it != userShelves.constEnd(); ++it) {
        ui->comboBox_Ghafaseh->addItem(it.value(), it.key());
    }

    int index = ui->comboBox_Ghafaseh->findData(currentSelectedId);
    if (index != -1)
        ui->comboBox_Ghafaseh->setCurrentIndex(index);
    else if (ui->comboBox_Ghafaseh->count() > 0)
        ui->comboBox_Ghafaseh->setCurrentIndex(0);

    ui->comboBox_Ghafaseh->blockSignals(false);

    if (ui->tabWidget->currentIndex() == 2 && ui->comboBox_Ghafaseh->count() > 0) {
        requestShelfBooks(ui->comboBox_Ghafaseh->currentData().toString());
    } else if (ui->tabWidget->currentIndex() == 2 && ui->comboBox_Ghafaseh->count() == 0) {
        clearLayout(shelvesBooksLayout);
    }
}

void LibraryWidget::on_comboBox_Ghafaseh_currentIndexChanged(int index)
{
    if (index >= 0)
        requestShelfBooks(ui->comboBox_Ghafaseh->itemData(index).toString());
}

void LibraryWidget::on_toolButton_addGhafaseh_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         "قفسه جدید",
                                         "نام قفسه را وارد کنید:",
                                         QLineEdit::Normal,
                                         "قفسه جدید",
                                         &ok);
    if (ok && !text.isEmpty()) {
        QString message = "ADD_SHELF||" + text;
        // client->sendMessage(message);

        // ==========================================
        // =============== شروع کدهای تست ===============
        QTimer::singleShot(100, this, [=]() { processServerResponse("ADD_SHELF_RESULT||SUCCESS"); });
        // =============== پایان کدهای تست ===============
        // ==========================================
    }
}

void LibraryWidget::on_toolButton_editnameGhafaseh_clicked()
{
    int idx = ui->comboBox_Ghafaseh->currentIndex();
    if (idx < 0)
        return;
    QString shelfId = ui->comboBox_Ghafaseh->itemData(idx).toString();
    QString currentName = ui->comboBox_Ghafaseh->itemText(idx);

    bool ok;
    QString text = QInputDialog::getText(this,
                                         "ویرایش قفسه",
                                         "نام جدید:",
                                         QLineEdit::Normal,
                                         currentName,
                                         &ok);
    if (ok && !text.isEmpty() && text != currentName) {
        QString message = "EDIT_SHELF||" + shelfId + "||" + text;
        // client->sendMessage(message);

        // ==========================================
        // =============== شروع کدهای تست ===============
        QTimer::singleShot(100, this, [=]() {
            processServerResponse("EDIT_SHELF_RESULT||SUCCESS");
        });
        // =============== پایان کدهای تست ===============
        // ==========================================
    }
}

void LibraryWidget::on_toolButton_removeGhafaseh_clicked()
{
    int idx = ui->comboBox_Ghafaseh->currentIndex();
    if (idx < 0)
        return;
    QString shelfId = ui->comboBox_Ghafaseh->itemData(idx).toString();

    auto reply = QMessageBox::question(this,
                                       "حذف قفسه",
                                       "آیا از حذف این قفسه اطمینان دارید؟",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString message = "DELETE_SHELF||" + shelfId;
        // client->sendMessage(message);

        // ==========================================
        // =============== شروع کدهای تست ===============
        QTimer::singleShot(100, this, [=]() {
            processServerResponse("DELETE_SHELF_RESULT||SUCCESS");
        });
        // =============== پایان کدهای تست ===============
        // ==========================================
    }
}

void LibraryWidget::handleBookDetails(QString bookId)
{
    Bookdetails *detailsWindow = new Bookdetails(nullptr, bookId);
    detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
    detailsWindow->show();
}

void LibraryWidget::handleBookStudy(QString bookId)
{
    currentReadingBookId = bookId;
    pdfLoadingDialog = new QProgressDialog("در حال دریافت کتاب...", "لغو", 0, 100, this);
    pdfLoadingDialog->setWindowTitle("لطفاً صبر کنید");
    pdfLoadingDialog->setWindowModality(Qt::WindowModal);
    pdfLoadingDialog->setValue(0);
    pdfLoadingDialog->show();

    currentPdfBuffer.clear();
    QString message = "DOWNLOAD_BOOK||" + bookId;
    // client->sendMessage(message);
}

void LibraryWidget::handleRemoveFromSaved(QString bookId)
{
    QString message = "REMOVE_FROM_SAVED||" + bookId;
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() { processServerResponse("REMOVE_SAVED_RESULT||SUCCESS"); });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::handleRemoveFromShelf(QString bookId)
{
    QString shelfId = ui->comboBox_Ghafaseh->currentData().toString();
    QString message = "REMOVE_FROM_SHELF||" + bookId + "||" + shelfId;
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() {
        processServerResponse("REMOVE_FROM_SHELF_RESULT||SUCCESS");
    });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::handleShelfAssignment(QString bookId, QString newShelfId)
{
    QString message = "ASSIGN_TO_SHELF||" + bookId + "||" + newShelfId;
    // client->sendMessage(message);

    // ==========================================
    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() { processServerResponse("ASSIGN_SHELF_RESULT||SUCCESS"); });
    // =============== پایان کدهای تست ===============
    // ==========================================
}

void LibraryWidget::handleLastPageSave(QString bookId, int pageNumber)
{
    QString message = "UPDATE_LAST_PAGE||" + bookId + "||" + QString::number(pageNumber);
    // client->sendMessage(message);
}