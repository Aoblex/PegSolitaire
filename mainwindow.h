#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget> // Add this include
#include "views/HomePageView.h"
#include "controllers/HomePageController.h"
#include "views/StartPageView.h"
#include "controllers/StartPageController.h"
#include "views/SettingsPageView.h"
#include "controllers/SettingsPageController.h"
#include "views/GameView.h" // Added
#include "controllers/BoardController.h" // Added  
#include "models/Board.h"    // Added

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showHomePage();
    void showStartPage();
    void showSettingsPage();
    void startGame(BoardType boardType); // Changed QString to BoardType
    void toggleFullscreen(bool fullscreen);

private:
    HomePageView *homePageView;
    HomePageController *homePageController;
    StartPageView *startPageView;
    StartPageController *startPageController;    SettingsPageView *settingsPageView;
    SettingsPageController *settingsPageController;
    GameView *gameView; // Changed from BoardView
    BoardController *boardController; // Added
    Board *currentBoardModel; // Added

    QStackedWidget *stackedWidget; // Replace currentView
};

#endif // MAINWINDOW_H
