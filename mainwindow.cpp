#include "mainwindow.h"
#include <QStackedWidget>
#include <QDebug>                   // For logging
#include "models/GridBoard.h"       // For creating specific board types
#include "models/OffsetGridBoard.h" // For creating specific board types
#include "views/BoardView.h"        // Ensure BoardView is included

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      homePageView(new HomePageView(this)),
      homePageController(new HomePageController(homePageView, this)),
      startPageView(new StartPageView(this)),
      startPageController(new StartPageController(startPageView, this)),
      settingsPageView(new SettingsPageView(this)),
      settingsPageController(new SettingsPageController(settingsPageView, this)),
      boardView(new BoardView(this)), // Initialize BoardView
      // boardController(nullptr),    // Initialize BoardController later if needed
      currentBoardModel(nullptr) // Initialize currentBoardModel
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(homePageView);
    stackedWidget->addWidget(startPageView);
    stackedWidget->addWidget(settingsPageView);
    stackedWidget->addWidget(boardView); // Add boardView to stackedWidget

    setCentralWidget(stackedWidget);

    // Connect signals for navigation
    connect(homePageController, &HomePageController::startClicked, this, &MainWindow::showStartPage);
    connect(homePageController, &HomePageController::settingsClicked, this, &MainWindow::showSettingsPage);
    connect(startPageController, &StartPageController::navigateToHome, this, &MainWindow::showHomePage);
    connect(startPageController, &StartPageController::startGame, this, &MainWindow::startGame);
    connect(settingsPageController, &SettingsPageController::navigateToHome, this, &MainWindow::showHomePage);
    connect(settingsPageController, &SettingsPageController::setFullscreen, this, &MainWindow::toggleFullscreen);

    showHomePage(); // Show home page initially
    setWindowTitle("Peg Solitaire");
    resize(800, 600); // Adjusted default size for better board visibility
}

MainWindow::~MainWindow()
{
    delete currentBoardModel; // Clean up the board model
    // Other owned objects (views, controllers) are children of MainWindow or QStackedWidget
    // and should be deleted automatically by Qt's parent-child mechanism.
}

void MainWindow::showHomePage()
{
    stackedWidget->setCurrentWidget(homePageView);
}

void MainWindow::showStartPage()
{
    stackedWidget->setCurrentWidget(startPageView);
}

void MainWindow::showSettingsPage() // Add this function
{
    settingsPageView->setFullscreenChecked(windowState().testFlag(Qt::WindowFullScreen));
    stackedWidget->setCurrentWidget(settingsPageView);
}

void MainWindow::startGame(const QString &boardType)
{
    qDebug() << "Main window: Attempting to start game with board type:" << boardType;

    // Clean up previous board model if it exists
    if (currentBoardModel)
    {
        delete currentBoardModel;
        currentBoardModel = nullptr;
    }
    // If you had a boardController, you would delete it here too.
    // if (boardController) { delete boardController; boardController = nullptr; }

    // Create the appropriate board model based on boardType
    if (boardType == "classic_english" || boardType == "english_standard")
    {
        currentBoardModel = new GridBoard("english_standard");
    }
    else if (boardType == "classic_european" || boardType == "european_standard")
    {
        currentBoardModel = new GridBoard("european_standard");
    }
    else if (boardType == "classic_cross")
    {
        currentBoardModel = new GridBoard("classic_cross");
    }
    else if (boardType == "classic_star") // Assuming classic_star is an OffsetGridBoard type
    {
        currentBoardModel = new OffsetGridBoard("star"); // Use "star" or "classic_star" as defined in OffsetGridBoard
    }
    else if (boardType == "triangular")
    {
        currentBoardModel = new OffsetGridBoard("triangular");
    }
    // Add other board types here (e.g., special modes might need different models or configurations)
    else
    {
        qDebug() << "Main window: Unknown or unsupported board type:" << boardType;
        // Default to English Standard or show an error message
        currentBoardModel = new GridBoard("english_standard"); // Fallback
    }

    if (currentBoardModel)
    {
        qDebug() << "Main window: Board model created:" << currentBoardModel->getBoardType()
                 << "Pegs:" << currentBoardModel->getPegCount();
        boardView->setBoard(currentBoardModel);
        // boardView->updateView(); // updateView is called by setBoard if board is valid
        stackedWidget->setCurrentWidget(boardView); // Switch to the board view
    }
    else
    {
        qDebug() << "Main window: Failed to create board model for type:" << boardType;
        showHomePage(); // Fallback to home page if board creation fails
    }
}

void MainWindow::toggleFullscreen(bool fullscreen) // Add this function
{
    if (fullscreen)
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }
}
