#include "StartPageView.h"
#include <QFont>

StartPageView::StartPageView(QWidget *parent)
    : QWidget(parent)
{
    // Title Label
    titleLabel = new QLabel("Select Game Mode", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Game Mode Buttons
    classicButton = new QPushButton("Classic", this);
    europeanButton = new QPushButton("European", this);
    crossButton = new QPushButton("Cross", this);
    starButton = new QPushButton("Star", this);
    endgameButton = new QPushButton("Endgame", this);

    // Back Button
    backButton = new QPushButton("Back to Home", this);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);

    gridLayout = new QGridLayout();
    gridLayout->addWidget(classicButton, 0, 0);
    gridLayout->addWidget(europeanButton, 0, 1);
    gridLayout->addWidget(crossButton, 1, 0);
    gridLayout->addWidget(starButton, 1, 1);
    gridLayout->addWidget(endgameButton, 2, 0, 1, 2); // Span across two columns

    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(backButton);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(15);

    setLayout(mainLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &StartPageView::backClicked);
    connect(classicButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected("classic"); });
    connect(europeanButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected("european"); });
    connect(crossButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected("cross"); });
    connect(starButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected("star"); });
    connect(endgameButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected("endgame"); });
}

StartPageView::~StartPageView()
{
}
