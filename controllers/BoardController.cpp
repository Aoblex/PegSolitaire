#include "BoardController.h"
#include <QDebug>

BoardController::BoardController(QObject *parent)
    : QObject(parent),
      boardModel(nullptr),
      boardView(nullptr),
      selectedPosition({-1, -1}),
      hasPegSelected(false),
      currentKeyboardPosition({-1, -1})
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
                qDebug() << "BoardController: Selected peg at (" << pos.row << "," << pos.col << ") with no valid moves";
                // Still highlight the selected peg even if it has no valid moves
                QList<Move> selectedPegHighlight;
                // Create a dummy move just to highlight the selected peg
                selectedPegHighlight.append({pos, pos, pos});
                emit highlightMovesSignal(selectedPegHighlight);
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
    
    if (boardModel->isAntiPegMode()) {
        // Anti-peg mode: game ends when no more moves are available
        // Goal is to place as many pegs as possible
        if (!hasValidMoves) {
            qDebug() << "BoardController: Anti-peg game over! Final peg count:" << pegCount;
            emit gameOver();
            return;
        }
    } else {
        // Normal mode: check for win condition (only 1 peg left) or lose condition
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

void BoardController::onPegSelectionRequested(int direction)
{
    if (!boardModel) {
        return;
    }

    Position targetPeg = findNearestPeg(direction);
    
    if (targetPeg.row != -1 && targetPeg.col != -1) {
        // Select the found peg
        onPegCellClicked(targetPeg);
        qDebug() << "BoardController: Keyboard selected peg at (" << targetPeg.row << "," << targetPeg.col << ")";
    } else {
        qDebug() << "BoardController: No peg found in direction" << direction;
    }
}

void BoardController::onMoveRequested(int direction)
{
    if (!boardModel || !hasPegSelected) {
        qDebug() << "BoardController: No peg selected for arrow key move";
        return;
    }

    // Direction mapping: 0=up, 1=left, 2=down, 3=right
    int deltaRow = 0, deltaCol = 0;
    switch (direction) {
        case 0: deltaRow = -2; break; // Up (jump 2 rows)
        case 1: deltaCol = -2; break; // Left (jump 2 cols)
        case 2: deltaRow = 2; break;  // Down (jump 2 rows)
        case 3: deltaCol = 2; break;  // Right (jump 2 cols)
    }

    Position targetPos(selectedPosition.row + deltaRow, selectedPosition.col + deltaCol);
    
    // Check if this move is valid among current valid moves
    for (const Move &move : currentValidMoves) {
        if (move.to.row == targetPos.row && move.to.col == targetPos.col) {
            // Valid move found, execute it
            onPegCellClicked(targetPos);
            qDebug() << "BoardController: Executed arrow key move to (" << targetPos.row << "," << targetPos.col << ")";
            return;
        }
    }
    
    qDebug() << "BoardController: Invalid arrow key move attempted to (" << targetPos.row << "," << targetPos.col << ")";
}

Position BoardController::findNearestPeg(int direction)
{
    if (!boardModel) {
        return Position(-1, -1);
    }

    // Start from center if no current keyboard position
    if (currentKeyboardPosition.row == -1) {
        currentKeyboardPosition = getBoardCenter();
    }

    // Direction mapping: 0=up, 1=left, 2=down, 3=right
    int deltaRow = 0, deltaCol = 0;
    switch (direction) {
        case 0: deltaRow = -1; break; // Up
        case 1: deltaCol = -1; break; // Left
        case 2: deltaRow = 1; break;  // Down
        case 3: deltaCol = 1; break;  // Right
    }

    Position searchPos = currentKeyboardPosition;
    
    // First, search in the specified direction from current position
    for (int i = 1; i < qMax(boardModel->getRows(), boardModel->getCols()); ++i) {
        searchPos.row += deltaRow;
        searchPos.col += deltaCol;
        
        // Check bounds
        if (searchPos.row < 0 || searchPos.row >= boardModel->getRows() ||
            searchPos.col < 0 || searchPos.col >= boardModel->getCols()) {
            break;
        }
        
        // Check if there's a peg at this position
        if (boardModel->getPegState(searchPos) == PegState::Peg) {
            currentKeyboardPosition = searchPos;
            return searchPos;
        }
    }

    // If no peg found in that direction, search the entire board systematically
    // We'll search in a spiral pattern starting from the current position
    Position bestPeg = findNearestPegInDirection(direction);
    if (bestPeg.row != -1 && bestPeg.col != -1) {
        currentKeyboardPosition = bestPeg;
        return bestPeg;
    }

    // If still no peg found, find any peg on the board
    for (int r = 0; r < boardModel->getRows(); ++r) {
        for (int c = 0; c < boardModel->getCols(); ++c) {
            Position testPos(r, c);
            if (boardModel->getPegState(testPos) == PegState::Peg) {
                currentKeyboardPosition = testPos;
                return testPos;
            }
        }
    }

    return Position(-1, -1); // No peg found
}

Position BoardController::getBoardCenter()
{
    if (!boardModel) {
        return Position(-1, -1);
    }

    // For standard peg solitaire, find the actual center of the playable area
    int centerRow = boardModel->getRows() / 2;
    int centerCol = boardModel->getCols() / 2;
    
    // Look for a valid position near the center
    for (int radius = 0; radius < qMax(boardModel->getRows(), boardModel->getCols()) / 2; ++radius) {
        for (int dr = -radius; dr <= radius; ++dr) {
            for (int dc = -radius; dc <= radius; ++dc) {
                Position testPos(centerRow + dr, centerCol + dc);
                
                if (testPos.row >= 0 && testPos.row < boardModel->getRows() &&
                    testPos.col >= 0 && testPos.col < boardModel->getCols()) {
                    
                    PegState state = boardModel->getPegState(testPos);
                    if (state != PegState::Blocked) {
                        return testPos;
                    }
                }
            }
        }
    }
    
    return Position(centerRow, centerCol);
}

Position BoardController::findNearestPegInDirection(int direction)
{
    if (!boardModel) {
        return Position(-1, -1);
    }

    Position currentPos = currentKeyboardPosition;
    Position bestPeg(-1, -1);
    double bestDistance = -1;

    // Search all pegs on the board and find the best one in the specified direction
    for (int r = 0; r < boardModel->getRows(); ++r) {
        for (int c = 0; c < boardModel->getCols(); ++c) {
            Position testPos(r, c);
            
            // Skip if not a peg or if it's the current position
            if (boardModel->getPegState(testPos) != PegState::Peg || 
                (testPos.row == currentPos.row && testPos.col == currentPos.col)) {
                continue;
            }

            // Check if this peg is in the desired direction
            bool isInDirection = false;
            double distance = 0;
            
            switch (direction) {
                case 0: // Up
                    if (testPos.row < currentPos.row) {
                        isInDirection = true;
                        // Prioritize pegs directly above, then by distance
                        distance = (currentPos.row - testPos.row) + abs(testPos.col - currentPos.col) * 0.1;
                    }
                    break;
                case 1: // Left
                    if (testPos.col < currentPos.col) {
                        isInDirection = true;
                        // Prioritize pegs directly to the left, then by distance
                        distance = (currentPos.col - testPos.col) + abs(testPos.row - currentPos.row) * 0.1;
                    }
                    break;
                case 2: // Down
                    if (testPos.row > currentPos.row) {
                        isInDirection = true;
                        // Prioritize pegs directly below, then by distance
                        distance = (testPos.row - currentPos.row) + abs(testPos.col - currentPos.col) * 0.1;
                    }
                    break;
                case 3: // Right
                    if (testPos.col > currentPos.col) {
                        isInDirection = true;
                        // Prioritize pegs directly to the right, then by distance
                        distance = (testPos.col - currentPos.col) + abs(testPos.row - currentPos.row) * 0.1;
                    }
                    break;
            }

            // If this peg is in the direction and is the closest so far
            if (isInDirection && (bestDistance < 0 || distance < bestDistance)) {
                bestPeg = testPos;
                bestDistance = distance;
            }
        }
    }

    return bestPeg;
}