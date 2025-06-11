#ifndef BOARD_H
#define BOARD_H

#include <QString>
#include <QVector> // Added for QVector
#include <QPair>   // Added for QPair (used in Position)
#include <QObject> // Added for QObject inheritance

// Enum to represent the type of board
enum class BoardType
{
    ClassicEnglish,
    ClassicEuropean,
    ClassicCross,
    ClassicStar, // For GridBoard fallback or specific GridBoard star
    Triangular,  // For OffsetGridBoard
    StarShape    // For OffsetGridBoard (distinct from ClassicStar if needed)
    // Add other board types here
};

// Enum to represent the state of a cell on the board
enum class PegState
{
    Empty,
    Peg,
    Blocked // For cells that are not part of the playable board
};

// Struct to represent a position on the board
struct Position
{
    int row;
    int col;

    // Operator for comparing positions, useful for std::map or std::set
    bool operator<(const Position &other) const
    {
        if (row != other.row)
        {
            return row < other.row;
        }
        return col < other.col;
    }

    // Add a comparison operator for use in QVector, QSet, etc.
    bool operator==(const Position &other) const
    {
        return row == other.row && col == other.col;
    }
};

// Struct to represent a move
struct Move
{
    Position from;
    Position jumped;
    Position to;
};

class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = nullptr);
    virtual ~Board() = default;

    // Pure virtual functions to be implemented by derived classes
    virtual void initializeBoard(BoardType boardType) = 0; // Changed from QString
    virtual QVector<Move> getValidMoves() const = 0; // Changed from std::vector to QVector
    virtual bool performMove(const Move &move) = 0;
    virtual PegState getPegState(Position pos) const = 0;
    virtual int getRows() const = 0;
    virtual int getCols() const = 0; // May not be applicable for all board types, but useful for grid-based ones
    virtual int getPegCount() const = 0;
    virtual bool isGameOver() const = 0;
    virtual BoardType getBoardType() const = 0; // Changed from QString
    // Add any other common board operations here

protected: // Added protected to allow derived classes to access currentBoardType
    BoardType currentBoardType; // Added
};

#endif // BOARD_H
