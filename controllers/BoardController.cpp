#include "BoardController.h"
#include <QDebug>

BoardController::BoardController(QObject *parent)
    : QObject(parent),
      boardModel(nullptr),
      boardView(nullptr),
      selectedPosition({-1, -1}),
      hasPegSelected(false)
{
}

void BoardController::setBoardModel(Board *model)
{
    boardModel = model;
    
    if (boardModel) {
        // Update the view when model is set
        updateView();
        emit pegsRemainingChanged(boardModel->getPegCount());
        
        // Check initial game status
        checkGameStatus();
    }
}

void BoardController::setBoardView(BoardView *view)
{
    boardView = view;
    
    if (boardView) {
        // Connect view signals to controller slots
        connect(boardView, &BoardView::pegClicked, this, &BoardController::onPegCellClicked);
    }
}

Board* BoardController::getBoardModel() const
{
    return boardModel;
}

void BoardController::onPegCellClicked(const Position &pos)
{
    if (!boardModel) {
        qDebug() << "BoardController: No board model set";
        return;
    }
    
    PegState clickedState = boardModel->getPegState(pos);
    
    if (!hasPegSelected) {
        // First click - select a peg
        if (clickedState == PegState::Peg) {
            selectedPosition = pos;
            hasPegSelected = true;
            
            // Get valid moves from this position
            currentValidMoves = getMovesFromPosition(pos);
            
            if (!currentValidMoves.isEmpty()) {
                qDebug() << "BoardController: Selected peg at (" << pos.row << "," << pos.col << ") with" << currentValidMoves.size() << "possible moves";
                emit highlightMovesSignal(currentValidMoves);
            } else {
                qDebug() << "BoardController: No valid moves from position (" << pos.row << "," << pos.col << ")";
                clearSelection();
            }
        }
    } else {
        // Second click - try to make a move or select different peg
        if (clickedState == PegState::Peg && !(pos.row == selectedPosition.row && pos.col == selectedPosition.col)) {
            // Clicking on a different peg - change selection
            clearSelection();
            onPegCellClicked(pos); // Recursively handle as new selection
            return;
        }
        
        if (clickedState == PegState::Empty) {
            // Try to make a move to this empty position
            Move attemptedMove = {selectedPosition, {-1, -1}, pos};
            
            // Find the complete move (including jumped position)
            bool moveFound = false;
            for (const Move &move : currentValidMoves) {
                if (move.to.row == pos.row && move.to.col == pos.col) {
                    attemptedMove = move;
                    moveFound = true;
                    break;
                }
            }
            
            if (moveFound && boardModel->performMove(attemptedMove)) {
                qDebug() << "BoardController: Move performed from (" << attemptedMove.from.row << "," << attemptedMove.from.col 
                         << ") to (" << attemptedMove.to.row << "," << attemptedMove.to.col 
                         << ") jumping (" << attemptedMove.jumped.row << "," << attemptedMove.jumped.col << ")";
                
                clearSelection();
                updateView();
                emit pegsRemainingChanged(boardModel->getPegCount());
                emit boardChanged();
                checkGameStatus();
            } else {
                qDebug() << "BoardController: Invalid move attempted";
            }
        } else {
            // Clicking on same peg - deselect
            if (pos.row == selectedPosition.row && pos.col == selectedPosition.col) {
                clearSelection();
            }
        }
    }
}

void BoardController::onUndoClicked()
{
    if (!boardModel) {
        return;
    }
    
    if (boardModel->undoLastMove()) {
        qDebug() << "BoardController: Move undone successfully";
        clearSelection();
        updateView();
        emit pegsRemainingChanged(boardModel->getPegCount());
        emit boardChanged();
        checkGameStatus();
    } else {
        qDebug() << "BoardController: No moves to undo";
    }
}

void BoardController::onResetClicked()
{
    if (!boardModel) {
        return;
    }
    
    qDebug() << "BoardController: Resetting board";
    
    // Re-initialize the board with its current type
    BoardType currentType = boardModel->getBoardType();
    boardModel->initializeBoard(currentType);
    
    clearSelection();
    updateView();
    emit pegsRemainingChanged(boardModel->getPegCount());
    emit boardChanged();
    checkGameStatus();
}

void BoardController::onHomeClicked()
{
    qDebug() << "BoardController: Navigating to home";
    emit navigateToHome();
}

void BoardController::onSuggestMoveClicked()
{
    if (!boardModel) {
        return;
    }
    
    Move suggestedMove = getSuggestedMove();
    
    if (suggestedMove.from.row != -1) {
        qDebug() << "BoardController: Suggesting move from (" << suggestedMove.from.row << "," << suggestedMove.from.col 
                 << ") to (" << suggestedMove.to.row << "," << suggestedMove.to.col << ")";
        
        // Clear current selection and highlight suggested move
        clearSelection();
        selectedPosition = suggestedMove.from;
        hasPegSelected = true;
        currentValidMoves.clear();
        currentValidMoves.append(suggestedMove);
        
        emit highlightMovesSignal(currentValidMoves);
    } else {
        qDebug() << "BoardController: No moves available for suggestion";
    }
}

void BoardController::updateView()
{
    if (boardView) {
        boardView->updateView();
    }
}

void BoardController::clearSelection()
{
    selectedPosition = {-1, -1};
    hasPegSelected = false;
    currentValidMoves.clear();
    
    // Emit empty moves list to clear highlights
    emit highlightMovesSignal(QList<Move>());
}

QList<Move> BoardController::getMovesFromPosition(const Position &fromPos)
{
    if (!boardModel) {
        return QList<Move>();
    }
    
    QVector<Move> allMoves = boardModel->getValidMoves();
    QList<Move> movesFromPos;
    
    for (const Move &move : allMoves) {
        if (move.from.row == fromPos.row && move.from.col == fromPos.col) {
            movesFromPos.append(move);
        }
    }
    
    return movesFromPos;
}

void BoardController::checkGameStatus()
{
    if (!boardModel) {
        return;
    }
    
    int pegCount = boardModel->getPegCount();
    bool hasValidMoves = !boardModel->getValidMoves().isEmpty();
    
    // Check for win condition: only 1 peg left
    if (pegCount == 1) {
        qDebug() << "BoardController: Player won! Only 1 peg remaining.";
        emit gameOver();
        return;
    }
    
    // Check for lose condition: no valid moves available (and more than 1 peg)
    if (!hasValidMoves && pegCount > 1) {
        qDebug() << "BoardController: Game over! No valid moves available. Final peg count:" << pegCount;
        emit gameOver();
        return;
    }
}

Move BoardController::getSuggestedMove()
{
    if (!boardModel) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }
    
    QVector<Move> allMoves = boardModel->getValidMoves();
    
    if (!allMoves.isEmpty()) {
        return allMoves.first();
    }
    
    return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
}