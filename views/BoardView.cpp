#include "BoardView.h"
#include "utils/ButtonStyles.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFont>
#include <QSizePolicy>
#include <QtCore/qmath.h>
#include <QTimer>

BoardView::BoardView(QWidget *parent)
    : QWidget(parent),
      mainLayout(nullptr),
      controlLayout(nullptr),
      infoLayout(nullptr),
      boardWidget(nullptr),
      undoButton(nullptr),
      resetButton(nullptr),
      homeButton(nullptr),
      pegCountLabel(nullptr),
      instructionLabel(nullptr),
      boardModel(nullptr),
      cellSize(40),
      pegRadius(15),
      boardMargin(20)
{
    setupColors();
    setupUI();
    setFocusPolicy(Qt::StrongFocus); // Enable keyboard events
}

BoardView::~BoardView()
{
    // Qt's parent-child mechanism will handle cleanup
}

void BoardView::setupColors()
{
    // Setup beautiful colors for the board
    pegColor = QColor(139, 69, 19);          // Saddle brown for pegs
    emptyColor = QColor(245, 245, 220);      // Beige for empty holes
    blockedColor = QColor(47, 79, 79);       // Dark slate gray for blocked areas
    selectedColor = QColor(255, 215, 0);     // Gold for selected peg
    highlightColor = QColor(50, 205, 50);    // Lime green for valid moves
    boardBackgroundColor = QColor(222, 184, 135); // Burlywood for board background
}

void BoardView::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Info layout for peg count and instructions
    infoLayout = new QHBoxLayout();
    
    pegCountLabel = new QLabel("Pegs: 0", this);
    QFont labelFont = pegCountLabel->font();
    labelFont.setPointSize(14);
    labelFont.setBold(true);
    pegCountLabel->setFont(labelFont);
    pegCountLabel->setAlignment(Qt::AlignCenter);
    
    instructionLabel = new QLabel("Click a peg to select it, then click an empty hole to move", this);
    instructionLabel->setFont(labelFont);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    
    infoLayout->addWidget(pegCountLabel);
    infoLayout->addWidget(instructionLabel, 1); // Give more space to instructions
    
    mainLayout->addLayout(infoLayout);

    // Board widget - this will expand to fill available space
    boardWidget = new QWidget(this);
    boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    boardWidget->setMinimumSize(200, 200);  // Ensure minimum usable size
    mainLayout->addWidget(boardWidget, 1); // Give most space to the board

    // Control buttons layout
    controlLayout = new QHBoxLayout();
    
    undoButton = new QPushButton("Undo", this);
    resetButton = new QPushButton("Reset", this);
    homeButton = new QPushButton("Home", this);
    
    // Apply consistent button styles using ButtonStyles utility
    ButtonStyles::applyControlStyle(undoButton);
    ButtonStyles::applyControlStyle(resetButton);
    ButtonStyles::applyControlStyle(homeButton);
    
    controlLayout->addWidget(undoButton);
    controlLayout->addWidget(resetButton);
    controlLayout->addWidget(homeButton);
    controlLayout->setSpacing(10);
    
    mainLayout->addLayout(controlLayout);

    // Connect button signals
    connect(undoButton, &QPushButton::clicked, this, &BoardView::onUndoButtonClicked);
    connect(resetButton, &QPushButton::clicked, this, &BoardView::onResetButtonClicked);
    connect(homeButton, &QPushButton::clicked, this, &BoardView::onHomeButtonClicked);

    setLayout(mainLayout);
    
    // Set size policy for the main widget to expand
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BoardView::setBoard(Board *board)
{
    boardModel = board;
    if (boardModel) {
        calculateDynamicSizes();
        updateView();
        updatePegCount(boardModel->getPegCount());
    }
}

void BoardView::updateView()
{
    if (boardWidget) {
        boardWidget->update();
        update(); // Update the entire widget
    }
}

void BoardView::highlightMoves(const QList<Move> &moves)
{
    highlightedMoves = moves;
    updateView();
}

void BoardView::updatePegCount(int count)
{
    if (pegCountLabel) {
        pegCountLabel->setText(QString("Pegs: %1").arg(count));
    }
}

