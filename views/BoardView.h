#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "../../models/Board.h" // Adjust path as necessary

class BoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit BoardView(QWidget *parent = nullptr);
    void setBoard(Board *board);
    void updateView();

signals:
    void pegClicked(Position pos);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    Board *currentBoard;
    qreal pegRadius;
    qreal cellSpacing;

    void drawGridBoard();
    void drawOffsetGridBoard();
    // Add other draw methods for different board types if needed

    // Helper to map board coordinates to scene coordinates
    QPointF getScenePosition(Position boardPos) const;
    // Helper to map scene coordinates back to board positions
    Position getBoardPosition(QPointF scenePos) const;
};

#endif // BOARDVIEW_H
