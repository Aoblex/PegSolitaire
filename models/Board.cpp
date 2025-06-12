#include "models/Board.h"
#include <QVector>
#include <stdexcept>
#include <QDebug>

Board::Board(QObject *parent) : QObject(parent), rows(0), cols(0), pegCount(0)
{
    // Default constructor, initialize with a default board type
    initializeBoard(BoardType::English);
}

Board::Board(BoardType boardType, QObject *parent) : QObject(parent), rows(0), cols(0), pegCount(0)
{
    initializeBoard(boardType);
}

void Board::initializeBoard(BoardType boardType)
{
    this->currentBoardType = boardType;
    pegCount = 0;
    grid.clear();
    moveHistory.clear();

    switch (boardType)
    {
        case BoardType::English:
            setupEnglishStandard();
            break;
        case BoardType::European:
            setupEuropeanStandard();
            break;
        case BoardType::Cross:
            setupCross();
            break;
        case BoardType::AntiPeg:
            setupAntiPeg();
            break;
        default:
            qWarning() << "Unknown or unsupported board type:" << static_cast<int>(boardType);
            setupEnglishStandard(); // Default fallback
            break;
    }
}

PegState Board::getPegState(Position pos) const
{
    if (!isValidPosition(pos) || pos.row >= grid.size() || pos.col >= grid[pos.row].size())
    {
        return PegState::Blocked;
    }
    return grid[pos.row][pos.col];
}

void Board::setPegState(Position pos, PegState state)
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

bool Board::isValidPosition(Position pos) const
{
    if (pos.row < 0 || pos.row >= rows)
        return false;
    return pos.col >= 0 && pos.col < cols;
}

QVector<Move> Board::getValidMoves() const
{
    QVector<Move> moves;
    // Standard 4-direction moves for rectangular boards
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    if (isAntiPegMode()) {
        // Anti-peg mode: peg jumps over empty cell to another empty cell, 
        // leaving a peg in the jumped-over cell
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (getPegState({r, c}) == PegState::Peg)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        Position jumped(r + dr[i], c + dc[i]);
                        Position to(r + 2 * dr[i], c + 2 * dc[i]);

                        // In anti-peg mode: jump over empty to empty, placing peg in jumped position
                        if (getPegState(jumped) == PegState::Empty && getPegState(to) == PegState::Empty)
                        {
                            moves.append({{r, c}, jumped, to});
                        }
                    }
                }
            }
        }
    } else {
        // Normal mode: peg jumps over peg to empty cell, removing jumped peg
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (getPegState({r, c}) == PegState::Peg)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        Position jumped(r + dr[i], c + dc[i]);
                        Position to(r + 2 * dr[i], c + 2 * dc[i]);

                        if (getPegState(jumped) == PegState::Peg && getPegState(to) == PegState::Empty)
                        {
                            moves.append({{r, c}, jumped, to});
                        }
                    }
                }
            }
        }
    }
    return moves;
}

bool Board::performMove(const Move &move)
{
    if (isAntiPegMode()) {
        // Anti-peg mode validation: peg jumps over empty to empty, placing peg in jumped cell
        if (getPegState(move.from) == PegState::Peg &&
            getPegState(move.jumped) == PegState::Empty &&
            getPegState(move.to) == PegState::Empty)
        {
            bool validJumpPath = false;
            // Standard rectangular jump validation
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
                // Store move in history for undo
                moveHistory.append(move);
                
                // Perform anti-peg move: move peg to destination, place peg in jumped cell
                setPegState(move.from, PegState::Empty);
                setPegState(move.jumped, PegState::Peg);    // Place peg in jumped cell
                setPegState(move.to, PegState::Peg);
                return true;
            }
        }
    } else {
        // Normal mode validation: peg jumps over peg to empty, removing jumped peg
        if (getPegState(move.from) == PegState::Peg &&
            getPegState(move.jumped) == PegState::Peg &&
            getPegState(move.to) == PegState::Empty)
        {
            bool validJumpPath = false;
            // Standard rectangular jump validation
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
                // Store move in history for undo
                moveHistory.append(move);
                
                // Perform normal move: move peg to destination, remove jumped peg
                setPegState(move.from, PegState::Empty);
                setPegState(move.jumped, PegState::Empty);  // Remove jumped peg
                setPegState(move.to, PegState::Peg);
                return true;
            }
        }
    }
    return false;
}

bool Board::undoLastMove()
{
    if (moveHistory.isEmpty()) {
        return false;
    }
    
    // Get the last move from history
    Move lastMove = moveHistory.takeLast();
    
    if (isAntiPegMode()) {
        // Reverse anti-peg move: remove peg from destination and jumped cell, restore original peg
        setPegState(lastMove.to, PegState::Empty);       // Remove peg from destination
        setPegState(lastMove.jumped, PegState::Empty);   // Remove peg from jumped cell
        setPegState(lastMove.from, PegState::Peg);       // Restore original peg
    } else {
        // Reverse normal move: remove peg from destination, restore jumped peg, restore original peg
        setPegState(lastMove.to, PegState::Empty);       // Remove peg from destination
        setPegState(lastMove.jumped, PegState::Peg);     // Restore jumped peg
        setPegState(lastMove.from, PegState::Peg);       // Restore original peg
    }
    
    return true;
}

int Board::getRows() const
{
    return rows;
}

int Board::getCols() const
{
    return cols;
}

int Board::getPegCount() const
{
    return pegCount;
}

bool Board::isGameOver() const
{
    return getValidMoves().isEmpty();
}

BoardType Board::getBoardType() const
{
    return currentBoardType;
}

bool Board::isAntiPegMode() const
{
    return currentBoardType == BoardType::AntiPeg;
}