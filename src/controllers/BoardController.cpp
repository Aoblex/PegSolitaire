#include "BoardController.h"
#include <QDebug>
#include <QMutexLocker>

// Static member definitions removed - now managed by StrategyWorker

BoardController::BoardController(QObject *parent)
    : QObject(parent),
      boardModel(nullptr),
      boardView(nullptr),
      selectedPosition({-1, -1}),
      hasPegSelected(false),
      loadingCircle(nullptr),
      strategyWorker(nullptr),
      isComputingStrategy(false),
      currentKeyboardPosition({-1, -1})
{
    // Create strategy worker
    strategyWorker = new StrategyWorker(this);
    connect(strategyWorker, &StrategyWorker::strategyComputed,
            this, &BoardController::onStrategyComputed);
    connect(strategyWorker, &StrategyWorker::computationCancelled,
            this, &BoardController::onStrategyComputationCancelled);
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
        connect(boardView, &BoardView::boardViewResized, this, &BoardController::onBoardViewResized);
        
        // If loading circle exists, update its parent
        if (loadingCircle) {
            loadingCircle->setParent(boardView);
            loadingCircle->resize(boardView->size());
        }
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
                emit informationUpdated(QString("Selected peg at (%1,%2)\n%3 possible moves").arg(pos.row).arg(pos.col).arg(currentValidMoves.size()));
                emit highlightMovesSignal(currentValidMoves);
            } else {
                qDebug() << "BoardController: Selected peg at (" << pos.row << "," << pos.col << ") with no valid moves";
                emit informationUpdated(QString("Selected peg at (%1,%2)\nNo valid moves available").arg(pos.row).arg(pos.col));
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
            }            if (moveFound && boardModel->performMove(attemptedMove)) {
                // Cancel any ongoing strategy computation since board state changed
                if (isComputingStrategy && strategyWorker) {
                    strategyWorker->requestCancellation();
                    if (loadingCircle) {
                        loadingCircle->stopAnimation();
                    }
                    isComputingStrategy = false;
                }
                
                qDebug() << "BoardController: Move performed from (" << attemptedMove.from.row << "," << attemptedMove.from.col 
                         << ") to (" << attemptedMove.to.row << "," << attemptedMove.to.col 
                         << ") jumping (" << attemptedMove.jumped.row << "," << attemptedMove.jumped.col << ")";
                emit informationUpdated(QString("Move executed!\nFrom (%1,%2) to (%3,%4)\nJumped over (%5,%6)")
                                      .arg(attemptedMove.from.row).arg(attemptedMove.from.col)
                                      .arg(attemptedMove.to.row).arg(attemptedMove.to.col)
                                      .arg(attemptedMove.jumped.row).arg(attemptedMove.jumped.col));
                
                clearSelection();
                updateView();
                emit pegsRemainingChanged(boardModel->getPegCount());
                emit boardChanged();
                checkGameStatus();
            } else {
                qDebug() << "BoardController: Invalid move attempted";
                emit informationUpdated("Invalid move attempted!\nTry selecting a valid destination.");
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
        // Cancel any ongoing strategy computation since board state changed
        if (isComputingStrategy && strategyWorker) {
            strategyWorker->requestCancellation();
            if (loadingCircle) {
                loadingCircle->stopAnimation();
            }
            isComputingStrategy = false;
        }
        
        qDebug() << "BoardController: Move undone successfully";
        emit informationUpdated("Move undone successfully!\nReturned to previous state.");
        clearSelection();
        updateView();
        emit pegsRemainingChanged(boardModel->getPegCount());
        emit boardChanged();
        checkGameStatus();
    } else {
        qDebug() << "BoardController: No moves to undo";
        emit informationUpdated("No moves to undo!\nNo previous moves available.");
    }
}

