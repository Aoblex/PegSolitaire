#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget> // Add this include
#include "views/HomePageView.h"
#include "controllers/HomePageController.h"
#include "views/StartPageView.h"
#include "controllers/StartPageController.h"
#include "views/SettingsPageView.h"             // Add this
#include "controllers/SettingsPageController.h" // Add this

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showHomePage();
    void showStartPage();
    void showSettingsPage(); // Add this
    void startGame(const QString &boardType);
    void toggleFullscreen(bool fullscreen); // Add this

private:
    HomePageView *homePageView;
    HomePageController *homePageController;
    StartPageView *startPageView;
    StartPageController *startPageController;
    SettingsPageView *settingsPageView;             // Add this
    SettingsPageController *settingsPageController; // Add this

    QStackedWidget *stackedWidget; // Replace currentView
};

#endif // MAINWINDOW_H
