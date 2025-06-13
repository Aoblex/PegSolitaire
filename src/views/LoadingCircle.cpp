#include "LoadingCircle.h"
#include <QPainter>
#include <QResizeEvent>
#include <QFont>
#include <QFontMetrics>
#include <QtCore/qmath.h>

LoadingCircle::LoadingCircle(QWidget *parent)
    : QWidget(parent),
      animationTimer(new QTimer(this)),
      rotationAngle(0),
      message("Searching for solution...")
{
    // Set up the widget
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    hide();
    
    // Set up the animation timer
    connect(animationTimer, &QTimer::timeout, this, &LoadingCircle::updateAnimation);
    animationTimer->setInterval(50); // 20 FPS animation
    
    calculateCircleRect();
}

LoadingCircle::~LoadingCircle()
{
    // Qt's parent-child mechanism will handle cleanup
}

void LoadingCircle::startAnimation()
{
    show();
    raise();
    animationTimer->start();
}

void LoadingCircle::stopAnimation()
{
    animationTimer->stop();
    hide();
}

void LoadingCircle::setMessage(const QString &newMessage)
{
    message = newMessage;
    update();
}

void LoadingCircle::updateAnimation()
{
    rotationAngle = (rotationAngle + 10) % 360;
    update();
}

void LoadingCircle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill background with semi-transparent overlay
    painter.fillRect(rect(), QColor(0, 0, 0, 100));
    
    // Draw the loading circle
    drawLoadingCircle(painter);
    
    // Draw the message
    drawMessage(painter);
}

void LoadingCircle::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    calculateCircleRect();
}

void LoadingCircle::calculateCircleRect()
{
    // Calculate circle size based on widget size
    int size = qMin(width(), height()) / 3;
    size = qMax(size, 60); // Minimum size
    size = qMin(size, 150); // Maximum size
    
    // Center the circle
    int x = (width() - size) / 2;
    int y = (height() - size) / 2 - 20; // Offset up a bit for the message
    
    circleRect = QRect(x, y, size, size);
}

void LoadingCircle::drawLoadingCircle(QPainter &painter)
{
    painter.save();
    
    // Move to center of circle
    painter.translate(circleRect.center());
    painter.rotate(rotationAngle);
    
    // Set up pen
    QPen pen;
    pen.setWidth(6);
    pen.setCapStyle(Qt::RoundCap);
    
    // Draw multiple arcs with different opacities to create loading effect
    int numArcs = 8;
    for (int i = 0; i < numArcs; ++i) {
        // Calculate opacity (fading from bright to dim)
        int opacity = 255 - (i * 255 / numArcs);
        pen.setColor(QColor(52, 152, 219, opacity)); // Nice blue color
        painter.setPen(pen);
        
        // Calculate arc parameters
        int startAngle = (i * 360 / numArcs) * 16; // Qt uses 16ths of a degree
        int spanAngle = (360 / numArcs / 2) * 16;  // Half segment
        
        // Draw the arc
        QRect arcRect(-circleRect.width()/2 + 10, -circleRect.height()/2 + 10, 
                      circleRect.width() - 20, circleRect.height() - 20);
        painter.drawArc(arcRect, startAngle, spanAngle);
    }
    
    painter.restore();
}

void LoadingCircle::drawMessage(QPainter &painter)
{
    if (message.isEmpty()) {
        return;
    }
    
    painter.save();
    
    // Set up font and color
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255));
    
    // Calculate text position (below the circle)
    QFontMetrics metrics(font);
    QRect textRect = metrics.boundingRect(message);
    int textX = (width() - textRect.width()) / 2;
    int textY = circleRect.bottom() + 30;
    
    // Draw text with shadow effect
    painter.setPen(QColor(0, 0, 0, 150));
    painter.drawText(textX + 1, textY + 1, message);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(textX, textY, message);
    
    painter.restore();
}
