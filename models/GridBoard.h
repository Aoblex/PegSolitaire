#ifndef GRIDBOARD_H
#define GRIDBOARD_H

#include "models/Board.h"
#include <QVector>
#include <QString>

class GridBoard : public Board
{
    Q_OBJECT
public:
    explicit GridBoard(QObject *parent = nullptr);
    explicit GridBoard(BoardType boardType, QObject *parent = nullptr); // Changed QString to BoardType
    // Constructor that takes a specific board type (e.g., "english_standard", "european_standard")

    void initializeBoard(BoardType boardType) override; // Changed QString to BoardType
    PegState getPegState(Position pos) const override;
    void setPegState(Position pos, PegState state); // Not overriding from Board
    bool isValidPosition(Position pos) const;       // Not overriding from Board
    QVector<Move> getValidMoves() const override;
    bool performMove(const Move &move) override; // Changed to const Move&
    int getRows() const override;
    int getCols() const override;
    int getPegCount() const override;
    bool isGameOver() const override;
    BoardType getBoardType() const override; // Changed QString to BoardType

private:
    void setupEnglishStandard();
    void setupEuropeanStandard();
    void setupCross();
    // Add other grid-based board setups (e.g., square, cross)

    QVector<QVector<PegState>> grid;
    int rows;
    int cols;
    int pegCount;
    // QString currentBoardType; // Removed, uses protected Board::currentBoardType
};

#endif // GRIDBOARD_H
