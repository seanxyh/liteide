#include "litedoc.h"
#include "liteapi/litefindobj.h"
#include "documentbrowser/documentbrowser.h"
#include "htmlutil/htmlutil.h"

#include <QAction>
#include <QMenu>
#include <QFile>
#include <QTextCodec>
#include <QFileInfo>
#include <QDir>
#include <QPlainTextEdit>
#include <QDesktopServices>
#include <QStatusBar>
#include <QComboBox>
#include <QDebug>

LiteDoc::LiteDoc(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_docBrowser = new DocumentBrowser(app,this);
    m_docBrowser->urlComboBox()->setEditable(false);
    m_docBrowser->setName(tr("LiteIDE Document Browser"));
    m_docBrowser->setSearchPaths(QStringList() << m_liteApp->resourcePath()+"/doc");

    m_browserAct = m_liteApp->editorManager()->registerBrowser(m_docBrowser);
    QMenu *menu = m_liteApp->actionManager()->loadMenu("view");
    if (menu) {
        menu->addAction(m_browserAct);
    }

    connect(m_docBrowser,SIGNAL(requestUrl(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_docBrowser,SIGNAL(highlighted(QUrl)),this,SLOT(highlighted(QUrl)));

    QString path = m_liteApp->resourcePath()+"/doc/doc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }

    m_liteApp->extension()->addObject("LiteApi.ILiteDoc",this);

    QUrl url("/readme.html");
    openUrl(url);
}

void LiteDoc::activeBrowser()
{
    m_liteApp->editorManager()->activeBrowser(m_docBrowser);
}

QUrl LiteDoc::parserUrl(const QUrl &_url)
{
    QDir root(m_liteApp->resourcePath()+"/doc");
    QUrl url = _url;
    if (url.isRelative() && !url.path().isEmpty()) {
        QFileInfo info;
        if (url.path().at(0) == '/') {
            info.setFile(root,url.path().right(url.path().length()-1));
        } else if (m_lastUrl.scheme() == "file") {
            info.setFile(QFileInfo(m_lastUrl.toLocalFile()).absoluteDir(),url.path());
        }
        if (info.exists() && info.isFile()) {
            url.setScheme("file");
            url.setPath(info.filePath());
        }
    }
    return url;
}

void LiteDoc::openUrl(const QUrl &_url)
{
    QUrl url = parserUrl(_url);
    if (url.scheme() == "file") {
        openUrlFile(url);
    } else if (url.scheme() == "http" ||
               url.scheme() == "mailto") {
        QDesktopServices::openUrl(url);
    }
}

void LiteDoc::highlighted(const QUrl &url)
{
    m_docBrowser->statusBar()->showMessage(url.toString());
}

void LiteDoc::openUrlFile(const QUrl &url)
{
    QFileInfo info(url.toLocalFile());
    if (!info.exists()) {
        info.setFile(url.path());
    }
    QString ext = info.suffix().toLower();
    if (ext == "html") {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            file.close();
            if (info.fileName().compare("docs.html",Qt::CaseInsensitive) == 0) {
                updateHtmlDoc(url,ba,QString(),false);
            } else {
                updateHtmlDoc(url,ba);
            }
        }
    } else if (ext == "go") {
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(info.filePath());
        if (editor) {
            editor->setReadOnly(true);
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
            if (ed && url.hasQueryItem("s")) {
                QStringList pos = url.queryItemValue("s").split(":");
                if (pos.length() == 2) {
                    bool ok = false;
                    int begin = pos.at(0).toInt(&ok);
                    if (ok) {
                        QTextCursor cur = ed->textCursor();
                        cur.setPosition(begin);
                        ed->setTextCursor(cur);
                        ed->centerCursor();
                    }
                }
            }
        }
    } else if (ext == "pdf") {
        QDesktopServices::openUrl(info.filePath());
    } else {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            updateTextDoc(url,ba,info.fileName());
        }
    }
}

void LiteDoc::updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header)
{
    m_lastUrl = url;
    QTextCodec *codec = QTextCodec::codecForUtfText(ba,QTextCodec::codecForName("utf-8"));
    QString html = Qt::escape(codec->toUnicode(ba));
    QString data = m_templateData;
    data.replace("{header}",header);
    data.replace("{nav}","");
    data.replace("{content}",QString("<pre>%1</pre>").arg(html));
    m_docBrowser->setUrlHtml(url,data);

}

void LiteDoc::updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header, bool toNav)
{
    m_lastUrl = url;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString genHeader;
    QString nav;
    QString content = HtmlUtil::docToNavdoc(codec->toUnicode(ba),genHeader,nav);
    QString data = m_templateData;

    if (!header.isEmpty()) {
        data.replace("{header}",header);
    } else {
        data.replace("{header}",genHeader);
    }
    if (toNav) {
        data.replace("{nav}",nav);
    } else {
        data.replace("{nav}","");
    }
    data.replace("{content}",content);
    m_docBrowser->setUrlHtml(url,data);
}
