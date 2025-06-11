#include "BoardView.h"
#include "../../models/GridBoard.h"       // For dynamic_cast and specific drawing
#include "../../models/OffsetGridBoard.h" // For dynamic_cast and specific drawing
#include <QMouseEvent>
#include <QDebug>
#include <cmath> // For std::sqrt, std::pow

BoardView::BoardView(QWidget *parent)
    : QGraphicsView(parent),
      currentBoard(nullptr),
      pegRadius(15.0),  // Default peg radius
      cellSpacing(40.0) // Default spacing between cell centers
{
    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    // Ensure the view can receive mouse press events even without items at a location
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void BoardView::setBoard(Board *board)
{
    currentBoard = board;
    if (currentBoard)
    {
        updateView();
    }
}

void BoardView::updateView()
{
    if (!currentBoard)
        return;

    scene->clear(); // Clear previous items

    // Determine board type for specific drawing logic if needed
    if (dynamic_cast<GridBoard *>(currentBoard))
    {
        drawGridBoard();
    }
    else if (dynamic_cast<OffsetGridBoard *>(currentBoard))
    {
        drawOffsetGridBoard();
    }
    else
    {
        // Generic drawing or error
        qWarning() << "BoardView: Unknown board type for drawing.";
        return;
    }

    // Adjust scene rect to fit the board
    // This might need to be more dynamic based on board dimensions
    // For now, a simple heuristic.
    if (currentBoard->getRows() > 0 && currentBoard->getCols() > 0)
    {
        scene->setSceneRect(0, 0,
                            currentBoard->getCols() * cellSpacing + cellSpacing,
                            currentBoard->getRows() * cellSpacing + cellSpacing);
    }
    centerOn(scene->sceneRect().center());
}

void BoardView::drawGridBoard()
{
    GridBoard *board = static_cast<GridBoard *>(currentBoard);
    int rows = board->getRows();
    int cols = board->getCols();

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            Position pos = {r, c};
            PegState state = board->getPegState(pos);
            QPointF scenePos = getScenePosition(pos);

            if (state != PegState::Blocked)
            {
                // Draw cell background (e.g., an empty hole placeholder)
                QGraphicsRectItem *bgCell = scene->addRect(scenePos.x() - pegRadius, scenePos.y() - pegRadius,
                                                           2 * pegRadius, 2 * pegRadius,
                                                           QPen(Qt::gray), QBrush(Qt::lightGray));
                bgCell->setData(0, QVariant::fromValue(pos)); // Store board position

                if (state == PegState::Peg)
                {
                    QGraphicsEllipseItem *pegItem = scene->addEllipse(scenePos.x() - pegRadius, scenePos.y() - pegRadius,
                                                                      2 * pegRadius, 2 * pegRadius,
                                                                      QPen(Qt::black), QBrush(Qt::blue));
                    pegItem->setData(0, QVariant::fromValue(pos)); // Store board position
                }
            }
        }
    }
}

void BoardView::drawOffsetGridBoard()
{
    OffsetGridBoard *board = static_cast<OffsetGridBoard *>(currentBoard);
    int totalRows = board->getRows();
    int totalCols = board->getCols(); // Max cols in the bounding box

    for (int r = 0; r < totalRows; ++r)
    {
        // For an offset grid, the number of columns per row might vary, or the horizontal position shifts.
        // The `getCols()` in OffsetGridBoard might return the max width of the bounding box.
        // We need to iterate through positions that are actually part of the board.
        for (int c = 0; c < totalCols; ++c)
        { // Iterate up to totalCols
            Position pos = {r, c};
            if (!board->isValidPosition(pos) || board->getPegState(pos) == PegState::Blocked)
            {
                continue; // Skip positions not on the board or explicitly blocked
            }

            PegState state = board->getPegState(pos);
            QPointF scenePos = getScenePosition(pos); // This needs to handle the offset

            // Draw cell background
            QGraphicsRectItem *bgCell = scene->addRect(scenePos.x() - pegRadius, scenePos.y() - pegRadius,
                                                       2 * pegRadius, 2 * pegRadius,
                                                       QPen(Qt::gray), QBrush(Qt::lightGray));
            bgCell->setData(0, QVariant::fromValue(pos));

            if (state == PegState::Peg)
            {
                QGraphicsEllipseItem *pegItem = scene->addEllipse(scenePos.x() - pegRadius, scenePos.y() - pegRadius,
                                                                  2 * pegRadius, 2 * pegRadius,
                                                                  QPen(Qt::black), QBrush(Qt::green)); // Different color for offset board pegs
                pegItem->setData(0, QVariant::fromValue(pos));
            }
        }
    }
}

QPointF BoardView::getScenePosition(Position boardPos) const
{
    qreal x = (boardPos.col + 0.5) * cellSpacing;
    qreal y = (boardPos.row + 0.5) * cellSpacing;

    if (currentBoard && dynamic_cast<OffsetGridBoard *>(currentBoard))
    {
        // Apply horizontal offset for odd/even rows if it's a hex-like grid
        // This is a common way to represent hex grids: shift every other row
        if (boardPos.row % 2 != 0)
        {
            x += cellSpacing / 2.0; // Shift odd rows
        }
        // For a triangular board, the `col` index is direct if `cellSpacing` accounts for diagonal packing.
        // If `cellSpacing` is for a square grid, then y might also need adjustment for triangular packing.
        // For simplicity, current `cellSpacing` is for center-to-center, assuming a grid overlay.
        // If true isometric/hexagonal projection is needed, this function becomes more complex.
    }
    return QPointF(x, y);
}

Position BoardView::getBoardPosition(QPointF scenePos) const
{
    int r = static_cast<int>(floor(scenePos.y() / cellSpacing));
    int c = 0;

    if (currentBoard && dynamic_cast<OffsetGridBoard *>(currentBoard))
    {
        qreal x_adjusted = scenePos.x();
        if (r % 2 != 0)
        { // If the row is odd, adjust x back for calculation
            x_adjusted -= cellSpacing / 2.0;
        }
        c = static_cast<int>(floor(x_adjusted / cellSpacing));
    }
    else
    {
        c = static_cast<int>(floor(scenePos.x() / cellSpacing));
    }

    // Basic bounds check, more specific validation should be done by Board model
    if (currentBoard && (r < 0 || r >= currentBoard->getRows() || c < 0 || c >= currentBoard->getCols()))
    {
        // Return an invalid position marker or handle error
        // For now, let's return the calculated one and let the model validate
    }
    return {r, c};
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePoint = mapToScene(event->pos());

    Position clickedBoardPos = getBoardPosition(scenePoint);

    if (currentBoard && currentBoard->getPegState(clickedBoardPos) != PegState::Blocked)
    {
        qDebug() << "BoardView: Clicked on board at: (" << clickedBoardPos.row << "," << clickedBoardPos.col << ") Scene: " << scenePoint;
        emit pegClicked(clickedBoardPos);
    }
    else
    {
        qDebug() << "BoardView: Clicked on invalid or blocked area: (" << clickedBoardPos.row << "," << clickedBoardPos.col << ")";
    }
    QGraphicsView::mousePressEvent(event); // Call base class if not handled or for other features
}
