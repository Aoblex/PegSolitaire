#ifndef HOMEPAGECONTROLLER_H
#define HOMEPAGECONTROLLER_H

#include <QObject>
#include "../views/HomePageView.h"

class HomePageController : public QObject
{
    Q_OBJECT

public:
    explicit HomePageController(HomePageView *view, QObject *parent = nullptr);

public slots:
    void onStartClicked();
    void onSettingsClicked();
    void onQuitClicked();

private:
    HomePageView *m_view;
};

#endif // HOMEPAGECONTROLLER_H
