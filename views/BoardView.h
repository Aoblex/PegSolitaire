#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QShowEvent>
#include <QList>
#include "models/Board.h"

/**
 * @brief View class for displaying and interacting with the Peg Solitaire board
 * 
 * This class handles the visual representation of the board, peg interactions,
 * move highlighting, and provides control buttons for game management.
 */
class BoardView : public QWidget
{
    Q_OBJECT

public:
    explicit BoardView(QWidget *parent = nullptr);
    ~BoardView();

    /**
     * @brief Set the board model to display
     * @param board Pointer to the board model
     */
    void setBoard(Board *board);

    /**
     * @brief Update the view to reflect current board state
     */
    void updateView();

    /**
     * @brief Highlight valid moves on the board
     * @param moves List of moves to highlight
     */
    void highlightMoves(const QList<Move> &moves);

    /**
     * @brief Update the peg count display
     * @param count Number of pegs remaining
     */
    void updatePegCount(int count);

signals:
    /**
     * @brief Emitted when a peg or empty cell is clicked
     * @param pos Position that was clicked
     */
    void pegClicked(const Position &pos);

    /**
     * @brief Emitted when undo button is clicked
     */
    void undoClicked();

    /**
     * @brief Emitted when reset button is clicked
     */
    void resetClicked();

    /**
     * @brief Emitted when home button is clicked
     */
    void homeClicked();

    /**
     * @brief Emitted when suggest move button is clicked (spacebar)
     */
    void suggestMoveClicked();

protected:
    /**
     * @brief Handle paint events to draw the board
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Handle mouse press events for peg selection
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handle key press events (e.g., spacebar for suggest move)
     */
    void keyPressEvent(QKeyEvent *event) override;    /**
     * @brief Handle resize events to recalculate board dimensions
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief Handle show events to recalculate board dimensions
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief Handle undo button press (kept for compatibility)
     */
    void onUndoButtonClicked();

    /**
     * @brief Handle reset button press (kept for compatibility)
     */
    void onResetButtonClicked();

    /**
     * @brief Handle home button press (kept for compatibility)
     */
    void onHomeButtonClicked();

private:
    // Board data
    Board *boardModel;
    QList<Move> highlightedMoves;
    
    // Visual properties - now dynamic
    int cellSize;
    int pegRadius;
    int boardMargin;
    
    // Colors
    QColor pegColor;
    QColor emptyColor;
    QColor blockedColor;
    QColor selectedColor;
    QColor highlightColor;
    QColor boardBackgroundColor;
    
    /**
     * @brief Initialize the UI components
     */
    void setupUI();
    
    /**
     * @brief Setup color scheme for the board
     */
    void setupColors();
    
    /**
     * @brief Convert mouse coordinates to board position
     * @param point Mouse coordinates
     * @return Board position, or invalid position if outside board
     */
    Position getBoardPosition(const QPoint &point);
    
    /**
     * @brief Convert board position to screen coordinates
     * @param pos Board position
     * @return Screen coordinates for the center of the cell
     */
    QPoint getScreenPosition(const Position &pos);
    
    /**
     * @brief Check if a position is highlighted for moves
     * @param pos Position to check
     * @return True if position is highlighted
     */
    bool isPositionHighlighted(const Position &pos);
    
    /**
     * @brief Draw a single cell (peg, empty, or blocked)
     * @param painter Painter object
     * @param pos Board position
     * @param screenPos Screen coordinates
     */
    void drawCell(QPainter &painter, const Position &pos, const QPoint &screenPos);    /**
     * @brief Calculate the board widget size based on board dimensions
     * @return Recommended size for the board widget
     */
    QSize calculateBoardSize();
    
    /**
     * @brief Calculate optimal cell size based on available space
     */
    void calculateDynamicSizes();
    
    /**
     * @brief Get the centered board rectangle within the board widget
     * @return Rectangle representing the centered board area
     */
    QRect getCenteredBoardRect();
};

#endif // BOARDVIEW_H