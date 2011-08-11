#ifndef HTMLUTIL_H
#define HTMLUTIL_H

#include <QString>

class HtmlUtil
{
public:
    HtmlUtil();
    static QString docToNavdoc(const QString &data, QString &header, QString &nav);
};

#endif // HTMLUTIL_H
