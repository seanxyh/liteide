#include "golangcode.h"
#include "fileutil/fileutil.h"
#include <QProcess>
#include <QTextDocument>
#include <QAbstractItemView>
#include <QDebug>

GolangCode::GolangCode(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_completer(0),
    m_build(0)
{
    m_process = new QProcess(this);
    connect(m_process,SIGNAL(started()),this,SLOT(started()));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
}

void GolangCode::setBuild(LiteApi::IBuild *build)
{
    m_build = build;
}

void GolangCode::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0);
    }
    m_completer = completer;
    if (m_completer) {
        connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString)),this,SLOT(prefixChanged(QTextCursor,QString)));
    }
}

void GolangCode::prefixChanged(QTextCursor cur,QString pre)
{
    QString cmd;
    if (m_build) {
        cmd =  FileUtil::lookPath("gocode",m_build->currentEnv(),true);
    } else {
        cmd = FileUtil::lookPath("gocode",QProcessEnvironment::systemEnvironment(),true);
    }
    if (cmd.isEmpty()) {
        return;
    }

    if (pre.right(1) != ".") {
        return;
    }
    QString src = cur.document()->toPlainText();
    src = src.replace("\r\n","\n");
    m_writeData = src.left(cur.position()).toUtf8();
    QStringList args;
    args << "-in" << "" << "-f" << "csv" << "autocomplete" << QString::number(m_writeData.length());
    m_writeData = src.toUtf8();
    m_prefix = pre;

    if (m_build){
        m_process->setProcessEnvironment(m_build->currentEnv());
    } else {
        m_process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    }
    m_process->start(cmd,args);
}

void GolangCode::started()
{
    m_process->write(m_writeData);
    m_process->closeWriteChannel();
}

void GolangCode::finished(int,QProcess::ExitStatus)
{
    QString read = m_process->readAllStandardOutput();
    QStringList all = read.split('\n');
    //func,,Fprint,,func(w io.Writer, a ...interface{}) (n int, error os.Error)
    //type,,Formatter,,interface
    int n = 0;
    foreach (QString s, all) {
        QStringList word = s.split(",,");
        QString item,param;
        //classes, names, types
        if (word.count() == 3) {
            if (word.at(0) == "type" || word.at(0) == "func") {
                item = word.at(1);
            }
            if (word.at(0) == "func") {
                int pos = word.at(2).indexOf("(");
                if (pos != -1) {
                    param = word.at(2).right(word.at(2).length()-pos);
                }
            }
        }
        if (!item.isEmpty()) {
            if (m_completer->appendItem(m_prefix+item+param,true)) {
                n++;
            }
        }
    }
    if (n >= 1) {
        m_completer->show();
    }
}
