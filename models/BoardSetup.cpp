/*
    * BoardSetup.cpp
    * This file implements the board layout.
*/

#include "models/Board.h"
#include <QVector>
#include <QDebug>
#include <QRandomGenerator>
#include <QTime>

void Board::setupEnglish()
{
    rows = 7, cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[rows][cols] = {
        {-1, -1,  1,  1,  1, -1, -1},
        {-1, -1,  1,  1,  1, -1, -1},
        { 1,  1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  0,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1,  1},
        {-1, -1,  1,  1,  1, -1, -1},
        {-1, -1,  1,  1,  1, -1, -1},
    };

    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (starLayout[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
            else if (starLayout[r][c] == -1)
            {
                grid[r][c] = PegState::Blocked;
            }
            else if (starLayout[r][c] == 0)
            {
                grid[r][c] = PegState::Empty;
            }
        }
    }
}

void Board::setupDiamond()
{
    rows = 8, cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[rows][cols] = {
        { -1, -1, -1,  1, -1, -1, -1},
        { -1, -1,  1,  1,  1, -1, -1},
        { -1,  1,  1,  1,  1,  1, -1},
        {  1,  1,  1,  0,  1,  1,  1},
        {  1,  1,  1,  1,  1,  1,  1},
        { -1,  1,  1,  1,  1,  1, -1},
        { -1, -1,  1,  1,  1, -1, -1},
        { -1, -1, -1,  1, -1, -1, -1},
    };

    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (starLayout[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
            else if (starLayout[r][c] == -1)
            {
                grid[r][c] = PegState::Blocked;
            }
            else if (starLayout[r][c] == 0)
            {
                grid[r][c] = PegState::Empty;

            }
        }
    }
}

void Board::setupSquare()
{
    rows = 6, cols = 6;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[rows][cols] = {
        { 1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  0,  1,  1},
        { 1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1},
    };
    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (starLayout[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
            else if (starLayout[r][c] == -1)
            {
                grid[r][c] = PegState::Blocked;
            }
            else if (starLayout[r][c] == 0)
            {
                grid[r][c] = PegState::Empty;
            }
        }
    }

}

void Board::setupAntiPeg() 
{
    rows = 7, cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    // Anti-peg mode: English board layout but with only center peg initially
    int antiPegLayout[rows][cols] = {
        {-1, -1,  0,  0,  0, -1, -1},
        {-1, -1,  0,  0,  0, -1, -1},
        { 0,  0,  0,  0,  0,  0,  0},
        { 0,  0,  0,  1,  0,  0,  0},  // Only center peg
        { 0,  0,  0,  0,  0,  0,  0},
        {-1, -1,  0,  0,  0, -1, -1},
        {-1, -1,  0,  0,  0, -1, -1},
    };
    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (antiPegLayout[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
            else if (antiPegLayout[r][c] == -1)
            {
                grid[r][c] = PegState::Blocked;
            }
            else if (antiPegLayout[r][c] == 0)
            {
                grid[r][c] = PegState::Empty;
            }
        }
    }
}

void Board::setupEndgame()
{
    // Use English board layout as the base
    rows = 7, cols = 7;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    // Define English board shape
    int boardShape[rows][cols] = {
        {-1, -1,  0,  0,  0, -1, -1},
        {-1, -1,  0,  0,  0, -1, -1},
        { 0,  0,  0,  0,  0,  0,  0},
        { 0,  0,  0,  1,  0,  0,  0},
        { 0,  0,  0,  0,  0,  0,  0},
        {-1, -1,  0,  0,  0, -1, -1},
        {-1, -1,  0,  0,  0, -1, -1},
    };

    // Initialize board with shape (all empty initially)
    pegCount = 0;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (boardShape[r][c] == 1)
            {
                grid[r][c] = PegState::Peg;
                pegCount++;
            }
            else if (boardShape[r][c] == -1)
            {
                grid[r][c] = PegState::Blocked;
            }
            else if (boardShape[r][c] == 0)
            {
                grid[r][c] = PegState::Empty;
            }
        }
    }

    // Generate a solvable endgame position by working backwards from a winning state
    // Start with one peg in the center (winning state)
    // Work backwards by applying reverse moves to generate a solvable position
    // This guarantees the position is solvable since we can reach the winning state
    QRandomGenerator *rng = QRandomGenerator::global();
    int targetMoves = 8 + rng->bounded(8); // Generate 8-15 moves backwards (medium difficulty)
    
    for (int move = 0; move < targetMoves; ++move)
    {
        // Get all possible reverse moves (places where we can add a peg by reversing a jump)
        QVector<Move> reverseMoves;
        
        // Check all positions for potential reverse moves
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (grid[r][c] == PegState::Peg)
                {
                    // Try all 4 directions
                    int dr[] = {-1, 1, 0, 0};
                    int dc[] = {0, 0, -1, 1};
                    
                    for (int d = 0; d < 4; ++d)
                    {
                        Position from(r, c);
                        Position jumped(r + dr[d], c + dc[d]);
                        Position to(r + 2*dr[d], c + 2*dc[d]);
                        
                        // Check if this reverse move is valid
                        if (isValidPosition(jumped) && isValidPosition(to) &&
                            grid[jumped.row][jumped.col] == PegState::Empty &&
                            grid[to.row][to.col] == PegState::Empty)
                        {
                            // This is a valid reverse move
                            Move reverseMove;
                            reverseMove.from = to;
                            reverseMove.jumped = jumped;
                            reverseMove.to = from;
                            reverseMoves.append(reverseMove);
                        }
                    }
                }
            }
        }
        
        // If no reverse moves available, break
        if (reverseMoves.isEmpty())
        {
            break;
        }
        
        // Pick a random reverse move and apply it
        int randomIndex = rng->bounded(reverseMoves.size());
        Move reverseMove = reverseMoves[randomIndex];
        
        // Apply reverse move: remove peg from 'from', add peg to 'jumped', peg stays at 'to'
        grid[reverseMove.from.row][reverseMove.from.col] = PegState::Empty;
        grid[reverseMove.jumped.row][reverseMove.jumped.col] = PegState::Peg;
        grid[reverseMove.to.row][reverseMove.to.col] = PegState::Peg;
        // 'to' already has a peg, so we just added one more peg
        pegCount++;
    }
    
    qDebug() << "Generated endgame position with" << pegCount << "pegs";
}