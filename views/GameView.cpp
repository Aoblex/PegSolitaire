#include "GameView.h"
#include "utils/ButtonStyles.h"
#include <QDebug>
#include <QFont>
#include <QDialog>
#include <QMessageBox>
#include <QResizeEvent>

GameView::GameView(QWidget *parent)
    : QWidget(parent),
      mainLayout(nullptr),
      gameLayout(nullptr),
      rightSideLayout(nullptr),
      controlButtonsLayout(nullptr),
      boardView(nullptr),
      scoringBoardTitle(nullptr),
      pegCountLabel(nullptr),
      undoButton(nullptr),
      resetButton(nullptr),
      homeButton(nullptr),
      guideButton(nullptr),
      guideOverlay(nullptr),
      boardController(nullptr)
{
    setupUI();
    
    // Create board controller
    boardController = new BoardController(this);
    
    // Connect board view to controller
    boardController->setBoardView(boardView);
    
    // Connect controller signals
    connect(boardController, &BoardController::highlightMovesSignal, 
            boardView, &BoardView::highlightMoves);
    connect(boardController, &BoardController::pegsRemainingChanged,
            this, &GameView::updatePegCount);
    connect(boardController, &BoardController::navigateToHome,
            this, &GameView::navigateToHome);
    
    // Connect view signals to controller
    connect(boardView, &BoardView::undoClicked,
            boardController, &BoardController::onUndoClicked);
    connect(boardView, &BoardView::resetClicked,
            boardController, &BoardController::onResetClicked);
    connect(boardView, &BoardView::homeClicked,
            boardController, &BoardController::onHomeClicked);
    connect(boardView, &BoardView::suggestMoveClicked,
            boardController, &BoardController::onSuggestMoveClicked);
    
    // Connect control buttons
    connect(undoButton, &QPushButton::clicked,
            boardController, &BoardController::onUndoClicked);
    connect(resetButton, &QPushButton::clicked,
            boardController, &BoardController::onResetClicked);    connect(homeButton, &QPushButton::clicked,
            boardController, &BoardController::onHomeClicked);
    connect(guideButton, &QPushButton::clicked,
            this, &GameView::showGuideDialog);
}

GameView::~GameView()
{
    // Qt's parent-child mechanism will handle cleanup
}

void GameView::setupUI()
{
    // Main vertical layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Control buttons layout at the top right
    controlButtonsLayout = new QHBoxLayout();
    controlButtonsLayout->addStretch(); // Push buttons to the right
      undoButton = new QPushButton("Undo", this);
    resetButton = new QPushButton("Reset", this);
    homeButton = new QPushButton("Home", this);
    guideButton = new QPushButton("Guide", this);
    
    // Apply consistent button styles
    ButtonStyles::applyControlStyle(undoButton);
    ButtonStyles::applyControlStyle(resetButton);
    ButtonStyles::applyControlStyle(homeButton);
    ButtonStyles::applyControlStyle(guideButton);
    
    controlButtonsLayout->addWidget(undoButton);
    controlButtonsLayout->addWidget(resetButton);
    controlButtonsLayout->addWidget(homeButton);
    controlButtonsLayout->addWidget(guideButton);
    controlButtonsLayout->setSpacing(10);
    
    mainLayout->addLayout(controlButtonsLayout);
    
    // Main game layout (horizontal: board on left, scoring on right)
    gameLayout = new QHBoxLayout();
    gameLayout->setSpacing(20);
    
    // Create simplified board view (without its own controls)
    boardView = new BoardView(this);
    boardView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    gameLayout->addWidget(boardView, 3); // Give board more space (75%)
    
    // Right side layout for scoring board
    rightSideLayout = new QVBoxLayout();
    rightSideLayout->setSpacing(15);
    
    // Scoring board title
    scoringBoardTitle = new QLabel("Score Board", this);
    QFont titleFont = scoringBoardTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    scoringBoardTitle->setFont(titleFont);
    scoringBoardTitle->setAlignment(Qt::AlignCenter);
    scoringBoardTitle->setStyleSheet(
        "QLabel {"
        "color: #2c3e50;"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ecf0f1, stop:1 #bdc3c7);"
        "border: 2px solid #95a5a6;"
        "border-radius: 8px;"
        "padding: 10px;"
        "}"
    );
    
    // Peg count display
    pegCountLabel = new QLabel("Pegs: 32", this);
    QFont labelFont = pegCountLabel->font();
    labelFont.setPointSize(14);
    labelFont.setBold(true);
    pegCountLabel->setFont(labelFont);
    pegCountLabel->setAlignment(Qt::AlignCenter);
    pegCountLabel->setStyleSheet(
        "QLabel {"
        "color: #27ae60;"
        "background: #ffffff;"
        "border: 2px solid #27ae60;"
        "border-radius: 8px;"
        "padding: 15px;"
        "}"    );
    
    rightSideLayout->addWidget(scoringBoardTitle);
    rightSideLayout->addWidget(pegCountLabel);
    rightSideLayout->addStretch(); // Push content to top
    
    gameLayout->addLayout(rightSideLayout, 1); // Give scoring board less space (25%)
    
    mainLayout->addLayout(gameLayout, 1); // Give game area most space
    
    setLayout(mainLayout);
}