void BoardView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    if (!boardModel) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    // Get board widget geometry
    QRect boardRect = boardWidget->geometry();
    
    // Get the centered board rectangle
    QRect centeredBoardRect = getCenteredBoardRect();
      // Fill the centered board background
    painter.fillRect(centeredBoardRect, boardBackgroundColor);

    // Draw board cells
    int rows = boardModel->getRows();
    int cols = boardModel->getCols();
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Position pos(r, c);
            QPoint screenPos = getScreenPosition(pos);
            
            // Only draw if position is within the board widget
            if (boardRect.contains(screenPos)) {
                drawCell(painter, pos, screenPos);
            }
        }
    }
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
    if (!boardModel || event->button() != Qt::LeftButton) {
        return;
    }

    Position pos = getBoardPosition(event->pos());
    
    // Check if click is within valid board bounds
    if (pos.row >= 0 && pos.row < boardModel->getRows() && 
        pos.col >= 0 && pos.col < boardModel->getCols()) {
        
        PegState state = boardModel->getPegState(pos);
        if (state != PegState::Blocked) {
            qDebug() << "BoardView: Clicked position (" << pos.row << "," << pos.col << ")";
            emit pegClicked(pos);
        }
    }
}

void BoardView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        emit suggestMoveClicked();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (boardModel) {
        // Small delay to ensure geometry is updated
        QTimer::singleShot(0, this, [this]() {
            calculateDynamicSizes();
            updateView();
        });
    }
}

void BoardView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (boardModel) {
        // Recalculate sizes when widget becomes visible
        QTimer::singleShot(0, this, [this]() {
            calculateDynamicSizes();
            updateView();
        });
    }
}

void BoardView::onUndoButtonClicked()
{
    emit undoClicked();
}

void BoardView::onResetButtonClicked()
{
    emit resetClicked();
}

void BoardView::onHomeButtonClicked()
{
    emit homeClicked();
}

Position BoardView::getBoardPosition(const QPoint &point)
{
    if (!boardModel || !boardWidget) {
        return Position(-1, -1);
    }

    QRect centeredBoardRect = getCenteredBoardRect();
    if (centeredBoardRect.isEmpty()) {
        return Position(-1, -1);
    }
    
    // Calculate relative position within the centered board
    int relativeX = point.x() - centeredBoardRect.left();
    int relativeY = point.y() - centeredBoardRect.top();
    
    // Convert to board coordinates
    int col = (relativeX - boardMargin) / cellSize;
    int row = (relativeY - boardMargin) / cellSize;
    
    return Position(row, col);
}

QPoint BoardView::getScreenPosition(const Position &pos)
{
    if (!boardWidget || !boardModel) {
        return QPoint(-1, -1);
    }

    QRect centeredBoardRect = getCenteredBoardRect();
    if (centeredBoardRect.isEmpty()) {
        return QPoint(-1, -1);
    }
    
    int x = centeredBoardRect.left() + boardMargin + pos.col * cellSize + cellSize / 2;
    int y = centeredBoardRect.top() + boardMargin + pos.row * cellSize + cellSize / 2;
    
    return QPoint(x, y);
}

bool BoardView::isPositionHighlighted(const Position &pos)
{
    for (const Move &move : highlightedMoves) {
        if ((move.from.row == pos.row && move.from.col == pos.col) ||
            (move.to.row == pos.row && move.to.col == pos.col)) {
            return true;
        }
    }
    return false;
}

