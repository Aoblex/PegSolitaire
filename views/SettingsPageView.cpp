#include "SettingsPageView.h"
#include "utils/ButtonStyles.h"
#include <QFont>

SettingsPageView::SettingsPageView(QWidget *parent)
    : QWidget(parent)
{
    // Title Label
    titleLabel = new QLabel("Settings", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Fullscreen CheckBox
    fullscreenCheckBox = new QCheckBox("Enable Fullscreen", this);

    // Back Button
    backButton = new QPushButton("Back to Home", this);
    
    // Apply consistent button styling
    ButtonStyles::applySecondaryStyle(backButton);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(fullscreenCheckBox);
    mainLayout->addStretch(); // Add stretch to push the button to the bottom or provide spacing
    mainLayout->addWidget(backButton);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(15);

    setLayout(mainLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &SettingsPageView::backClicked);
    connect(fullscreenCheckBox, &QCheckBox::checkStateChanged, this, &SettingsPageView::onFullscreenCheckBoxStateChanged);
}

SettingsPageView::~SettingsPageView()
{
}

bool SettingsPageView::isFullscreenChecked() const
{
    return fullscreenCheckBox->isChecked();
}

void SettingsPageView::setFullscreenChecked(bool checked)
{
    fullscreenCheckBox->setChecked(checked);
}

void SettingsPageView::onFullscreenCheckBoxStateChanged(int state)
{
    emit fullscreenToggled(state == Qt::Checked);
}
