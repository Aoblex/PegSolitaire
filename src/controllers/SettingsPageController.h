#ifndef SETTINGSPAGECONTROLLER_H
#define SETTINGSPAGECONTROLLER_H

#include <QObject>
#include "views/SettingsPageView.h"

class SettingsPageController : public QObject
{
    Q_OBJECT

public:
    explicit SettingsPageController(SettingsPageView *view, QObject *parent = nullptr);

signals:
    void navigateToHome();
    void setFullscreen(bool fullscreen);

public slots:
    void onBackClicked();
    void onFullscreenToggled(bool checked);

private:
    SettingsPageView *m_view;
};

#endif // SETTINGSPAGECONTROLLER_H
