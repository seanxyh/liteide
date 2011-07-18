#ifndef GOLANGCODE_H
#define GOLANGCODE_H

#include "liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "litebuildapi/litebuildapi.h"

class QProcess;
class GolangCode : public QObject
{
    Q_OBJECT
public:
    explicit GolangCode(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangCode();
    void setCompleter(LiteApi::ICompleter *completer);
    void setBuild(LiteApi::IBuild *build);
public slots:
    void prefixChanged(QTextCursor,QString);
    void started();
    void finished(int,QProcess::ExitStatus);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ICompleter   *m_completer;
    QString     m_prefix;
    QProcess   *m_process;
    QByteArray  m_writeData;
    LiteApi::IBuild *m_build;
    QString     m_cmd;
};

#endif // GOLANGCODE_H
