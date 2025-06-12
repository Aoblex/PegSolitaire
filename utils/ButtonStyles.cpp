#include "ButtonStyles.h"

void ButtonStyles::applyPrimaryStyle(QPushButton* button)
{
    if (!button) return;
    
    button->setFont(getButtonFont(14, true));
    button->setStyleSheet(getPrimaryStyleSheet());
    button->setMinimumSize(160, 50);
    button->setMaximumSize(200, 50);
    button->setCursor(Qt::PointingHandCursor);
}

void ButtonStyles::applySecondaryStyle(QPushButton* button)
{
    if (!button) return;
    
    button->setFont(getButtonFont(12, true));
    button->setStyleSheet(getSecondaryStyleSheet());
    button->setMinimumSize(140, 45);
    button->setMaximumSize(180, 45);
    button->setCursor(Qt::PointingHandCursor);
}

void ButtonStyles::applyGameModeStyle(QPushButton* button)
{
    if (!button) return;
    
    button->setFont(getButtonFont(12, false));
    button->setStyleSheet(getGameModeStyleSheet());
    button->setMinimumSize(150, 45);
    button->setMaximumSize(200, 45);
    button->setCursor(Qt::PointingHandCursor);
}

void ButtonStyles::applyControlStyle(QPushButton* button)
{
    if (!button) return;
    
    button->setFont(getButtonFont(11, true));
    button->setStyleSheet(getControlStyleSheet());
    button->setMinimumSize(80, 40);
    button->setMaximumSize(120, 40);
    button->setCursor(Qt::PointingHandCursor);
}

QFont ButtonStyles::getButtonFont(int pointSize, bool bold)
{
    QFont font("Segoe UI", pointSize);
    font.setBold(bold);
    font.setWeight(bold ? QFont::Bold : QFont::Normal);
    font.setStyleStrategy(QFont::PreferAntialias);
    return font;
}

QString ButtonStyles::getPrimaryStyleSheet()
{
    return R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #4a90e2, stop:1 #357abd);
            color: white;
            border: 2px solid #2c5aa0;
            border-radius: 8px;
            padding: 10px 20px;
            font-weight: bold;
            text-align: center;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #5ba0f2, stop:1 #4689cd);
            border: 2px solid #4a90e2;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #357abd, stop:1 #2c5aa0);
            border: 2px solid #1e3d6f;
            transform: translateY(1px);
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid #87ceeb;
        }
        QPushButton:disabled {
            background: #cccccc;
            color: #666666;
            border: 2px solid #999999;
        }
    )";
}

QString ButtonStyles::getSecondaryStyleSheet()
{
    return R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #f0f0f0, stop:1 #d4d4d4);
            color: #333333;
            border: 2px solid #999999;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
            text-align: center;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #ffffff, stop:1 #e0e0e0);
            border: 2px solid #777777;
            color: #000000;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #d4d4d4, stop:1 #b8b8b8);
            border: 2px solid #555555;
            transform: translateY(1px);
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid #4a90e2;
        }
        QPushButton:disabled {
            background: #f5f5f5;
            color: #aaaaaa;
            border: 2px solid #cccccc;
        }
    )";
}

QString ButtonStyles::getGameModeStyleSheet()
{
    return R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #ffffff, stop:1 #f8f8f8);
            color: #2c3e50;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 10px 15px;
            text-align: center;
            font-weight: normal;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #ecf0f1, stop:1 #d5dbdb);
            border: 2px solid #95a5a6;
            color: #34495e;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #d5dbdb, stop:1 #bdc3c7);
            border: 2px solid #7f8c8d;
            transform: translateY(1px);
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid #3498db;
        }
        QPushButton:disabled {
            background: #f9f9f9;
            color: #cccccc;
            border: 2px solid #e0e0e0;
        }
    )";
}

QString ButtonStyles::getControlStyleSheet()
{
    return R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #27ae60, stop:1 #229954);
            color: white;
            border: 2px solid #1e8449;
            border-radius: 8px;
            padding: 8px 12px;
            font-weight: bold;
            text-align: center;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #2ecc71, stop:1 #27ae60);
            border: 2px solid #27ae60;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #229954, stop:1 #1e8449);
            border: 2px solid #145a32;
            transform: translateY(1px);
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid #85d1a0;
        }
        QPushButton:disabled {
            background: #cccccc;
            color: #666666;
            border: 2px solid #999999;
        }
    )";
}
