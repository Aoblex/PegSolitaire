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
#include "views/BoardView.h" // Added
#include "models/Board.h"    // Added
// Forward declare BoardController if its header is not fully needed here
// class BoardController;

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
    void startGame(const QString &boardType);
    void toggleFullscreen(bool fullscreen);

private:
    HomePageView *homePageView;
    HomePageController *homePageController;
    StartPageView *startPageView;
    StartPageController *startPageController;
    SettingsPageView *settingsPageView;
    SettingsPageController *settingsPageController;
    BoardView *boardView; // Added
    // BoardController *boardController; // Added - uncomment if/when BoardController is created
    Board *currentBoardModel; // Added

    QStackedWidget *stackedWidget; // Replace currentView
};

#endif // MAINWINDOW_H
