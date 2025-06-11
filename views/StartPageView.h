#ifndef STARTPAGEVIEW_H
#define STARTPAGEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout> // Keep for now, might remove if QHBoxLayout is sufficient
#include <QHBoxLayout> // For columns

class StartPageView : public QWidget
{
    Q_OBJECT

public:
    explicit StartPageView(QWidget *parent = nullptr);
    ~StartPageView();

signals:
    void backClicked();
    void gameModeSelected(const QString &mode);

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
    QPushButton *classicStarButton;

    // Column 2: Special Game Mode
    QVBoxLayout *specialModeLayout;
    QLabel *specialModeLabel;
    QPushButton *specialAntiPegButton;
    QPushButton *specialDestinationButton;
    QPushButton *specialEndgameButton;

    // Column 3: Peg Duo Mode
    QVBoxLayout *pegDuoModeLayout; // Changed from creativeModeLayout
    QLabel *pegDuoModeLabel;       // Changed from creativeModeLabel
    QPushButton *pegDuoButton;

    QPushButton *backButton;
    QVBoxLayout *mainLayout; // Overall vertical layout

    // Remove old individual buttons and gridLayout if no longer directly used
    // QPushButton *classicButton;
    // QPushButton *europeanButton;
    // QPushButton *crossButton;
    // QPushButton *starButton;
    // QPushButton *endgameButton;
    // QGridLayout *gridLayout;
};

#endif // STARTPAGEVIEW_H
