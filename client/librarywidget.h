#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QGridLayout>
#include <QMap>
#include <QWidget>
#include "booklibraryitem.h"
#include "clientsocketmanager.h"
#include <qprogressdialog.h>

namespace Ui {
class LibraryWidget;
}

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget(QWidget *parent = nullptr);
    ~LibraryWidget();
    void refreshCurrentTab();
    void setClient(ClientSocketManager *client);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_comboBox_Ghafaseh_currentIndexChanged(int index);
    void on_toolButton_addGhafaseh_clicked();
    void on_toolButton_editnameGhafaseh_clicked();
    void on_toolButton_removeGhafaseh_clicked();
    void handleBookDetails(QString bookId);
    void handleBookStudy(QString bookId);
    void handleRemoveFromSaved(QString bookId);
    void handleRemoveFromShelf(QString bookId);
    void handleShelfAssignment(QString bookId, QString newShelfId);
    void handleLastPageSave(QString bookId, int pageNumber);
    void processServerResponse(const QString &response);

private:
    Ui::LibraryWidget *ui;

    QGridLayout *myBooksLayout;
    QGridLayout *savedBooksLayout;
    QGridLayout *shelvesBooksLayout;

    QMap<QString, QString> userShelves;

    void setupLayouts();
    void clearLayout(QLayout *layout);
    void addBookToGrid(BookLibraryItem *item, QGridLayout *layout, int maxColumns);

    void requestMyBooks();
    void requestSavedBooks();
    void requestShelves();
    void requestShelfBooks(const QString &shelfId);
    void requestLibraryBookInfo(const QString &bookId, BookLibraryItem::TabMode mode);

    void updateComboBox();

    QByteArray currentPdfBuffer;
    QProgressDialog *pdfLoadingDialog = nullptr;
    QString currentReadingBookId;
    int currentBookLastPage = 0;
    ClientSocketManager *m_client = nullptr;
};

#endif