void GameView::updatePegCount(int count)
{
    if (pegCountLabel) {
        pegCountLabel->setText(QString("Pegs: %1").arg(count));
    }
}

void GameView::setBoard(Board *board)
{
    if (boardController && boardView) {
        qDebug() << "GameView: Setting board model";
        
        // Set the board in both controller and view
        boardController->setBoardModel(board);
        boardView->setBoard(board);
        
        // Update initial peg count
        if (board) {
            updatePegCount(board->getPegCount());
        }
          qDebug() << "GameView: Board model set successfully";
    }
}

void GameView::showGuideDialog()
{
    // If overlay already exists, just show it
    if (guideOverlay) {
        guideOverlay->show();
        guideOverlay->raise();
        return;
    }
    
    // Create overlay widget
    guideOverlay = new QWidget(this);
    guideOverlay->setStyleSheet(
        "QWidget {"
        "background-color: rgba(0, 0, 0, 150);" // Semi-transparent background
        "}"
    );
    
    // Create the content widget
    QWidget *contentWidget = new QWidget(guideOverlay);
    contentWidget->setFixedSize(500, 400);
    contentWidget->setStyleSheet(
        "QWidget {"
        "background-color: #f8f9fa;"
        "border: 2px solid #dee2e6;"
        "border-radius: 12px;"
        "}"
    );
    
    // Create layout for the content
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 15, 20, 20);
    
    // Create close button positioned at top right
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    // Title
    QLabel *titleLabel = new QLabel("How to Play", contentWidget);
    titleLabel->setStyleSheet(
        "QLabel {"
        "color: #2c3e50;"
        "font-size: 18px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
        "}"
    );
    topLayout->addWidget(titleLabel);
    
    topLayout->addStretch();
    
    QPushButton *closeButton = new QPushButton("✕", contentWidget);
    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet(
        "QPushButton {"
        "background-color: #e74c3c;"
        "color: white;"
        "border: none;"
        "border-radius: 15px;"
        "font-weight: bold;"
        "font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "background-color: #a93226;"
        "}"
    );
    topLayout->addWidget(closeButton);
    contentLayout->addLayout(topLayout);
    
    // Add guide content
    QLabel *guideContent = new QLabel(contentWidget);
    guideContent->setWordWrap(true);
    guideContent->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    guideContent->setText(
        "<p><b>Goal:</b> Remove all but one peg from the board.</p>"
        "<p><b>How to play:</b></p>"
        "<p>• Jump one peg over another into an empty hole. The peg that is jumped over is removed.</p>"
        "<p>• Jumps can be made vertically or horizontally, but not diagonally.</p>"
        "<p>• Click on a peg to select it, then click on a valid empty hole to move it. Or drag a peg to an empty hole.</p>"
        "<p><b>Controls:</b></p>"
        "<p>• <b>Undo:</b> Take back your last move</p>"
        "<p>• <b>Reset:</b> Start the game over</p>"
        "<p>• <b>Home:</b> Return to the main menu</p>"
        "<p><b>Tip:</b> Try to work towards the center of the board!</p>"
    );
    guideContent->setStyleSheet(
        "QLabel {"
        "color: #2c3e50;"
        "background: transparent;"
        "border: none;"
        "font-size: 13px;"
        "line-height: 1.4;"
        "}"
    );
    
    contentLayout->addWidget(guideContent);
    
    // Position content widget in center of overlay
    QVBoxLayout *overlayLayout = new QVBoxLayout(guideOverlay);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->addStretch();
    
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(contentWidget);
    centerLayout->addStretch();
    
    overlayLayout->addLayout(centerLayout);
    overlayLayout->addStretch();
    
    // Connect close button to hide overlay
    connect(closeButton, &QPushButton::clicked, [this]() {
        if (guideOverlay) {
            guideOverlay->hide();
        }
    });
    
    // Make overlay fill the entire GameView
    guideOverlay->resize(this->size());
    
    // Show the overlay
    guideOverlay->show();
    guideOverlay->raise();
}

void GameView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Resize the overlay to match the new size
    if (guideOverlay && guideOverlay->isVisible()) {
        guideOverlay->resize(this->size());
    }
}
