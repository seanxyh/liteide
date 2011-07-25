#include "envmanager.h"

Env::Env(QObject *parent) :
    LiteApi::IEnv(parent)
{
}

QString Env::id() const
{
    return m_id;
}

QProcessEnvironment Env::currentEnv() const
{
    return m_env;
}

EnvManager::EnvManager(QObject *parent)
    : LiteApi::IEnvManager(parent),
      m_curEnv(0)
{
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
}

LiteApi::IEnv *EnvManager::currentEnv() const
{
    return m_curEnv;
}
