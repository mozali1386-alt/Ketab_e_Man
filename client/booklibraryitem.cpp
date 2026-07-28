#include "booklibraryitem.h"
#include <QAction>
#include <QByteArray>
#include <QPixmap>
#include "ui_booklibraryitem.h"

BookLibraryItem::BookLibraryItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookLibraryItem)
{
    ui->setupUi(this);

    shelfMenu = new QMenu(this);
    ui->toolButton_editGhafaseh->setMenu(shelfMenu);
    ui->toolButton_editGhafaseh->setPopupMode(QToolButton::InstantPopup);
}

BookLibraryItem::~BookLibraryItem()
{
    delete ui;
}

void BookLibraryItem::setMode(TabMode mode)
{
    currentMode = mode;

    ui->pushButton_details->hide();
    ui->pushButton_study->hide();
    ui->pushButton_removesavehaANDremoveasGhafaseh->hide();
    ui->toolButton_editGhafaseh->hide();
    ui->label->hide();

    if (mode == MyBooks) {
        ui->pushButton_details->show();
        ui->pushButton_study->show();
        ui->toolButton_editGhafaseh->show();

    } else if (mode == SavedBooks) {
        ui->pushButton_details->show();
        ui->pushButton_removesavehaANDremoveasGhafaseh->show();
        ui->pushButton_removesavehaANDremoveasGhafaseh->setText("حذف از ذخیره شده ها");

    } else if (mode == MyShelves) {
        ui->pushButton_study->show();
        ui->pushButton_removesavehaANDremoveasGhafaseh->show();
        ui->pushButton_removesavehaANDremoveasGhafaseh->setText("حذف از قفسه");
        ui->toolButton_editGhafaseh->show();
    }
}

void BookLibraryItem::setIsPurchased(bool purchased)
{
    if (currentMode == SavedBooks) {
        if (purchased) {
            ui->pushButton_study->show();
            ui->label->hide();
        } else {
            ui->pushButton_study->hide();
            ui->label->show();
        }
    }
}

void BookLibraryItem::setBookData(const QString &id,
                                  const QString &image,
                                  const QString &title,
                                  const QString &currentShelfId)
{
    currentBookId = id;
    currentShelf = currentShelfId;

    if (image == "NO_IMAGE" || image.isEmpty()) {
        ui->label_picture->setText("بدون تصویر");
    } else {
        QByteArray byteArray = QByteArray::fromBase64(image.toUtf8());
        QPixmap pixmap;
        if (pixmap.loadFromData(byteArray)) {
            ui->label_picture->setPixmap(pixmap);
            ui->label_picture->setScaledContents(true);
        } else {
            ui->label_picture->setText("خطا در عکس");
        }
    }

    ui->label_bookname->setText(title);
}

void BookLibraryItem::updateShelvesMenu(const QMap<QString, QString> &shelvesMap)
{
    shelfMenu->clear();

    if (shelvesMap.isEmpty()) {
        QAction *emptyAction = shelfMenu->addAction("قفسه‌ای وجود ندارد");
        emptyAction->setEnabled(false);
        return;
    }

    for (auto it = shelvesMap.constBegin(); it != shelvesMap.constEnd(); ++it) {
        QString shelfId = it.key();
        QString shelfName = it.value();

        QAction *action = shelfMenu->addAction(shelfName);
        action->setCheckable(true);

        if (shelfId == currentShelf) {
            action->setChecked(true);
            action->setEnabled(false);
        } else {
            connect(action, &QAction::triggered, this, [=]() {
                emit shelfAssignmentChanged(currentBookId, shelfId);
            });
        }
    }
}

QString BookLibraryItem::getBookId() const
{
    return currentBookId;
}

void BookLibraryItem::on_pushButton_details_clicked()
{
    emit detailsRequested(currentBookId);
}

void BookLibraryItem::on_pushButton_study_clicked()
{
    emit studyRequested(currentBookId);
}

void BookLibraryItem::on_pushButton_removesavehaANDremoveasGhafaseh_clicked()
{
    if (currentMode == SavedBooks) {
        emit removeFromSavedRequested(currentBookId);
    } else if (currentMode == MyShelves) {
        emit removeFromShelfRequested(currentBookId);
    }
}