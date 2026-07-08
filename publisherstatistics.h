#ifndef PUBLISHERSTATISTICS_H
#define PUBLISHERSTATISTICS_H

#include <QWidget>

namespace Ui {
class PublisherStatistics;
}

class PublisherStatistics : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherStatistics(QWidget *parent = nullptr);
    ~PublisherStatistics();

private:
    Ui::PublisherStatistics *ui;
};

#endif // PUBLISHERSTATISTICS_H
