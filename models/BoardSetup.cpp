/*
    * GridBoard.cpp
    * This file implements the board layout.
*/

#include "models/GridBoard.h"
#include "models/OffsetGridBoard.h"
#include <QVector>
#include <QDebug>

void GridBoard::setupEnglishStandard()
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

void GridBoard::setupCross() {
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

void OffsetGridBoard::setupTriangular()
{
    rows = 5;
    cols = rows; // Max columns needed for representation
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols); // Initialize all to Blocked
    }
    pegCount = 0;

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c <= r; ++c)
        {
            grid[r][c] = PegState::Peg;
            pegCount++;
        }
    }

    if (rows > 0 && cols > 0 && grid[0][0] == PegState::Peg)
    {
        grid[0][0] = PegState::Empty;
        if (pegCount > 0)
            pegCount--; // Decrement if it was a peg
    }
}

void OffsetGridBoard::setupStar()
{
    rows = 5;
    cols = 5;
    grid.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(PegState::Blocked, cols);
    }

    int starLayout[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}};
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
        }
    }

    if (rows > 2 && cols > 2 && grid[2][2] == PegState::Peg) // Center for 5x5
    {
        grid[2][2] = PegState::Empty;
        if (pegCount > 0)
            pegCount--;
    }
}