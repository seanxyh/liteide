#ifndef LITEDOC_H
#define LITEDOC_H

#include "liteapi/liteapi.h"
#include <QUrl>

class DocumentBrowser;
class LiteDoc : public QObject
{
    Q_OBJECT
public:
    explicit LiteDoc(LiteApi::IApplication *app, QObject *parent = 0);

public slots:
    void openUrl(const QUrl &url);
    void highlighted(const QUrl &url);
protected:
    void openUrlFile(const QUrl &url);
    void updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header);
    void updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header = QString(), bool toNav = true);
    QUrl parserUrl(const QUrl &url);
    LiteApi::IApplication *m_liteApp;
    DocumentBrowser *m_docBrowser;
    QAction *m_browserAct;
    QString m_templateData;
    QUrl m_lastUrl;
};

#endif // LITEDOC_H
