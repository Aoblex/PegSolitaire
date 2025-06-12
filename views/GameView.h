#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include "views/BoardView.h"
#include "controllers/BoardController.h"
#include "models/Board.h"

/**
 * @brief Container view that manages the board view and controller with proper game UI layout
 * 
 * This class acts as a container that combines the BoardView with scoring board and controls
 * to provide a complete game interface following the specified layout requirements:
 * - Scoring board on the right
 * - Peg board on the left aligned with scoring board
 * - Control buttons at upper right corner
 */
class GameView : public QWidget
{
    Q_OBJECT

public:
    explicit GameView(QWidget *parent = nullptr);
    ~GameView();

    /**
     * @brief Set the board model and setup the game
     * @param board Pointer to the board model
     */
    void setBoard(Board *board);

signals:
    /**
     * @brief Emitted when user requests to navigate back to home
     */
    void navigateToHome();

protected:
    /**
     * @brief Handle resize events to properly resize the guide overlay
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    // Main layout structure
    QVBoxLayout *mainLayout;
    QHBoxLayout *gameLayout;
    QVBoxLayout *rightSideLayout;
    QHBoxLayout *controlButtonsLayout;
    
    // Board display
    BoardView *boardView;
      // Right side components
    QLabel *scoringBoardTitle;
    QLabel *pegCountLabel;
      // Control buttons (upper right)
    QPushButton *undoButton;
    QPushButton *resetButton;
    QPushButton *homeButton;
    QPushButton *guideButton;
      // Guide overlay
    QWidget *guideOverlay;
    
    // Game result overlays
    QWidget *gameResultOverlay;
    
    // Controller
    BoardController *boardController;
    
    /**
     * @brief Setup the UI layout according to requirements
     */
    void setupUI();
      /**
     * @brief Update the peg count display
     * @param count Current number of pegs
     */
    void updatePegCount(int count);
      /**
     * @brief Show the guide information dialog
     */
    void showGuideDialog();
    
    /**
     * @brief Show game result overlay (win or lose)
     * @param isWin True if player won, false if lost
     * @param pegCount Final number of pegs remaining
     */
    void showGameResultOverlay(bool isWin, int pegCount);

private slots:
    /**
     * @brief Handle game over signal from board controller
     */
    void onGameOver();
};

#endif // GAMEVIEW_H
