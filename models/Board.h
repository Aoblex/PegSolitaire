#ifndef BOARD_H
#define BOARD_H

#include <QString>
#include <QVector> // Added for QVector
#include <QPair>   // Added for QPair (used in Position)
#include <QObject> // Added for QObject inheritance

// Enum to represent the type of board
enum class BoardType
{
    English,
    European,
    Cross,
    AntiPeg,  // Anti-peg mode: start with one peg, fill the board
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
    
    // Constructor
    Position(int r = 0, int c = 0) : row(r), col(c) {}

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
    explicit Board(BoardType boardType, QObject *parent = nullptr);
    virtual ~Board() = default;

    // Board functions - no longer pure virtual
    void initializeBoard(BoardType boardType);
    QVector<Move> getValidMoves() const;
    bool performMove(const Move &move);
    bool undoLastMove();
    PegState getPegState(Position pos) const;
    void setPegState(Position pos, PegState state);
    bool isValidPosition(Position pos) const;
    int getRows() const;
    int getCols() const;
    int getPegCount() const;
    bool isGameOver() const;
    BoardType getBoardType() const;
    
    /**
     * @brief Check if this board is in anti-peg mode
     * @return True if the board is in anti-peg mode
     */
    bool isAntiPegMode() const;

private:
    // Board setup methods
    void setupEnglishStandard();
    void setupEuropeanStandard();
    void setupCross();
    void setupAntiPeg();  // Anti-peg mode setup

    // Board data
    QVector<QVector<PegState>> grid;
    int rows;
    int cols;
    int pegCount;

protected:
    BoardType currentBoardType;
    QVector<Move> moveHistory; // Track move history for undo
};

#endif // BOARD_H
