#ifndef OFFSETGRIDBOARD_H
#define OFFSETGRIDBOARD_H

#include "Board.h"
#include <QVector> // Ensure QVector is included

class OffsetGridBoard : public Board
{
public:
    OffsetGridBoard();
    // Constructor that takes a specific board type (e.g., "triangular", "star")
    OffsetGridBoard(const QString &boardType);

    void initializeBoard(const QString &boardType) override;
    PegState getPegState(Position pos) const override;
    void setPegState(Position pos, PegState state); // Not overriding from Board
    bool isValidPosition(Position pos) const;       // Not overriding from Board
    QVector<Move> getValidMoves() const override;
    bool performMove(const Move &move) override; // Changed to const Move&
    int getRows() const override;                // May need re-evaluation for non-rectangular shapes
    int getCols() const override;                // May need re-evaluation for non-rectangular shapes
    int getPegCount() const override;
    bool isGameOver() const override;
    QString getBoardType() const override;

private:
    void setupTriangular();
    void setupStar();
    // Add other offset grid-based board setups

    QVector<QVector<PegState>> grid;
    int numRows; // Effective number of rows
    int numCols; // Effective number of columns or max width
    int pegCount;
    QString currentBoardType;

    // Helper to convert logical row/col to storage indices if needed,
    // or to handle the offset logic directly in move generation.
};

#endif // OFFSETGRIDBOARD_H
