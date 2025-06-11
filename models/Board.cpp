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
        case BoardType::Star:
            setupStar();
            break;
        case BoardType::Triangular:
            setupTriangular();
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

    if (currentBoardType == BoardType::Triangular)
    {
        // For triangular, col is valid from 0 up to row index
        return pos.col >= 0 && pos.col <= pos.row;
    }
    else
    {
        // For rectangular boards (including star)
        return pos.col >= 0 && pos.col < cols;
    }
}

QVector<Move> Board::getValidMoves() const
{
    QVector<Move> moves;

    if (currentBoardType == BoardType::Triangular
     || currentBoardType == BoardType::Star)
    {
        // Define 6 directions for triangular board
        QVector<QPair<Position, Position>> directions;
        
        // Horizontal
        directions.append(qMakePair(Position{0, -1}, Position{0, -2}));
        directions.append(qMakePair(Position{0, 1}, Position{0, 2}));
        // Diagonal Up
        directions.append(qMakePair(Position{-1, -1}, Position{-2, -2}));
        directions.append(qMakePair(Position{-1, 0}, Position{-2, 0}));
        // Diagonal Down
        directions.append(qMakePair(Position{1, 0}, Position{2, 0}));
        directions.append(qMakePair(Position{1, 1}, Position{2, 2}));

        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                Position from(r, c);
                if (getPegState(from) == PegState::Peg)
                {
                    for (const auto &dir : directions)
                    {
                        Position jumped(from.row + dir.first.row, from.col + dir.first.col);
                        Position to(from.row + dir.second.row, from.col + dir.second.col);

                        if (getPegState(jumped) == PegState::Peg && getPegState(to) == PegState::Empty)
                        {
                            moves.append({from, jumped, to});
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Standard 4-direction moves for rectangular boards
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

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
    // Validate move
    if (getPegState(move.from) == PegState::Peg &&
        getPegState(move.jumped) == PegState::Peg &&
        getPegState(move.to) == PegState::Empty)
    {
        bool validJumpPath = false;

        if (currentBoardType == BoardType::Triangular)
        {
            // Check if it's a valid triangular jump
            int dr = move.to.row - move.from.row;
            int dc = move.to.col - move.from.col;
            int jr = move.jumped.row - move.from.row;
            int jc = move.jumped.col - move.from.col;

            // Valid triangular jumps
            if ((dr == 0 && abs(dc) == 2 && jc == dc/2) ||  // Horizontal
                (dr == -2 && dc == -2 && jr == -1 && jc == -1) || // Diagonal up-left
                (dr == -2 && dc == 0 && jr == -1 && jc == 0) ||   // Diagonal up-right
                (dr == 2 && dc == 0 && jr == 1 && jc == 0) ||     // Diagonal down-left
                (dr == 2 && dc == 2 && jr == 1 && jc == 1))       // Diagonal down-right
            {
                validJumpPath = true;
            }
        }
        else
        {
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
        }

        if (validJumpPath)
        {
            // Store move in history for undo
            moveHistory.append(move);
            
            // Perform the move
            setPegState(move.from, PegState::Empty);
            setPegState(move.jumped, PegState::Empty);
            setPegState(move.to, PegState::Peg);
            return true;
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
    
    // Reverse the move
    setPegState(lastMove.to, PegState::Empty);     // Remove peg from destination
    setPegState(lastMove.jumped, PegState::Peg);   // Restore jumped peg
    setPegState(lastMove.from, PegState::Peg);     // Restore original peg
    
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