#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    homePageView = new HomePageView(this);
    homePageController = new HomePageController(homePageView, this);

    setCentralWidget(homePageView);
    setWindowTitle("Peg Solitaire");
}

MainWindow::~MainWindow()
{
}
