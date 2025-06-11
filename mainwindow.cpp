#include "mainwindow.h"
#include <QStackedWidget> // Ensure this is included

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      homePageView(new HomePageView(this)),
      homePageController(new HomePageController(homePageView, this)),
      startPageView(new StartPageView(this)),
      startPageController(new StartPageController(startPageView, this))
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(homePageView);
    stackedWidget->addWidget(startPageView);

    setCentralWidget(stackedWidget);

    // Connect signals for navigation
    connect(homePageController, &HomePageController::startClicked, this, &MainWindow::showStartPage);
    connect(startPageController, &StartPageController::navigateToHome, this, &MainWindow::showHomePage);
    connect(startPageController, &StartPageController::startGame, this, &MainWindow::startGame);

    showHomePage(); // Show home page initially
    setWindowTitle("Peg Solitaire");
    resize(400, 300); // Set a default size
}

MainWindow::~MainWindow()
{
    // QStackedWidget will take care of deleting its widgets if they are its children.
    // HomePageView and StartPageView are children of MainWindow, and added to stackedWidget.
    // Controllers are QObjects with MainWindow as parent, so they should be auto-deleted.
}

void MainWindow::showHomePage()
{
    stackedWidget->setCurrentWidget(homePageView);
}

void MainWindow::showStartPage()
{
    stackedWidget->setCurrentWidget(startPageView);
}

void MainWindow::startGame(const QString &boardType)
{
    // Placeholder for starting the actual game
    qDebug() << "Main window: Start game with board type:" << boardType;
    // Here you would typically create and show the GameView
}
