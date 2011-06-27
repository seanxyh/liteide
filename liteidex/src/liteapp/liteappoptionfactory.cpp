#include "liteappoption.h"
#include "liteappoptionfactory.h"

LiteAppOptionFactory::LiteAppOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList LiteAppOptionFactory::mimeTypes() const
{
    return QStringList() << "option/liteapp";
}

LiteApi::IOption *LiteAppOptionFactory::create(const QString &mimeType)
{
    if (mimeType == "option/liteapp") {
        return new LiteAppOption(m_liteApp,this);
    }
    return 0;
}
