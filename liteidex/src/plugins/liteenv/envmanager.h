#ifndef ENVMANAGER_H
#define ENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class Env : public LiteApi::IEnv
{
    Q_OBJECT
public:
    Env(QObject *parent = 0);
public:
    virtual QString id() const;
    virtual QProcessEnvironment currentEnv() const;
protected:
    QString m_id;
    QProcessEnvironment m_env;
};

class EnvManager : public LiteApi::IEnvManager
{
    Q_OBJECT
public:
    EnvManager(QObject *parent = 0);
public:
    virtual void addEnv(LiteApi::IEnv *build);
    virtual void removeEnv(LiteApi::IEnv *build);
    virtual LiteApi::IEnv *findEnv(const QString &id) const;
    virtual QList<LiteApi::IEnv*> envList() const;
    virtual void setCurrentEnv(LiteApi::IEnv *env);
    virtual LiteApi::IEnv *currentEnv() const;
protected:
    QList<LiteApi::IEnv*>    m_envList;
    LiteApi::IEnv           *m_curEnv;
};

#endif // ENVMANAGER_H
