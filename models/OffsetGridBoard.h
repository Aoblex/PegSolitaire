#ifndef OFFSETGRIDBOARD_H
#define OFFSETGRIDBOARD_H

#include "models/Board.h"
#include <QVector>
#include <QString>

class OffsetGridBoard : public Board
{
    Q_OBJECT
public:
    explicit OffsetGridBoard(QObject *parent = nullptr);
    explicit OffsetGridBoard(BoardType boardType, QObject *parent = nullptr); // Changed QString to BoardType

    void initializeBoard(BoardType boardType) override; // Changed QString to BoardType
    PegState getPegState(Position pos) const override;
    void setPegState(Position pos, PegState state); // Not overriding from Board
    bool isValidPosition(Position pos) const;       // Not overriding from Board
    QVector<Move> getValidMoves() const override;
    bool performMove(const Move &move) override; // Changed to const Move&
    bool undoLastMove() override; // Implementation for undo functionality
    int getRows() const override;                // May need re-evaluation for non-rectangular shapes
    int getCols() const override;                // May need re-evaluation for non-rectangular shapes
    int getPegCount() const override;
    bool isGameOver() const override;
    BoardType getBoardType() const override; // Changed QString to BoardType

private:
    void setupTriangular();
    void setupStar();
    // Add other offset grid-based board setups

    QVector<QVector<PegState>> grid;
    int rows; // Effective number of rows
    int cols; // Effective number of columns or max width
    int pegCount;
    // QString currentBoardType; // Removed, uses protected Board::currentBoardType

    // Helper to convert logical row/col to storage indices if needed,
    // or to handle the offset logic directly in move generation.
};

#endif // OFFSETGRIDBOARD_H
