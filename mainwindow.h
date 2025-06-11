#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "views/HomePageView.h"
#include "controllers/HomePageController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    HomePageView *homePageView;
    HomePageController *homePageController;
};

#endif // MAINWINDOW_H
