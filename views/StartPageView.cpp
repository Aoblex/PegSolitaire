#include "StartPageView.h"
#include "models/Board.h" // Include BoardType definition
#include <QFont>
#include <QHBoxLayout> // Ensure this is included

StartPageView::StartPageView(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20); // Overall spacing
    mainLayout->setAlignment(Qt::AlignCenter);

    // Title Label
    titleLabel = new QLabel("Select Game Mode", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24); // Increased title font size
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Columns Layout
    columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(30); // Spacing between columns

    // --- Column 1: Classic Mode ---
    classicModeLayout = new QVBoxLayout();
    classicModeLabel = new QLabel("Classic Mode", this);
    classicModeLabel->setFont(titleFont); // Reuse title font for section headers
    classicModeLayout->addWidget(classicModeLabel, 0, Qt::AlignHCenter);
    classicEnglishButton = new QPushButton("English Board", this);
    classicEuropeanButton = new QPushButton("European Board", this);
    classicCrossButton = new QPushButton("Cross Board", this);
    classicStarButton = new QPushButton("Star Board", this);
    classicModeLayout->addWidget(classicEnglishButton);
    classicModeLayout->addWidget(classicEuropeanButton);
    classicModeLayout->addWidget(classicCrossButton);
    classicModeLayout->addWidget(classicStarButton);
    classicModeLayout->addStretch(); // Pushes buttons to the top
    columnsLayout->addLayout(classicModeLayout);

    // --- Column 2: Special Game Mode ---
    specialModeLayout = new QVBoxLayout();
    specialModeLabel = new QLabel("Special Modes", this);
    specialModeLabel->setFont(titleFont);
    specialModeLayout->addWidget(specialModeLabel, 0, Qt::AlignHCenter);
    specialAntiPegButton = new QPushButton("Anti-Peg Mode", this);
    specialDestinationButton = new QPushButton("Destination Mode", this);
    specialEndgameButton = new QPushButton("Endgame Scenarios", this);
    specialModeLayout->addWidget(specialAntiPegButton);
    specialModeLayout->addWidget(specialDestinationButton);
    specialModeLayout->addWidget(specialEndgameButton);
    specialModeLayout->addStretch();
    columnsLayout->addLayout(specialModeLayout);

    // --- Column 3: Peg Duo Mode ---
    pegDuoModeLayout = new QVBoxLayout();
    pegDuoModeLabel = new QLabel("Peg Duo", this);
    pegDuoModeLabel->setFont(titleFont);
    pegDuoModeLayout->addWidget(pegDuoModeLabel, 0, Qt::AlignHCenter);
    pegDuoButton = new QPushButton("Start Peg Duo", this);
    pegDuoModeLayout->addWidget(pegDuoButton);
    pegDuoModeLayout->addStretch();
    columnsLayout->addLayout(pegDuoModeLayout);

    mainLayout->addLayout(columnsLayout);

    // Back Button
    backButton = new QPushButton("Back to Home", this);
    mainLayout->addWidget(backButton, 0, Qt::AlignHCenter); // Center back button

    setLayout(mainLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &StartPageView::backClicked);

    // Classic Mode Buttons
    connect(classicEnglishButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::English); });
    connect(classicEuropeanButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::European); });
    connect(classicCrossButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::Cross); });
    connect(classicStarButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::Star); });

    // Special Mode Buttons
    connect(specialAntiPegButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::English); });
    connect(specialDestinationButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::English); });
    connect(specialEndgameButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::English); });

    // Peg Duo Mode Button
    connect(pegDuoButton, &QPushButton::clicked, this, [this]()
            { emit gameModeSelected(BoardType::English); });
}

StartPageView::~StartPageView()
{
    // Qt's parent-child mechanism should handle deletion of layouts and widgets.
}
