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
    explicit BookLibraryItem(QWidget *parent = nullptr);
    ~BookLibraryItem();

    enum TabMode { MyBooks = 0, SavedBooks = 1, MyShelves = 2 };

    void setMode(TabMode mode);
    void setBookData(const QString &id,
                     const QString &image,
                     const QString &title,
                     const QString &currentShelfId);
    void updateShelvesMenu(const QMap<QString, QString> &shelvesMap);
    void setIsPurchased(bool purchased);

    QString getBookId() const;

signals:
    void detailsRequested(QString bookId);
    void studyRequested(QString bookId);
    void removeFromSavedRequested(QString bookId);
    void removeFromShelfRequested(QString bookId);
    void shelfAssignmentChanged(QString bookId, QString newShelfId);

private slots:
    void on_pushButton_details_clicked();
    void on_pushButton_study_clicked();
    void on_pushButton_removesavehaANDremoveasGhafaseh_clicked();

private:
    Ui::BookLibraryItem *ui;
    TabMode currentMode;
    QString currentBookId;
    QString currentShelf;
    QMenu *shelfMenu;
};

#endif