#ifndef PDFVIEWERWIDGET_H
#define PDFVIEWERWIDGET_H

#include <QBuffer>
#include <QPdfDocument>
#include <QWidget>

namespace Ui {
class pdfviewerWidget;
}

class pdfviewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit pdfviewerWidget(QWidget *parent = nullptr);
    ~pdfviewerWidget();
    // تابع اصلی برای دریافت کتاب
    bool loadPdfFromData(const QByteArray &pdfData);
    void setBookId(const QString &bookId);
    void jumpToPage(int pageIndex);

signals:
    void lastPageSaved(QString bookId, int pageNumber);

private slots:
    // این همان توابعی است که کامپایلر شما گم کرده بود!
    void on_pushButton_exit_clicked();
    void on_toolButton_nextpage_clicked();
    void on_toolButton_backpage_clicked();
    void on_toolButton_zoomin_clicked();
    void on_toolButton_zoomout_clicked();
    void on_spinBox_pagenumber_valueChanged(int arg1);

private:
    Ui::pdfviewerWidget *ui;
    QPdfDocument *m_document;
    QBuffer *m_pdfBuffer;
    QString m_bookId;
};

#endif // PDFVIEWERWIDGET_H