void BoardController::onResetClicked()
{
    if (!boardModel) {
        return;
    }
    
    // Cancel any ongoing strategy computation
    if (isComputingStrategy && strategyWorker) {
        strategyWorker->requestCancellation();
        if (loadingCircle) {
            loadingCircle->stopAnimation();
        }
        isComputingStrategy = false;
    }
    
    qDebug() << "BoardController: Resetting board";
    emit informationUpdated("Board reset!\nGame restarted with fresh board.");
    
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
    
    // If already computing strategy, ignore the request
    if (isComputingStrategy) {
        qDebug() << "BoardController: Strategy computation already in progress";
        emit informationUpdated("Strategy computation in progress...\nPlease wait for current calculation to complete.");
        return;
    }
    
    // Start the asynchronous strategy computation
    isComputingStrategy = true;
    
    // Set up the loading circle if not already created
    if (!loadingCircle) {
        loadingCircle = new LoadingCircle(boardView);
    }
    
    // Copy current board state to pass to worker thread
    QVector<QVector<PegState>> boardData;
    boardData.resize(boardModel->getRows());
    for (int r = 0; r < boardModel->getRows(); ++r) {
        boardData[r].resize(boardModel->getCols());
        for (int c = 0; c < boardModel->getCols(); ++c) {
            Position pos(r, c);
            boardData[r][c] = boardModel->getPegState(pos);
        }
    }
    
    // Show loading circle with appropriate message
    if (loadingCircle && boardView) {
        loadingCircle->setMessage("Searching for winning move, this may take a while...");
        loadingCircle->resize(boardView->size());
        loadingCircle->startAnimation();
    }
    
    // Start the strategy worker thread
    strategyWorker->computeStrategy(boardModel->getBoardType(), boardData);
    
    emit informationUpdated("Analyzing board...\nSearching for optimal strategy...");
    qDebug() << "BoardController: Started asynchronous strategy computation";
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
    
    // Check for winning condition first
    if (boardModel->isWinningState()) {
        if (boardModel->isAntiPegMode()) {
            qDebug() << "BoardController: Anti-peg victory! One empty space remains at starting position.";
            emit informationUpdated("Victory!\nAnti-peg game won!\nOnly the starting position is empty!");
        } else {
            qDebug() << "BoardController: Victory! One peg remains at starting position.";
            emit informationUpdated("Victory!\nPerfect game!\nOne peg remains at the starting position!");
        }
        emit gameOver();
        return;
    }
    
    int pegCount = boardModel->getPegCount();
    bool hasValidMoves = !boardModel->getValidMoves().isEmpty();
    
    if (boardModel->isAntiPegMode()) {
        // Anti-peg mode: game ends when no more moves are available
        if (!hasValidMoves) {
            qDebug() << "BoardController: Anti-peg game over! Final peg count:" << pegCount;
            emit informationUpdated(QString("Anti-peg game complete!\nFinal peg count: %1\nNo more moves possible.").arg(pegCount));
            emit gameOver();
            return;
        }
    }
    else {
        // Check for lose condition: no valid moves available (and more than 1 peg)
        if (!hasValidMoves && pegCount > 1) {
            qDebug() << "BoardController: Game over! No valid moves available. Final peg count:" << pegCount;
            emit informationUpdated(QString("Game Over!\nNo valid moves available\n%1 pegs remaining").arg(pegCount));
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
    
    // Only return strategic moves that guarantee a win
    Move strategicMove = getStrategicMove();
    return strategicMove; // This will be invalid if no winning solution exists
}

Move BoardController::getStrategicMove()
{
    if (!boardModel) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    // Check if board is solvable first
    if (!isBoardSolvable()) {
        qDebug() << "BoardController: Dead game detected - no winning solution exists";
        emit deadGameDetected();
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    // Find the best strategic move
    return findBestStrategicMove();
}

bool BoardController::isBoardSolvable()
{
    if (!boardModel) {
        return false;
    }

    // Create a copy of the board to test solvability
    Board* testBoard = new Board(boardModel->getBoardType(), this);
    
    // Copy current state
    for (int r = 0; r < boardModel->getRows(); ++r) {
        for (int c = 0; c < boardModel->getCols(); ++c) {
            Position pos(r, c);
            testBoard->setPegState(pos, boardModel->getPegState(pos));
        }
    }

    bool solvable = solveBoard(testBoard);
    
    testBoard->deleteLater();
    return solvable;
}

bool BoardController::solveBoard(Board* board)
{
    if (!board) {
        return false;
    }

    // Check win condition - if only 1 peg left and it's at the center
    if (board->isWinningState()) {
        return true;
    }

    // Get unique board state identifier
    quint64 boardStateId = board->getBoardStateId();
      // Check if this board state (or any symmetric equivalent) has been flagged as unsolvable
    {
        QMutexLocker locker(&StrategyWorker::failedStatesMutex);
        if (StrategyWorker::failedBoardStates.contains(boardStateId)) {
            return false; // This board state or a symmetric equivalent leads to no solution
        }
    }

    // Get all possible moves
    QVector<Move> moves = board->getValidMoves();
    if (moves.isEmpty()) {
        // No moves available and not in winning state - flag this state as failed
        QMutexLocker locker(&StrategyWorker::failedStatesMutex);
        StrategyWorker::failedBoardStates.insert(boardStateId);
        return false;
    }

    // Try each move recursively
    for (const Move& move : moves) {
        // Make the move
        if (board->performMove(move)) {
            // Recursively check if this leads to a solution
            if (solveBoard(board)) {
                // Found a solution! Undo the move before returning
                board->undoLastMove();
                return true;
            }
            // Undo the move and try next
            board->undoLastMove();
        }
    }    // No solution found from this state - flag it as failed
    {
        QMutexLocker locker(&StrategyWorker::failedStatesMutex);
        StrategyWorker::failedBoardStates.insert(boardStateId);
    }
    
    return false;
}

Move BoardController::findBestStrategicMove()
{
    if (!boardModel) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    QVector<Move> allMoves = boardModel->getValidMoves();
    if (allMoves.isEmpty()) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    // Test each move to see which ones lead to winning solutions
    QVector<Move> winningMoves;
    
    for (const Move& move : allMoves) {
        // Create a test board
        Board* testBoard = new Board(boardModel->getBoardType(), this);
        
        // Copy current state
        for (int r = 0; r < boardModel->getRows(); ++r) {
            for (int c = 0; c < boardModel->getCols(); ++c) {
                Position pos(r, c);
                testBoard->setPegState(pos, boardModel->getPegState(pos));
            }
        }

        // Try this move
        if (testBoard->performMove(move)) {
            // Check if this state is still solvable
            if (solveBoard(testBoard)) {
                winningMoves.append(move);
            }
        }
          testBoard->deleteLater();
    }
    
    // Return the first winning move found, or invalid move if none guarantee win
    if (!winningMoves.isEmpty()) {
        qDebug() << "BoardController: Found" << winningMoves.size() << "winning moves";
        return winningMoves.first();
    }

    qDebug() << "BoardController: No guaranteed winning moves found";
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
        QString directionName[] = {"Up", "Left", "Down", "Right"};
        emit informationUpdated(QString("Keyboard navigation:\nSelected peg at (%1,%2)\nDirection: %3")
                              .arg(targetPeg.row).arg(targetPeg.col).arg(directionName[direction]));
    } else {
        qDebug() << "BoardController: No peg found in direction" << direction;
        QString directionName[] = {"Up", "Left", "Down", "Right"};
        emit informationUpdated(QString("Keyboard navigation:\nNo peg found in %1 direction\nTry a different direction").arg(directionName[direction]));
    }
}

void BoardController::onMoveRequested(int direction)
{    if (!boardModel || !hasPegSelected) {
        qDebug() << "BoardController: No peg selected for arrow key move";
        emit informationUpdated("Arrow key move failed!\nPlease select a peg first\nusing WASD keys.");
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
        if (move.to.row == targetPos.row && move.to.col == targetPos.col) {            // Valid move found, execute it
            onPegCellClicked(targetPos);
            qDebug() << "BoardController: Executed arrow key move to (" << targetPos.row << "," << targetPos.col << ")";
            QString directionName[] = {"Up", "Left", "Down", "Right"};
            emit informationUpdated(QString("Arrow key move executed!\nDirection: %1\nMoved to (%2,%3)")
                                  .arg(directionName[direction]).arg(targetPos.row).arg(targetPos.col));
            return;
        }
    }
    
    qDebug() << "BoardController: Invalid arrow key move attempted to (" << targetPos.row << "," << targetPos.col << ")";
    QString directionName[] = {"Up", "Left", "Down", "Right"};
    emit informationUpdated(QString("Invalid arrow key move!\nDirection: %1\nNo valid move to (%2,%3)")
                          .arg(directionName[direction]).arg(targetPos.row).arg(targetPos.col));
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

void BoardController::onStrategyComputed(const Move &move, bool isDeadGame)
{
    qDebug() << "BoardController: Strategy computation completed";
    
    // Stop the loading animation
    if (loadingCircle) {
        loadingCircle->stopAnimation();
    }
    
    isComputingStrategy = false;
    
    if (isDeadGame) {
        qDebug() << "BoardController: Dead game detected by strategy worker";
        emit informationUpdated("💀 Dead game detected!\nNo winning moves available.\nThe current board state cannot be won.");
        emit deadGameDetected();
    } else if (move.from.row != -1) {
        qDebug() << "BoardController: Suggesting winning move from (" << move.from.row << "," << move.from.col 
                 << ") to (" << move.to.row << "," << move.to.col << ")";
        emit informationUpdated(QString("✨ Winning move found!\nFrom (%1,%2) to (%3,%4)\nThis move guarantees victory!")
                              .arg(move.from.row).arg(move.from.col)
                              .arg(move.to.row).arg(move.to.col));
        
        // Clear current selection and highlight suggested move
        clearSelection();
        selectedPosition = move.from;
        hasPegSelected = true;
        currentValidMoves.clear();
        currentValidMoves.append(move);
        
        emit highlightMovesSignal(currentValidMoves);
    } else {
        qDebug() << "BoardController: No winning moves found by strategy worker";
        emit informationUpdated("🤔 No guaranteed winning moves found!\nThe current position is too complex\nor may not have a perfect solution.");
    }
}

void BoardController::onStrategyComputationCancelled()
{
    qDebug() << "BoardController: Strategy computation was cancelled";
    
    // Stop the loading animation
    if (loadingCircle) {
        loadingCircle->stopAnimation();
    }
    
    isComputingStrategy = false;
    
    emit informationUpdated("Strategy computation cancelled.\nYou can try again by pressing space.");
}

void BoardController::onBoardViewResized()
{
    // Resize the loading circle to match the new board view size
    if (loadingCircle && boardView) {
        loadingCircle->resize(boardView->size());
        qDebug() << "BoardController: Loading circle resized to match board view size:" << boardView->size();
    }
}

void BoardController::clearFailedStatesCache()
{
    // Clear the StrategyWorker's failed states cache
    QMutexLocker locker(&StrategyWorker::failedStatesMutex);
    StrategyWorker::failedBoardStates.clear();
    qDebug() << "BoardController: Failed board states cache cleared";
}