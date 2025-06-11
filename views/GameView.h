#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include "views/BoardView.h"
#include "controllers/BoardController.h"
#include "models/Board.h"

/**
 * @brief Container view that manages the board view and controller
 * 
 * This class acts as a container that combines the BoardView with its controller
 * to provide a complete game interface.
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

private:
    QVBoxLayout *mainLayout;
    BoardView *boardView;
    BoardController *boardController;
};

#endif // GAMEVIEW_H
