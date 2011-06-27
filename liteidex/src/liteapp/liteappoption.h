#ifndef LITEAPPOPTION_H
#define LITEAPPOPTION_H

#include "liteapi.h"

namespace Ui {
    class LiteAppOption;
}

class LiteAppOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit LiteAppOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~LiteAppOption();
    virtual QWidget *widget();
    virtual QString displayName() const;
    virtual QString mimeType() const;
    virtual void apply();
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::LiteAppOption *ui;
};

#endif // LITEAPPOPTION_H
