#ifndef HOMEPAGEVIEW_H
#define HOMEPAGEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class HomePageView : public QWidget
{
    Q_OBJECT

public:
    explicit HomePageView(QWidget *parent = nullptr);
    ~HomePageView();

signals:
    void startClicked();
    void settingsClicked();
    void quitClicked();

private:
    QLabel *titleLabel;
    QPushButton *startButton;
    QPushButton *settingsButton;
    QPushButton *quitButton;
    QVBoxLayout *mainLayout;
};

#endif // HOMEPAGEVIEW_H
