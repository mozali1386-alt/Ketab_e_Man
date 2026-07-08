#ifndef PUBLISHERPROFILEPAGE_H
#define PUBLISHERPROFILEPAGE_H

#include <QWidget>

namespace Ui {
class Publisherprofilepage;
}

class Publisherprofilepage : public QWidget
{
    Q_OBJECT

public:
    explicit Publisherprofilepage(QWidget *parent = nullptr);
    ~Publisherprofilepage();

private:
    Ui::Publisherprofilepage *ui;
};

#endif // PUBLISHERPROFILEPAGE_H
