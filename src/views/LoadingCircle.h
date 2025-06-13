#ifndef LOADINGCIRCLE_H
#define LOADINGCIRCLE_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QRect>

/**
 * @brief A customizable animated loading circle widget
 * 
 * This widget displays a spinning circle animation to indicate loading or processing.
 * It can be overlaid on top of other widgets and shows animated progress.
 */
class LoadingCircle : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingCircle(QWidget *parent = nullptr);
    ~LoadingCircle();

    /**
     * @brief Start the loading animation
     */
    void startAnimation();

    /**
     * @brief Stop the loading animation and hide the widget
     */
    void stopAnimation();

    /**
     * @brief Set the message to display below the loading circle
     * @param message The message text
     */
    void setMessage(const QString &message);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateAnimation();

private:
    QTimer *animationTimer;
    int rotationAngle;
    QString message;
    QRect circleRect;
    
    /**
     * @brief Calculate the position and size of the loading circle
     */
    void calculateCircleRect();
    
    /**
     * @brief Draw the loading circle
     * @param painter The painter to draw with
     */
    void drawLoadingCircle(QPainter &painter);
    
    /**
     * @brief Draw the message text
     * @param painter The painter to draw with
     */
    void drawMessage(QPainter &painter);
};

#endif // LOADINGCIRCLE_H
