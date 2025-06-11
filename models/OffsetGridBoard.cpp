#include "OffsetGridBoard.h"
#include <QVector>
#include <stdexcept> // For std::out_of_range
#include <cmath>     // For std::abs, std::round
#include <QDebug>

OffsetGridBoard::OffsetGridBoard() : numRows(0), numCols(0), pegCount(0)
{
    initializeBoard("triangular"); // Example default
}

OffsetGridBoard::OffsetGridBoard(const QString &boardType) : numRows(0), numCols(0), pegCount(0)
{
    initializeBoard(boardType);
}

void OffsetGridBoard::initializeBoard(const QString &boardType)
{
    currentBoardType = boardType;
    pegCount = 0;
    grid.clear();

    if (boardType == "triangular")
    {
        setupTriangular();
    }
    else if (boardType == "classic_star" || boardType == "star")
    {
        setupStar();
    }
    else
    {
        qWarning() << "Unknown offset grid board type:" << boardType;
        // Fallback or throw error
    }
}

void OffsetGridBoard::setupTriangular()
{
    numRows = 5;
    numCols = numRows; // Max columns needed for representation
    grid.resize(numRows);
    for (int i = 0; i < numRows; ++i)
    {
        grid[i].fill(PegState::Blocked, numCols); // Initialize all to Blocked
    }
    pegCount = 0;

    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c <= r; ++c)
        {
            grid[r][c] = PegState::Peg;
            pegCount++;
        }
    }

    if (numRows > 0 && numCols > 0 && grid[0][0] == PegState::Peg)
    {
        grid[0][0] = PegState::Empty;
        if (pegCount > 0)
            pegCount--; // Decrement if it was a peg
    }
}

void OffsetGridBoard::setupStar()
{
    numRows = 5;
    numCols = 5;
    grid.resize(numRows);
    for (int i = 0; i < numRows; ++i)
    {
        grid[i].fill(PegState::Blocked, numCols);
    }

    int starLayout[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}};
    pegCount = 0;
    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            if (starLayout[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
        }
    }

    if (numRows > 2 && numCols > 2 && grid[2][2] == PegState::Peg) // Center for 5x5
    {
        grid[2][2] = PegState::Empty;
        if (pegCount > 0)
            pegCount--;
    }
}

PegState OffsetGridBoard::getPegState(Position pos) const
{
    if (!isValidPosition(pos) || pos.row >= grid.size() || pos.col >= grid[pos.row].size())
    {
        return PegState::Blocked;
    }
    return grid[pos.row][pos.col];
}

void OffsetGridBoard::setPegState(Position pos, PegState state)
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
        qWarning() << "Attempted to set peg state for invalid offset position:" << pos.row << pos.col;
    }
}

bool OffsetGridBoard::isValidPosition(Position pos) const
{
    if (pos.row < 0 || pos.row >= numRows)
        return false;

    if (currentBoardType == "triangular")
    {
        // For triangular, col is valid from 0 up to row index
        return pos.col >= 0 && pos.col <= pos.row;
    }
    else if (currentBoardType == "star" || currentBoardType == "classic_star")
    {
        // For the star, it's within numCols bounds AND not initially Blocked
        // This check is a bit redundant if getPegState is used, but good for clarity.
        if (pos.col < 0 || pos.col >= numCols)
            return false;
        // The star shape is defined by non-Blocked cells in the initial setup.
        // A position is valid if it's within bounds and was part of the star shape.
        // This requires checking the initial layout or relying on getPegState != Blocked.
        // So, just check row/col bounds for the containing grid.
        return true; // Further check in getPegState will return Blocked if not part of star
    }
    // Default for other potential offset boards, check within rectangular bounds
    return pos.col >= 0 && pos.col < numCols;
}

