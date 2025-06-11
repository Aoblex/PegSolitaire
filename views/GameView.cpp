#include "GameView.h"
#include <QDebug>

GameView::GameView(QWidget *parent)
    : QWidget(parent),
      mainLayout(nullptr),
      boardView(nullptr),
      boardController(nullptr)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create board view
    boardView = new BoardView(this);
    mainLayout->addWidget(boardView);
    
    // Create board controller
    boardController = new BoardController(this);
    
    // Connect board view to controller
    boardController->setBoardView(boardView);
    
    // Connect controller signals
    connect(boardController, &BoardController::highlightMovesSignal, 
            boardView, &BoardView::highlightMoves);
    connect(boardController, &BoardController::pegsRemainingChanged,
            boardView, &BoardView::updatePegCount);
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

    setLayout(mainLayout);
}

GameView::~GameView()
{
    // Qt's parent-child mechanism will handle cleanup
}

void GameView::setBoard(Board *board)
{
    if (boardController && boardView) {
        qDebug() << "GameView: Setting board model";
        
        // Set the board in both controller and view
        boardController->setBoardModel(board);
        boardView->setBoard(board);
        
        qDebug() << "GameView: Board model set successfully";
    }
}
