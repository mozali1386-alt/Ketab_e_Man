//in the name of ALLAH
//YA MAHDI

#ifndef PIPEESCAPE_H
#define PIPEESCAPE_H

#include <QString>

class PipeEscape {
public:
    static QString escape(const QString &text);

    static QString unescape(const QString &text);
};

#endif
