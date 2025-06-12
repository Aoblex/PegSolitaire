#include "GameView.h"
#include "utils/ButtonStyles.h"
#include <QDebug>
#include <QFont>

GameView::GameView(QWidget *parent)
    : QWidget(parent),
      mainLayout(nullptr),
      gameLayout(nullptr),
      rightSideLayout(nullptr),
      controlButtonsLayout(nullptr),
      boardView(nullptr),
      scoringBoardTitle(nullptr),
      pegCountLabel(nullptr),
      instructionLabel(nullptr),
      undoButton(nullptr),
      resetButton(nullptr),
      homeButton(nullptr),
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
            boardController, &BoardController::onResetClicked);
    connect(homeButton, &QPushButton::clicked,
            boardController, &BoardController::onHomeClicked);
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
    
    // Apply consistent button styles
    ButtonStyles::applyControlStyle(undoButton);
    ButtonStyles::applyControlStyle(resetButton);
    ButtonStyles::applyControlStyle(homeButton);
    
    controlButtonsLayout->addWidget(undoButton);
    controlButtonsLayout->addWidget(resetButton);
    controlButtonsLayout->addWidget(homeButton);
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
        "}"
    );
    
    // Instruction label
    instructionLabel = new QLabel("Click a peg to select it,\nthen click an empty hole to move", this);
    instructionLabel->setFont(labelFont);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    instructionLabel->setStyleSheet(
        "QLabel {"
        "color: #34495e;"
        "background: #f8f9fa;"
        "border: 1px solid #dee2e6;"
        "border-radius: 6px;"
        "padding: 10px;"
        "}"
    );
    
    rightSideLayout->addWidget(scoringBoardTitle);
    rightSideLayout->addWidget(pegCountLabel);
    rightSideLayout->addWidget(instructionLabel);
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
