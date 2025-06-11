#include "SettingsPageController.h"
#include <QDebug>

SettingsPageController::SettingsPageController(SettingsPageView *view, QObject *parent)
    : QObject(parent), m_view(view)
{
    connect(m_view, &SettingsPageView::backClicked, this, &SettingsPageController::onBackClicked);
    connect(m_view, &SettingsPageView::fullscreenToggled, this, &SettingsPageController::onFullscreenToggled);
}

void SettingsPageController::onBackClicked()
{
    qDebug() << "SettingsPage: Back button clicked";
    emit navigateToHome();
}

void SettingsPageController::onFullscreenToggled(bool checked)
{
    qDebug() << "SettingsPage: Fullscreen toggled -" << checked;
    emit setFullscreen(checked);
}
