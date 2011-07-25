#include "envmanager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>

Env::Env(QObject *parent) :
    LiteApi::IEnv(parent)
{
    m_env = QProcessEnvironment::systemEnvironment();
}

QString Env::id() const
{
    return m_id;
}

QProcessEnvironment Env::currentEnv() const
{
    return m_env;
}

void Env::loadEnv(LiteApi::IEnvManager *manager, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    Env *env = new Env(manager);
    env->m_id = QFileInfo(filePath).baseName();

#ifdef Q_OS_WIN
    QRegExp rx("\\%([\\w]+)\\%");
#else
    QRegExp rx("\\$([\\w]+)");
#endif
    while (!f.atEnd()) {
        QString line = f.readLine().trimmed();
        int pos = line.indexOf("=");
        if (pos == -1) {
            continue;
        }
        QString key = line.left(pos).trimmed();
        QString value = line.right(line.length()-pos-1).trimmed();
        pos = 0;
        int rep = 0;
        while (rep++ < 10) {
            pos = rx.indexIn(value,pos);
            if (pos == -1) {
                break;
            }
            QString v = env->m_env.value(rx.cap(1));
            if (!v.isEmpty()) {
                value.replace(pos,rx.cap(0).length(),v);
                pos = 0;
            } else {
                pos += rx.matchedLength();
            }
        }
        env->m_env.insert(key,value);
    }
    manager->addEnv(env);
}

EnvManager::EnvManager(QObject *parent)
    : LiteApi::IEnvManager(parent),
      m_curEnv(0)
{
}

EnvManager::~EnvManager()
{
    if (m_curEnv) {
        m_liteApp->settings()->setValue("LiteEnv/current",m_curEnv->id());
    }
}

void EnvManager::addEnv(LiteApi::IEnv *env)
{
    m_envList.append(env);
}

void EnvManager::removeEnv(LiteApi::IEnv *env)
{
    m_envList.removeAll(env);
}

LiteApi::IEnv *EnvManager::findEnv(const QString &id) const
{
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->id() == id) {
            return env;
        }
    }
    return NULL;
}

QList<LiteApi::IEnv*> EnvManager::envList() const
{
    return m_envList;
}

void EnvManager::setCurrentEnv(LiteApi::IEnv *env)
{
    if (m_curEnv == env) {
        return;
    }
    m_curEnv = env;
    emit currentEnvChanged(m_curEnv);

    if (m_curEnv == 0) {
        return;
    }
    if (m_curEnv->id() == m_envCmb->currentText()) {
        return;
    }

    for (int i = 0; i < m_envCmb->count(); i++) {
        if (m_envCmb->itemText(i) == env->id()) {
            m_envCmb->setCurrentIndex(i);
            break;
        }
    }
}

LiteApi::IEnv *EnvManager::currentEnv() const
{
    return m_curEnv;
}

void EnvManager::loadEnvFiles(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendConsole("LiteEnv","LoadEnvFiles",path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.env"));
    foreach (QString fileName, dir.entryList()) {
        Env::loadEnv(this,QFileInfo(dir,fileName).absoluteFilePath());
    }
}

bool EnvManager::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IEnvManager::initWithApp(app)) {
        return false;
    }
    loadEnvFiles(m_liteApp->resourcePath()+"/environment");

    m_toolBar = new QToolBar(tr("LiteEnv"));
    m_toolBar->setObjectName("LiteEnv");

    m_envCmb = new QComboBox;
    m_envCmb->setToolTip(tr("Environment"));

    m_toolBar->addWidget(new QLabel(tr("Env:")));
    m_toolBar->addWidget(m_envCmb);

    foreach (LiteApi::IEnv *env, m_envList) {
        m_envCmb->addItem(env->id());
    }

    QString id = m_liteApp->settings()->value("LiteEnv/current").toString();
    envActivated(id);

    m_liteApp->actionManager()->addToolBar(m_toolBar);
    connect(m_envCmb,SIGNAL(activated(QString)),this,SLOT(envActivated(QString)));


    m_liteApp->extension()->addObject("LiteApi.IEnvManager",this);

    return true;
}

void EnvManager::envActivated(QString id)
{
    LiteApi::IEnv *env = findEnv(id);
    setCurrentEnv(env);
}
