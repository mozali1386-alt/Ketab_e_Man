#ifndef PDFVIEWERWIDGET_H
#define PDFVIEWERWIDGET_H

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

private:
    Ui::pdfviewerWidget *ui;
};

#endif // PDFVIEWERWIDGET_H
