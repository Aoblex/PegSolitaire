#ifndef STRATEGYWORKER_H
#define STRATEGYWORKER_H

#include <QThread>
#include <QMutex>
#include "models/Board.h"

/**
 * @brief Worker thread for computing strategy moves in the background
 * 
 * This class runs the computationally expensive strategy calculation
 * in a separate thread to prevent UI freezing during calculation.
 */
class StrategyWorker : public QThread
{
    Q_OBJECT

public:
    explicit StrategyWorker(QObject *parent = nullptr);
    ~StrategyWorker();

    /**
     * @brief Start computing strategy for the given board state
     * @param boardType The type of board
     * @param boardData Current board state data (row by row)
     */
    void computeStrategy(BoardType boardType, const QVector<QVector<PegState>> &boardData);

    /**
     * @brief Request cancellation of the current computation
     */
    void requestCancellation();

    // Static optimization cache - shared across all instances (public for access by BoardController)
    static QSet<quint64> failedBoardStates;
    static QMutex failedStatesMutex;

signals:
    /**
     * @brief Emitted when strategy computation is complete
     * @param move The suggested move, or invalid move if no solution found
     * @param isDeadGame True if the game is determined to be unwinnable
     */
    void strategyComputed(const Move &move, bool isDeadGame);

    /**
     * @brief Emitted when strategy computation is cancelled
     */
    void computationCancelled();

protected:
    void run() override;

private:
    QMutex cancelMutex;
    bool cancelRequested;
    BoardType currentBoardType;
    QVector<QVector<PegState>> currentBoardData;
    
    /**
     * @brief Check if cancellation has been requested
     * @return True if computation should be cancelled
     */
    bool isCancellationRequested();
    
    /**
     * @brief Perform the actual strategy computation
     * @return The computed move and whether it's a dead game
     */
    QPair<Move, bool> performStrategyComputation();
    
    /**
     * @brief Check if the board is solvable
     * @param board The board to check
     * @return True if solvable
     */
    bool isBoardSolvable(Board* board);
    
    /**
     * @brief Solve the board using recursive backtracking
     * @param board The board to solve
     * @return True if a solution exists
     */
    bool solveBoard(Board* board);
      /**
     * @brief Find the best strategic move
     * @param board The board to analyze
     * @return The best move, or invalid move if none found
     */
    Move findBestStrategicMove(Board* board);
};

#endif // STRATEGYWORKER_H
