/*
    * BoardSetup.cpp
    * This file implements the board layout.
*/

#include "models/Board.h"
#include <QVector>
#include <QDebug>

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