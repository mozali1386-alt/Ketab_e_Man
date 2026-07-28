#include "pdfviewerwidget.h"
#include <QMessageBox>
#include <QPdfPageNavigator>
#include <QPdfView>
#include "ui_pdfviewerwidget.h"

pdfviewerWidget::pdfviewerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::pdfviewerWidget)
{
    ui->setupUi(this);

    m_document = new QPdfDocument(this);
    m_pdfBuffer = new QBuffer(this);

    // تنظیمات صفحه
    ui->widget->setPageMode(QPdfView::PageMode::MultiPage);
    ui->widget->setZoomMode(QPdfView::ZoomMode::FitToWidth);

    connect(ui->widget->pageNavigator(), &QPdfPageNavigator::currentPageChanged, this, [=](int page) {
        ui->spinBox_pagenumber->blockSignals(true);
        ui->spinBox_pagenumber->setValue(page + 1);
        ui->spinBox_pagenumber->blockSignals(false);
    });
}

pdfviewerWidget::~pdfviewerWidget()
{
    delete ui;
}

bool pdfviewerWidget::loadPdfFromData(const QByteArray &pdfData)
{
    if (m_pdfBuffer->isOpen()) {
        m_pdfBuffer->close();
    }

    m_pdfBuffer->setData(pdfData);
    m_pdfBuffer->open(QIODevice::ReadOnly);

    m_document->load(m_pdfBuffer);

    if (m_document->pageCount() == 0) {
        QMessageBox::critical(this, "خطای فایل", "فایل PDF خراب است.");
        return false;
    }

    ui->widget->setDocument(m_document);

    int totalPages = m_document->pageCount();
    ui->spinBox_pagenumber->setMinimum(1);
    ui->spinBox_pagenumber->setMaximum(totalPages);
    ui->label->setText(QString("از %1").arg(totalPages));

    return true;
}

void pdfviewerWidget::on_pushButton_exit_clicked()
{
    int currentPage = ui->widget->pageNavigator()->currentPage();
    emit lastPageSaved(m_bookId, currentPage + 1);

    this->close();
}

void pdfviewerWidget::on_toolButton_nextpage_clicked()
{
    int currentPage = ui->widget->pageNavigator()->currentPage();
    if (currentPage < m_document->pageCount() - 1) {
        ui->widget->pageNavigator()->jump(currentPage + 1,
                                          QPointF(),
                                          ui->widget->pageNavigator()->currentZoom());
    }
}

void pdfviewerWidget::on_toolButton_backpage_clicked()
{
    int currentPage = ui->widget->pageNavigator()->currentPage();
    if (currentPage > 0) {
        ui->widget->pageNavigator()->jump(currentPage - 1,
                                          QPointF(),
                                          ui->widget->pageNavigator()->currentZoom());
    }
}

void pdfviewerWidget::on_toolButton_zoomin_clicked()
{
    // ابتدا قفل "فیت بودن صفحه" را باز می‌کنیم
    ui->widget->setZoomMode(QPdfView::ZoomMode::Custom);

    // حالا زوم را 20 درصد افزایش می‌دهیم
    qreal currentZoom = ui->widget->zoomFactor();
    ui->widget->setZoomFactor(currentZoom * 1.2);
}

void pdfviewerWidget::on_toolButton_zoomout_clicked()
{
    //  ابتدا قفل "فیت بودن صفحه" را باز می‌کنیم
    ui->widget->setZoomMode(QPdfView::ZoomMode::Custom);

    //حالا زوم را 20 درصد کاهش می‌دهیم
    qreal currentZoom = ui->widget->zoomFactor();
    ui->widget->setZoomFactor(currentZoom / 1.2);
}

void pdfviewerWidget::on_spinBox_pagenumber_valueChanged(int arg1)
{
    ui->widget->pageNavigator()->jump(arg1 - 1,
                                      QPointF(),
                                      ui->widget->pageNavigator()->currentZoom());
}

void pdfviewerWidget::setBookId(const QString &bookId)
{
    m_bookId = bookId;
}

void pdfviewerWidget::jumpToPage(int pageIndex)
{
    if (m_document && pageIndex >= 0 && pageIndex < m_document->pageCount()) {
        ui->widget->pageNavigator()->jump(pageIndex - 1,
                                          QPointF(),
                                          ui->widget->pageNavigator()->currentZoom());
    }
}
