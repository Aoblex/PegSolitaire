/*
    * BoardSetup.cpp
    * This file implements the board layout.
*/

#include "models/Board.h"
#include <QVector>
#include <QDebug>

void Board::setupEnglishStandard()
{
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
                // Set starting position to the center empty cell
                startingPosition = Position(r, c);
            }
        }
    }
}

void Board::setupEuropeanStandard()
{
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[rows][cols] = {
        {-1, -1,  1,  1,  1, -1, -1},
        {-1,  1,  1,  1,  1,  1, -1},
        { 1,  1,  1,  0,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1,  1},
        {-1,  1,  1,  1,  1,  1, -1},
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
                // Set starting position to the empty cell
                startingPosition = Position(r, c);
            }
        }
    }
}

void Board::setupCross() {
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[rows][cols] = {
        {-1, -1,  1,  1,  1, -1, -1},
        {-1,  1,  1,  0,  1,  1, -1},
        { 1,  1,  1,  0,  1,  1,  1},
        { 1,  0,  0,  0,  0,  0,  1},
        { 1,  1,  1,  0,  1,  1,  1},
        {-1,  1,  1,  0,  1,  1, -1},
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
                // For Cross layout, set starting position to the center empty cell (3,3)
                if (r == 3 && c == 3) {
                    startingPosition = Position(r, c);
                }
            }
        }
    }

}

void Board::setupAntiPeg() 
{
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
                // Set starting position to the center peg
                startingPosition = Position(r, c);
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