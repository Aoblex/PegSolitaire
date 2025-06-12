#ifndef STARTPAGEVIEW_H
#define STARTPAGEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout> // Keep for now, might remove if QHBoxLayout is sufficient
#include <QHBoxLayout> // For columns
#include "models/Board.h" // Added for BoardType

class StartPageView : public QWidget
{
    Q_OBJECT

public:
    explicit StartPageView(QWidget *parent = nullptr);
    ~StartPageView();

signals:
    void backClicked();
    void gameModeSelected(BoardType mode); // Changed from const QString&

private:
    QLabel *titleLabel;

    // Main layout for columns
    QHBoxLayout *columnsLayout;

    // Column 1: Classic Mode
    QVBoxLayout *classicModeLayout;
    QLabel *classicModeLabel;
    QPushButton *classicEnglishButton;
    QPushButton *classicEuropeanButton;
    QPushButton *classicCrossButton;

    // Column 2: Special Game Mode
    QVBoxLayout *specialModeLayout;
    QLabel *specialModeLabel;
    QPushButton *specialAntiPegButton;
    QPushButton *specialEndgameButton;

    // Column 3: Peg Duo Mode
    QVBoxLayout *pegDuoModeLayout; // Changed from creativeModeLayout
    QLabel *pegDuoModeLabel;       // Changed from creativeModeLabel
    QPushButton *pegDuoButton;

    QPushButton *backButton;
    QVBoxLayout *mainLayout; // Overall vertical layout
};

#endif // STARTPAGEVIEW_H
