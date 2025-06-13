#ifndef STARTPAGECONTROLLER_H
#define STARTPAGECONTROLLER_H

#include <QObject>
#include "views/StartPageView.h"
#include "models/Board.h" // Added for BoardType

class StartPageController : public QObject
{
    Q_OBJECT

public:
    explicit StartPageController(StartPageView *view, QObject *parent = nullptr);

signals:
    void navigateToHome();
    void startGame(BoardType boardType); // Changed from const QString&

public slots:
    void onBackClicked();
    void onGameModeSelected(BoardType boardType);

private:
    StartPageView *m_view;
};

#endif // STARTPAGECONTROLLER_H
