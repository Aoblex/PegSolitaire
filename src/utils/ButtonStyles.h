#ifndef BUTTONSTYLES_H
#define BUTTONSTYLES_H

#include <QPushButton>
#include <QFont>
#include <QString>

/**
 * @brief Utility class for applying consistent button styles across the application
 */
class ButtonStyles
{
public:
    /**
     * @brief Apply primary button style (for main actions like Start, Back, etc.)
     */
    static void applyPrimaryStyle(QPushButton* button);
    
    /**
     * @brief Apply secondary button style (for less prominent actions)
     */
    static void applySecondaryStyle(QPushButton* button);
    
    /**
     * @brief Apply game mode button style (for game selection buttons)
     */
    static void applyGameModeStyle(QPushButton* button);
    
    /**
     * @brief Apply control button style (for game controls like Undo, Reset)
     */
    static void applyControlStyle(QPushButton* button);
    
    /**
     * @brief Set consistent font for all buttons
     */
    static QFont getButtonFont(int pointSize = 12, bool bold = true);

private:
    // Common style strings
    static QString getPrimaryStyleSheet();
    static QString getSecondaryStyleSheet();
    static QString getGameModeStyleSheet();
    static QString getControlStyleSheet();
};

#endif // BUTTONSTYLES_H
