#include "StrategyWorker.h"
#include <QDebug>
#include <QMutexLocker>

// Static member definitions for optimization
QSet<quint64> StrategyWorker::failedBoardStates;
QMutex StrategyWorker::failedStatesMutex;

StrategyWorker::StrategyWorker(QObject *parent)
    : QThread(parent),
      cancelRequested(false)
{
}

StrategyWorker::~StrategyWorker()
{
    requestCancellation();
    wait(); // Wait for thread to finish
}

void StrategyWorker::computeStrategy(BoardType boardType, const QVector<QVector<PegState>> &boardData)
{
    QMutexLocker locker(&cancelMutex);
    cancelRequested = false;
    currentBoardType = boardType;
    currentBoardData = boardData;
    locker.unlock();
    
    if (!isRunning()) {
        start();
    }
}

void StrategyWorker::requestCancellation()
{
    QMutexLocker locker(&cancelMutex);
    cancelRequested = true;
}

bool StrategyWorker::isCancellationRequested()
{
    QMutexLocker locker(&cancelMutex);
    return cancelRequested;
}

void StrategyWorker::run()
{
    QPair<Move, bool> result = performStrategyComputation();
    
    if (isCancellationRequested()) {
        emit computationCancelled();
    } else {
        emit strategyComputed(result.first, result.second);
    }
}

QPair<Move, bool> StrategyWorker::performStrategyComputation()
{
    // Create a board for computation
    Board* testBoard = new Board();
    testBoard->initializeBoard(currentBoardType);
    
    // Copy the current board state
    for (int r = 0; r < currentBoardData.size(); ++r) {
        for (int c = 0; c < currentBoardData[r].size(); ++c) {
            Position pos(r, c);
            testBoard->setPegState(pos, currentBoardData[r][c]);
        }
    }
    
    Move resultMove{Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    bool isDeadGame = false;
    
    try {
        // Check if cancelled before heavy computation
        if (isCancellationRequested()) {
            testBoard->deleteLater();
            return qMakePair(resultMove, isDeadGame);
        }
        
        // Check if board is solvable first
        if (!isBoardSolvable(testBoard)) {
            qDebug() << "StrategyWorker: Dead game detected - no winning solution exists";
            isDeadGame = true;
        } else {
            // Find the best strategic move
            resultMove = findBestStrategicMove(testBoard);
        }
    } catch (...) {
        qDebug() << "StrategyWorker: Exception during computation";
        isDeadGame = true;
    }
    
    testBoard->deleteLater();
    return qMakePair(resultMove, isDeadGame);
}

bool StrategyWorker::isBoardSolvable(Board* board)
{
    if (!board) {
        return false;
    }
    
    // Create a copy for testing
    Board* testBoard = new Board(board->getBoardType());
    
    // Copy current state
    for (int r = 0; r < board->getRows(); ++r) {
        for (int c = 0; c < board->getCols(); ++c) {
            Position pos(r, c);
            testBoard->setPegState(pos, board->getPegState(pos));
        }
        
        // Check for cancellation periodically
        if (isCancellationRequested()) {
            testBoard->deleteLater();
            return false;
        }
    }

    bool solvable = solveBoard(testBoard);
    testBoard->deleteLater();
    return solvable;
}

bool StrategyWorker::solveBoard(Board* board)
{
    if (!board || isCancellationRequested()) {
        return false;
    }

    // Check win condition
    if (board->isWinningState()) {
        return true;
    }

    // Get unique board state identifier
    quint64 boardStateId = board->getBoardStateId();    // Check if this board state has been flagged as unsolvable
    {
        QMutexLocker locker(&failedStatesMutex);
        if (failedBoardStates.contains(boardStateId)) {
            return false;
        }
    }

    // Get all possible moves
    QVector<Move> moves = board->getValidMoves();
    if (moves.isEmpty()) {
        // No moves available and not in winning state - flag as failed
        QMutexLocker locker(&failedStatesMutex);
        failedBoardStates.insert(boardStateId);
        return false;
    }

    // Try each move recursively
    for (const Move& move : moves) {
        // Check for cancellation
        if (isCancellationRequested()) {
            return false;
        }
        
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
        QMutexLocker locker(&failedStatesMutex);
        failedBoardStates.insert(boardStateId);
    }
    
    return false;
}

Move StrategyWorker::findBestStrategicMove(Board* board)
{
    if (!board || isCancellationRequested()) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    QVector<Move> allMoves = board->getValidMoves();
    if (allMoves.isEmpty()) {
        return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
    }

    // Test each move to see which ones lead to winning solutions
    QVector<Move> winningMoves;
    
    for (const Move& move : allMoves) {
        // Check for cancellation
        if (isCancellationRequested()) {
            return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
        }
        
        // Create a test board
        Board* testBoard = new Board(board->getBoardType());
        
        // Copy current state
        for (int r = 0; r < board->getRows(); ++r) {
            for (int c = 0; c < board->getCols(); ++c) {
                Position pos(r, c);
                testBoard->setPegState(pos, board->getPegState(pos));
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
        qDebug() << "StrategyWorker: Found" << winningMoves.size() << "winning moves";
        return winningMoves.first();
    }

    qDebug() << "StrategyWorker: No guaranteed winning moves found";
    return {Position{-1, -1}, Position{-1, -1}, Position{-1, -1}};
}
