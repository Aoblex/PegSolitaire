#include "GridBoard.h"
#include <QVector>
#include <stdexcept> // For std::out_of_range
#include <QDebug>

GridBoard::GridBoard() : rows(0), cols(0), pegCount(0)
{
    // Default constructor, perhaps initialize with a default board type or leave empty
    // initializeBoard("english_standard"); // Example
}

GridBoard::GridBoard(const QString &boardType) : rows(0), cols(0), pegCount(0)
{
    initializeBoard(boardType);
}

void GridBoard::initializeBoard(const QString &boardType)
{
    currentBoardType = boardType;
    pegCount = 0;
    grid.clear();

    if (boardType == "classic_english" || boardType == "english_standard")
    {
        setupEnglishStandard();
    }
    else if (boardType == "classic_european" || boardType == "european_standard")
    {
        setupEuropeanStandard();
    }
    else if (boardType == "classic_cross")
    {
        rows = 7;
        cols = 7;
        grid.resize(rows);
        for (int i = 0; i < rows; ++i)
        {
            grid[i].fill(PegState::Blocked, cols);
        }

        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (!((r < 2 || r > 4) && (c < 2 || c > 4)))
                {
                    grid[r][c] = PegState::Peg;
                    pegCount++;
                }
            }
        }
        if (rows > 0 && cols > 0 && rows / 2 < grid.size() && cols / 2 < grid[rows / 2].size())
            grid[rows / 2][cols / 2] = PegState::Empty;
        if (pegCount > 0)
            pegCount--; // Account for the empty center
    }
    else if (boardType == "classic_star")
    {
        qWarning() << "Star board is complex for GridBoard, consider OffsetGridBoard.";
        setupEnglishStandard(); // Fallback
    }
    else
    {
        qWarning() << "Unknown grid board type:" << boardType;
        setupEnglishStandard();
    }
}

void GridBoard::setupEnglishStandard()
{
    rows = 7;
    cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    // Define the cross shape for English Standard
    int validCells[33][2] = {
        {0, 2}, {0, 3}, {0, 4}, {1, 2}, {1, 3}, {1, 4}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 0}, {3, 1}, {3, 2}, {3, 4}, {3, 5}, {3, 6}, // {3,3} is empty
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {4, 6},
        {5, 2},
        {5, 3},
        {5, 4},
        {6, 2},
        {6, 3},
        {6, 4}};

    pegCount = 0;
    for (const auto &cell : validCells)
    {
        if (isValidPosition({cell[0], cell[1]}))
        {
            grid[cell[0]][cell[1]] = PegState::Peg;
            pegCount++;
        }
    }

    if (isValidPosition({3, 3}))
    {
        grid[3][3] = PegState::Empty;
        // pegCount is already correct as it counts pegs, and {3,3} was not added as a peg.
    }
    else
    {
        qWarning() << "Center position {3,3} is invalid for English Standard setup.";
    }
}

void GridBoard::setupEuropeanStandard()
{
    rows = 7;
    cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Peg, cols);
    }

    // Block corners for European style
    int blockedCoords[12][2] = {
        {0, 0}, {0, 1}, {0, 5}, {0, 6}, {1, 0}, {1, 6}, {5, 0}, {5, 6}, {6, 0}, {6, 1}, {6, 5}, {6, 6}};

    for (const auto &coord : blockedCoords)
    {
        if (isValidPosition({coord[0], coord[1]}))
        {
            grid[coord[0]][coord[1]] = PegState::Blocked;
        }
    }

    if (isValidPosition({3, 3}))
    {
        grid[3][3] = PegState::Empty;
    }

    // Recalculate pegCount accurately
    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (grid[r][c] == PegState::Peg)
            {
                pegCount++;
            }
        }
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
            return true;
        }
    }
    return false;
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

QString GridBoard::getBoardType() const
{
    return currentBoardType;
}
