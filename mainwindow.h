#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget> // Add this include
#include "views/HomePageView.h"
#include "controllers/HomePageController.h"
#include "views/StartPageView.h"
#include "controllers/StartPageController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showHomePage();
    void showStartPage();
    void startGame(const QString &boardType);

private:
    HomePageView *homePageView;
    HomePageController *homePageController;
    StartPageView *startPageView;
    StartPageController *startPageController;

    QStackedWidget *stackedWidget; // Replace currentView
};

#endif // MAINWINDOW_H
