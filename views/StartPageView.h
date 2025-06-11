#ifndef STARTPAGEVIEW_H
#define STARTPAGEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>

class StartPageView : public QWidget
{
    Q_OBJECT

public:
    explicit StartPageView(QWidget *parent = nullptr);
    ~StartPageView();

signals:
    void backClicked();
    void gameModeSelected(const QString &mode); // Signal to indicate a game mode was chosen

private:
    QLabel *titleLabel;
    QPushButton *classicButton;
    QPushButton *europeanButton;
    QPushButton *crossButton;
    QPushButton *starButton;
    QPushButton *endgameButton;
    // Add more buttons for other modes if needed

    QPushButton *backButton;

    QVBoxLayout *mainLayout;
    QGridLayout *gridLayout; // For game mode buttons
};

#endif // STARTPAGEVIEW_H
