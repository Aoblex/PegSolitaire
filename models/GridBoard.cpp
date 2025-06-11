#include "models/GridBoard.h"
#include <QVector>
#include <stdexcept> // For std::out_of_range
#include <QDebug>

GridBoard::GridBoard(QObject *parent) : Board(parent), rows(0), cols(0), pegCount(0)
{
    // Default constructor, initialize with a default board type
    initializeBoard(BoardType::ClassicEnglish); // Example default
}

GridBoard::GridBoard(BoardType boardType, QObject *parent) : Board(parent), rows(0), cols(0), pegCount(0)
{
    initializeBoard(boardType);
}

void GridBoard::initializeBoard(BoardType boardType)
{
    this->currentBoardType = boardType; // Use the inherited member
    pegCount = 0;
    grid.clear();

    switch (boardType)
    {
        case BoardType::ClassicEnglish:
            setupEnglishStandard();
            break;
        case BoardType::ClassicEuropean:
            setupEuropeanStandard();
            break;
        case BoardType::ClassicCross:
            setupCross();
            break;
        case BoardType::ClassicStar: // Fallback for GridBoard if it handles a simple star
            qWarning() << "ClassicStar is typically handled by OffsetGridBoard, using English Standard fallback for GridBoard.";
            setupEnglishStandard(); // Fallback
            break;
        default:
            qWarning() << "Unknown or unsupported grid board type:" << static_cast<int>(boardType);
            setupEnglishStandard(); // Default fallback
            break;
    }
}

PegState GridBoard::getPegState(Position pos) const
{
    if (!isValidPosition(pos) || pos.row >= grid.size() || pos.col >= grid[pos.row].size())
    {
        return PegState::Blocked;
    }
    return grid[pos.row][pos.col];
}

void GridBoard::setPegState(Position pos, PegState state)
{
    if (isValidPosition(pos) && pos.row < grid.size() && pos.col < grid[pos.row].size())
    {
        PegState oldState = grid[pos.row][pos.col];
        grid[pos.row][pos.col] = state;

        if (oldState == PegState::Peg && state != PegState::Peg)
        {
            pegCount--;
        }
        else if (oldState != PegState::Peg && state == PegState::Peg)
        {
            pegCount++;
        }
    }
    else
    {
        qWarning() << "Attempted to set peg state for invalid position:" << pos.row << pos.col;
    }
}

bool GridBoard::isValidPosition(Position pos) const
{
    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols;
}

QVector<Move> GridBoard::getValidMoves() const
{
    QVector<Move> moves;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (getPegState({r, c}) == PegState::Peg) // Use getPegState for safety
            {
                for (int i = 0; i < 4; ++i)
                {
                    Position current = {r, c};
                    Position jumped = {r + dr[i], c + dc[i]};
                    Position target = {r + 2 * dr[i], c + 2 * dc[i]};

                    if (isValidPosition(jumped) && isValidPosition(target) &&
                        getPegState(jumped) == PegState::Peg &&
                        getPegState(target) == PegState::Empty)
                    {
                        moves.append({current, jumped, target}); // Use append for QVector
                    }
                }
            }
        }
    }
    return moves;
}

// Changed to const Move& to match Board.h and GridBoard.h
bool GridBoard::performMove(const Move &move)
{
    // Validate positions first using getPegState to handle out-of-bounds gracefully
    if (getPegState(move.from) == PegState::Peg &&
        getPegState(move.jumped) == PegState::Peg &&
        getPegState(move.to) == PegState::Empty)
    {
        bool validJumpPath = false;
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; ++i)
        {
            if (move.jumped.row == move.from.row + dr[i] &&
                move.jumped.col == move.from.col + dc[i] &&
                move.to.row == move.from.row + 2 * dr[i] &&
                move.to.col == move.from.col + 2 * dc[i])
            {
                validJumpPath = true;
                break;
            }
        }

        if (validJumpPath)
        {
            setPegState(move.from, PegState::Empty);
            setPegState(move.jumped, PegState::Empty);
            setPegState(move.to, PegState::Peg);
            moveHistory.append(move); // Save the move to history
            return true;
        }
    }
    return false;
}

bool GridBoard::undoLastMove()
{
    if (moveHistory.isEmpty()) {
        return false; // No moves to undo
    }
    
    // Get the last move from history
    Move lastMove = moveHistory.takeLast();
    
    // Reverse the move
    setPegState(lastMove.to, PegState::Empty);     // Remove peg from destination
    setPegState(lastMove.jumped, PegState::Peg);   // Restore jumped peg
    setPegState(lastMove.from, PegState::Peg);     // Restore original peg
    
    return true;
}

int GridBoard::getRows() const
{
    return rows;
}

int GridBoard::getCols() const
{
    return cols;
}

int GridBoard::getPegCount() const
{
    return pegCount;
}

// Basic game over condition: no more valid moves
bool GridBoard::isGameOver() const
{
    return getValidMoves().isEmpty();
}

BoardType GridBoard::getBoardType() const // Changed QString to BoardType
{
    return currentBoardType;
}
