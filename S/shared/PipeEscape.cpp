//in the name of ALLAH
//YA MAHDI

#include "PipeEscape.h"

QString PipeEscape::escape(const QString &text) {
    QString result = text;
    result.replace("&pipe;", "&amp;pipe;");
    result.replace("|", "&pipe;");
    return result;
}

QString PipeEscape::unescape(const QString &text) {
    QString result = text;
    result.replace("&pipe;", "|");
    result.replace("&amp;pipe;", "&pipe;");
    return result;
}
