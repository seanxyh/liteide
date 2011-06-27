#include "liteappoption.h"
#include "ui_liteappoption.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>

LiteAppOption::LiteAppOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteAppOption)
{
    ui->setupUi(m_widget);
    const QString &liteideTrPath = m_liteApp->resourcePath()+"/translations";
    ui->langComboBox->addItem(QLocale::languageToString(QLocale::English),QLocale::English);
    QDir dir(liteideTrPath);
    if (dir.exists()) {
        foreach (QFileInfo info,dir.entryInfoList(QStringList() << "liteide_*.qm")) {
            QString base = info.baseName();
            QString lc = base.right(base.length()-8);
            QLocale::Language lang = QLocale(lc).language();
            ui->langComboBox->addItem(QLocale::languageToString(lang),lang);
        }
    }
    QString locale = QLocale::system().name();
    locale = m_liteApp->settings()->value("General/Language",locale).toString();
    if (!locale.isEmpty()) {
        QLocale::Language lang = QLocale(locale).language();
        for (int i = 0; i < ui->langComboBox->count(); i++) {
            if (lang == ui->langComboBox->itemData(i).toInt()) {
                ui->langComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
}

LiteAppOption::~LiteAppOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteAppOption::widget()
{
    return m_widget;
}

QString LiteAppOption::displayName() const
{
    return "LiteApp";
}

QString LiteAppOption::mimeType() const
{
    return "option/liteapp";
}
void LiteAppOption::apply()
{
    int index = ui->langComboBox->currentIndex();
    if (index < 0) {
        return;
    }
    QLocale::Language lc = (QLocale::Language)ui->langComboBox->itemData(index).toInt();
    m_liteApp->settings()->setValue("General/Language",QLocale(lc).name());

}
