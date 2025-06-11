#include "controllers/StartPageController.h"
#include <QDebug>

StartPageController::StartPageController(StartPageView *view, QObject *parent)
    : QObject(parent), m_view(view)
{
    connect(m_view, &StartPageView::backClicked, this, &StartPageController::onBackClicked);
    connect(m_view, &StartPageView::gameModeSelected, this, &StartPageController::onGameModeSelected);
}

void StartPageController::onBackClicked()
{
    qDebug() << "StartPage: Back button clicked";
    emit navigateToHome();
}

void StartPageController::onGameModeSelected(BoardType boardType)
{
    // qDebug() << "StartPage: Game mode selected -" << boardType;
    emit startGame(boardType);
    // Logic to switch to the game view with the selected mode
}