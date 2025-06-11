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
    connect(settingsPageController, &SettingsPageController::navigateToHome, this, &MainWindow::showHomePage);
    connect(settingsPageController, &SettingsPageController::setFullscreen, this, &MainWindow::toggleFullscreen);

    // Direct connection now possible as StartPageController::startGame emits BoardType
    connect(startPageController, &StartPageController::startGame, this, &MainWindow::startGame);

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

void MainWindow::startGame(BoardType boardType) // Changed QString to BoardType
{
    qDebug() << "Main window: Attempting to start game with board type (enum):" << static_cast<int>(boardType);

    // Clean up previous board model if it exists
    if (currentBoardModel)
    {
        delete currentBoardModel;
        currentBoardModel = nullptr;
    }
    // If you had a boardController, you would delete it here too.
    // if (boardController) { delete boardController; boardController = nullptr; }

    // Create the appropriate board model based on boardType
    switch (boardType)
    {
        case BoardType::ClassicEnglish:
            currentBoardModel = new GridBoard(BoardType::ClassicEnglish, this);
            break;
        case BoardType::ClassicEuropean:
            currentBoardModel = new GridBoard(BoardType::ClassicEuropean, this);
            break;
        case BoardType::ClassicCross:
            currentBoardModel = new GridBoard(BoardType::ClassicCross, this);
            break;
        case BoardType::ClassicStar: // This could be GridBoard or OffsetGridBoard
            // For now, let's assume OffsetGridBoard is preferred for Star, or make a specific StarShape for it
            currentBoardModel = new OffsetGridBoard(BoardType::ClassicStar, this); // Or BoardType::StarShape
            break;
        case BoardType::Triangular:
            currentBoardModel = new OffsetGridBoard(BoardType::Triangular, this);
            break;
        // Add other board types here
        default:
            qDebug() << "Main window: Unknown or unsupported board type (enum):" << static_cast<int>(boardType);
            currentBoardModel = new GridBoard(BoardType::ClassicEnglish, this); // Fallback
            break;
    }

    if (currentBoardModel)
    {
        qDebug() << "Main window: Board model created with type (enum):" << static_cast<int>(currentBoardModel->getBoardType())
                 << "Pegs:" << currentBoardModel->getPegCount();
        boardView->setBoard(currentBoardModel);
        // boardView->updateView(); // updateView is called by setBoard if board is valid
        stackedWidget->setCurrentWidget(boardView); // Switch to the board view
    }
    else
    {
        qDebug() << "Main window: Failed to create board model for type:" << static_cast<int>(boardType);
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
