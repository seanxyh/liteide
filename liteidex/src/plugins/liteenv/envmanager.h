#ifndef ENVMANAGER_H
#define ENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class QComboBox;

class Env : public LiteApi::IEnv
{
    Q_OBJECT
public:
    Env(QObject *parent = 0);
public:
    virtual QString id() const;
    virtual QProcessEnvironment env() const;
    static void loadEnv(LiteApi::IEnvManager *manager, const QString &filePath);
protected:
    QString m_id;
    QProcessEnvironment m_env;
};

class EnvManager : public LiteApi::IEnvManager
{
    Q_OBJECT
public:
    EnvManager(QObject *parent = 0);   
    ~EnvManager();
public:
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual void addEnv(LiteApi::IEnv *build);
    virtual void removeEnv(LiteApi::IEnv *build);
    virtual LiteApi::IEnv *findEnv(const QString &id) const;
    virtual QList<LiteApi::IEnv*> envList() const;
    virtual void setCurrentEnv(LiteApi::IEnv *env);
    virtual LiteApi::IEnv *currentEnv() const;
    virtual QProcessEnvironment currentEnvironment() const;
protected slots:
    void envActivated(QString);
public:
    void loadEnvFiles(const QString &path);
protected:
    QList<LiteApi::IEnv*>    m_envList;
    LiteApi::IEnv           *m_curEnv;
    QToolBar        *m_toolBar;
    QComboBox       *m_envCmb;
};

#endif // ENVMANAGER_H
