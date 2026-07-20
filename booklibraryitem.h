#ifndef BOOKLIBRARYITEM_H
#define BOOKLIBRARYITEM_H

#include <QMap>
#include <QMenu>
#include <QWidget>

namespace Ui {
class BookLibraryItem;
}

class BookLibraryItem : public QWidget
{
    Q_OBJECT

public:
    // تعریف سه حالت برای سه تب مختلف
    enum TabMode { MyBooks, SavedBooks, MyShelves };

    explicit BookLibraryItem(QWidget *parent = nullptr);
    ~BookLibraryItem();

    void setMode(TabMode mode);

    // دریافت اطلاعات کتاب و وضعیت قفسه فعلی آن (در صورت وجود)
    void setBookData(const QString &id,
                     const QString &image,
                     const QString &title,
                     const QString &currentShelfId = "NONE");

    // آپدیت کردن منوی قفسه‌ها (سه‌نقطه)
    void updateShelvesMenu(const QMap<QString, QString> &shelvesMap);

    QString getBookId() const;

signals:
    void detailsRequested(QString bookId);
    void studyRequested(QString bookId);
    void removeFromSavedRequested(QString bookId);
    void removeFromShelfRequested(QString bookId);
    void shelfAssignmentChanged(QString bookId, QString newShelfId); // برای تغییر قفسه

private slots:
    void on_pushButton_details_clicked();
    void on_pushButton_study_clicked();
    void on_pushButton_removesavehaANDremoveasGhafaseh_clicked();

private:
    Ui::BookLibraryItem *ui;

    QString currentBookId;
    QString currentShelf;
    TabMode currentMode;
    QMenu *shelfMenu;
};

#endif // BOOKLIBRARYITEM_H