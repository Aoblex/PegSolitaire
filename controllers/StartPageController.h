#ifndef STARTPAGECONTROLLER_H
#define STARTPAGECONTROLLER_H

#include <QObject>
#include "../views/StartPageView.h"

class StartPageController : public QObject
{
    Q_OBJECT

public:
    explicit StartPageController(StartPageView *view, QObject *parent = nullptr);

signals:
    void navigateToHome();
    void startGame(const QString &boardType);

public slots:
    void onBackClicked();
    void onGameModeSelected(const QString &mode);

private:
    StartPageView *m_view;
};

#endif // STARTPAGECONTROLLER_H
