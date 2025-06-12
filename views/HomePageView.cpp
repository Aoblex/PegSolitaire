#include "HomePageView.h"
#include "utils/ButtonStyles.h"
#include <QFont>
#include <QApplication>

HomePageView::HomePageView(QWidget *parent)
    : QWidget(parent)
{
    // Title Label
    titleLabel = new QLabel("Peg Solitaire", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);    // Buttons
    startButton = new QPushButton("Start", this);
    settingsButton = new QPushButton("Settings", this);
    quitButton = new QPushButton("Quit", this);

    // Apply consistent button styles
    ButtonStyles::applyPrimaryStyle(startButton);
    ButtonStyles::applyPrimaryStyle(settingsButton);
    ButtonStyles::applyPrimaryStyle(quitButton);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(startButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(quitButton);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);

    setLayout(mainLayout);

    // Connect signals
    connect(startButton, &QPushButton::clicked, this, &HomePageView::startClicked);
    connect(settingsButton, &QPushButton::clicked, this, &HomePageView::settingsClicked);
    connect(quitButton, &QPushButton::clicked, this, &HomePageView::quitClicked);
}

HomePageView::~HomePageView()
{
}
