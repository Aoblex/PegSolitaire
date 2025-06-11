#ifndef GRIDBOARD_H
#define GRIDBOARD_H

#include "Board.h"
#include <QVector> // Ensure QVector is included

class GridBoard : public Board
{
public:
    GridBoard();
    // Constructor that takes a specific board type (e.g., "english_standard", "european_standard")
    GridBoard(const QString &boardType);

    void initializeBoard(const QString &boardType) override;
    PegState getPegState(Position pos) const override;
    void setPegState(Position pos, PegState state); // Not overriding from Board
    bool isValidPosition(Position pos) const;       // Not overriding from Board
    QVector<Move> getValidMoves() const override;
    bool performMove(const Move &move) override; // Changed to const Move&
    int getRows() const override;
    int getCols() const override;
    int getPegCount() const override;
    bool isGameOver() const override;
    QString getBoardType() const override;

private:
    void setupEnglishStandard();
    void setupEuropeanStandard();
    // Add other grid-based board setups (e.g., square, cross)

    QVector<QVector<PegState>> grid;
    int rows;
    int cols;
    int pegCount;
    QString currentBoardType;
};

#endif // GRIDBOARD_H
