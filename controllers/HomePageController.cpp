#include "HomePageController.h"
#include <QApplication>
#include <QDebug>

HomePageController::HomePageController(HomePageView *view, QObject *parent)
    : QObject(parent), m_view(view)
{
    connect(m_view, &HomePageView::startClicked, this, [this]()
            { emit startClicked(); }); // Emit signal
    connect(m_view, &HomePageView::settingsClicked, this, &HomePageController::onSettingsClicked);
    connect(m_view, &HomePageView::quitClicked, this, &HomePageController::onQuitClicked);
}

// Removed onStartClicked() as it's now a signal emission

void HomePageController::onSettingsClicked()
{
    qDebug() << "Settings button clicked";
    // Logic to switch to the settings view
}

void HomePageController::onQuitClicked()
{
    qDebug() << "Quit button clicked";
    QApplication::quit();
}