void BoardView::drawCell(QPainter &painter, const Position &pos, const QPoint &screenPos)
{
    if (!boardModel) {
        return;
    }

    PegState state = boardModel->getPegState(pos);
    
    // Choose color based on state and highlights
    QColor cellColor;
    bool highlighted = isPositionHighlighted(pos);
    
    switch (state) {
        case PegState::Peg:
            cellColor = highlighted ? selectedColor : pegColor;
            break;
        case PegState::Empty:
            cellColor = highlighted ? highlightColor : emptyColor;
            break;
        case PegState::Blocked:
            cellColor = blockedColor;
            break;
    }

    if (state == PegState::Blocked) {
        // Don't draw anything for blocked cells
        return;
    }

    // Draw the cell background (for grid appearance)
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(QBrush(boardBackgroundColor));
    QRect cellRect(screenPos.x() - cellSize/2, screenPos.y() - cellSize/2, cellSize, cellSize);
    painter.drawRect(cellRect);

    if (state == PegState::Peg) {
        // Draw peg as a filled circle
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(QBrush(cellColor));
        painter.drawEllipse(screenPos, pegRadius, pegRadius);
        
        // Add some shading for 3D effect
        painter.setPen(QPen(Qt::white, 1));
        painter.drawArc(screenPos.x() - pegRadius/2, screenPos.y() - pegRadius/2, 
                       pegRadius, pegRadius, 45 * 16, 90 * 16);
    } else if (state == PegState::Empty) {
        // Draw empty hole as a circle outline
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(QBrush(cellColor));
        painter.drawEllipse(screenPos, pegRadius, pegRadius);
        
        // Draw inner shadow for hole effect
        painter.setPen(QPen(Qt::darkGray, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(screenPos, pegRadius - 2, pegRadius - 2);
    }

    // Draw highlight border if needed
    if (highlighted) {
        painter.setPen(QPen(highlightColor, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(screenPos, pegRadius + 4, pegRadius + 4);
    }
}

QSize BoardView::calculateBoardSize()
{
    if (!boardModel) {
        return QSize(400, 400);
    }

    int rows = boardModel->getRows();
    int cols = boardModel->getCols();
    
    int width = cols * cellSize + 2 * boardMargin;
    int height = rows * cellSize + 2 * boardMargin;
    
    return QSize(width, height);
}

void BoardView::calculateDynamicSizes()
{
    if (!boardModel || !boardWidget) {
        return;
    }

    // Get available space for the board widget
    QRect availableSpace = boardWidget->geometry();
    
    // If board widget geometry is not set or too small, use parent size
    if (availableSpace.width() <= 0 || availableSpace.height() <= 0) {
        availableSpace = rect();
        // Reserve space for controls and info labels (approximately 120px)
        int reservedHeight = 120;
        if (availableSpace.height() > reservedHeight) {
            availableSpace.setHeight(availableSpace.height() - reservedHeight);
        }
        // Add some margin
        availableSpace.adjust(10, 10, -10, -10);
    }

    int rows = boardModel->getRows();
    int cols = boardModel->getCols();
    
    if (rows <= 0 || cols <= 0) {
        return;
    }

    // Start with default margin
    int tempMargin = 20;
    
    // Calculate maximum cell size that fits in available space
    int maxCellWidth = (availableSpace.width() - 2 * tempMargin) / cols;
    int maxCellHeight = (availableSpace.height() - 2 * tempMargin) / rows;
    
    // Use the smaller dimension to ensure the board fits (maintain aspect ratio)
    cellSize = qMin(maxCellWidth, maxCellHeight);
    
    // Set reasonable bounds for cell size
    cellSize = qMax(cellSize, 15);  // Minimum size for usability
    cellSize = qMin(cellSize, 100); // Maximum size to prevent oversized board
    
    // Calculate peg radius based on cell size (about 60-70% of cell size)
    pegRadius = static_cast<int>(cellSize * 0.3);
    pegRadius = qMax(pegRadius, 6);   // Minimum peg size
    pegRadius = qMin(pegRadius, 35);  // Maximum peg size
      // Adjust board margin based on cell size
    boardMargin = static_cast<int>(cellSize * 0.4);
    boardMargin = qMax(boardMargin, 8);   // Minimum margin
    boardMargin = qMin(boardMargin, 50);  // Maximum margin
}

QRect BoardView::getCenteredBoardRect()
{
    if (!boardModel || !boardWidget) {
        return QRect();
    }

    QRect boardRect = boardWidget->geometry();
    
    // Calculate the actual board size
    int rows = boardModel->getRows();
    int cols = boardModel->getCols();
    int boardWidth = cols * cellSize + 2 * boardMargin;
    int boardHeight = rows * cellSize + 2 * boardMargin;
    
    // Center the board within the widget
    int startX = boardRect.left() + (boardRect.width() - boardWidth) / 2;
    int startY = boardRect.top() + (boardRect.height() - boardHeight) / 2;
    
    return QRect(startX, startY, boardWidth, boardHeight);
}