QVector<Move> OffsetGridBoard::getValidMoves() const
{
    QVector<Move> moves;
    // Define 6 directions for hex-like/diagonal jumps
    // These are pairs of (jumped_offset, target_offset) from 'from'
    // { {dr_jumped, dc_jumped}, {dr_target, dc_target} }
    QVector<QPair<Position, Position>> directions;

    if (currentBoardType == "triangular")
    {
        // Moves for a triangular board (6 directions)
        // Horizontal
        directions.append(qMakePair(Position{0, -1}, Position{0, -2})); // Left
        directions.append(qMakePair(Position{0, 1}, Position{0, 2}));   // Right
        // Diagonal Up (relative to visual top of triangle)
        directions.append(qMakePair(Position{-1, -1}, Position{-2, -2})); // Up-Left
        directions.append(qMakePair(Position{-1, 0}, Position{-2, 0}));   // Up-Right
        // Diagonal Down
        directions.append(qMakePair(Position{1, 0}, Position{2, 0})); // Down-Left
        directions.append(qMakePair(Position{1, 1}, Position{2, 2})); // Down-Right
    }
    else if (currentBoardType == "star" || currentBoardType == "classic_star")
    {
        // Moves for a hex grid (star board is often on a hex grid)
        // These are standard 6 hex jump directions if rows are NOT offset in storage
        // If rows ARE offset (e.g. odd rows shifted), dr/dc needs adjustment or a mapping layer
        // Assuming a direct grid mapping for the 5x5 star for now, similar to GridBoard moves
        // but potentially allowing diagonal if the star structure implies it.
        // For the 5x5 star example, it behaves like a GridBoard with blocked cells.
        // So, cardinal jumps:
        directions.append(qMakePair(Position{-1, 0}, Position{-2, 0})); // Up
        directions.append(qMakePair(Position{1, 0}, Position{2, 0}));   // Down
        directions.append(qMakePair(Position{0, -1}, Position{0, -2})); // Left
        directions.append(qMakePair(Position{0, 1}, Position{0, 2}));   // Right
        // If diagonal jumps are allowed on this star representation:
        directions.append(qMakePair(Position{-1, -1}, Position{-2, -2})); // Up-Left
        directions.append(qMakePair(Position{-1, 1}, Position{-2, 2}));   // Up-Right
        directions.append(qMakePair(Position{1, -1}, Position{2, -2}));   // Down-Left
        directions.append(qMakePair(Position{1, 1}, Position{2, 2}));     // Down-Right
    }

    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c) // Iterate through the bounding box
        {
            Position from = {r, c};
            if (getPegState(from) == PegState::Peg)
            {
                for (const auto &dir_pair : directions)
                {
                    Position jumped = {r + dir_pair.first.row, c + dir_pair.first.col};
                    Position to = {r + dir_pair.second.row, c + dir_pair.second.col};

                    if (getPegState(jumped) == PegState::Peg && getPegState(to) == PegState::Empty)
                    {
                        // Ensure 'to' and 'jumped' are valid positions not just by coordinates but also part of the playable board
                        // getPegState handles this by returning Blocked for invalid/non-playable spots.
                        moves.append({from, jumped, to});
                    }
                }
            }
        }
    }
    return moves;
}

// Changed to const Move& to match Board.h and OffsetGridBoard.h
bool OffsetGridBoard::performMove(const Move &move)
{
    if (getPegState(move.from) == PegState::Peg &&
        getPegState(move.jumped) == PegState::Peg &&
        getPegState(move.to) == PegState::Empty)
    {
        // Validate that this is a legitimate jump path according to board rules
        // This re-checks the path, similar to getValidMoves logic for the specific move
        bool validJumpPath = false;
        QVector<QPair<Position, Position>> directions;
        if (currentBoardType == "triangular")
        {
            directions.append(qMakePair(Position{0, -1}, Position{0, -2}));
            directions.append(qMakePair(Position{0, 1}, Position{0, 2}));
            directions.append(qMakePair(Position{-1, -1}, Position{-2, -2}));
            directions.append(qMakePair(Position{-1, 0}, Position{-2, 0}));
            directions.append(qMakePair(Position{1, 0}, Position{2, 0}));
            directions.append(qMakePair(Position{1, 1}, Position{2, 2}));
        }
        else if (currentBoardType == "star" || currentBoardType == "classic_star")
        {
            directions.append(qMakePair(Position{-1, 0}, Position{-2, 0}));
            directions.append(qMakePair(Position{1, 0}, Position{2, 0}));
            directions.append(qMakePair(Position{0, -1}, Position{0, -2}));
            directions.append(qMakePair(Position{0, 1}, Position{0, 2}));
            directions.append(qMakePair(Position{-1, -1}, Position{-2, -2}));
            directions.append(qMakePair(Position{-1, 1}, Position{-2, 2}));
            directions.append(qMakePair(Position{1, -1}, Position{2, -2}));
            directions.append(qMakePair(Position{1, 1}, Position{2, 2}));
        }

        for (const auto &dir_pair : directions)
        {
            if (move.jumped.row == move.from.row + dir_pair.first.row &&
                move.jumped.col == move.from.col + dir_pair.first.col &&
                move.to.row == move.from.row + dir_pair.second.row &&
                move.to.col == move.from.col + dir_pair.second.col)
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
            return true;
        }
    }
    return false;
}

int OffsetGridBoard::getRows() const
{
    return numRows;
}

int OffsetGridBoard::getCols() const
{
    // This might represent max columns in the bounding box
    return numCols;
}

int OffsetGridBoard::getPegCount() const
{
    return pegCount;
}

bool OffsetGridBoard::isGameOver() const
{
    return getValidMoves().isEmpty();
}

QString OffsetGridBoard::getBoardType() const
{
    return currentBoardType;
}
