#include "BoardView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFont>
#include <QSizePolicy>

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
      boardModel(nullptr)
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
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

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

    // Board widget - this will be where we draw the board
    boardWidget = new QWidget(this);
    boardWidget->setMinimumSize(400, 400);
    boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(boardWidget, 1, Qt::AlignCenter); // Give most space to the board

    // Control buttons layout
    controlLayout = new QHBoxLayout();
    
    undoButton = new QPushButton("Undo", this);
    resetButton = new QPushButton("Reset", this);
    homeButton = new QPushButton("Home", this);
    
    // Style the buttons
    QString buttonStyle = "QPushButton {"
                         "    background-color: #4CAF50;"
                         "    border: none;"
                         "    color: white;"
                         "    padding: 8px 16px;"
                         "    text-align: center;"
                         "    font-size: 14px;"
                         "    font-weight: bold;"
                         "    border-radius: 4px;"
                         "}"
                         "QPushButton:hover {"
                         "    background-color: #45a049;"
                         "}"
                         "QPushButton:pressed {"
                         "    background-color: #3d8b40;"
                         "}";
    
    undoButton->setStyleSheet(buttonStyle);
    resetButton->setStyleSheet(buttonStyle);
    homeButton->setStyleSheet(buttonStyle);
    
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
}

void BoardView::setBoard(Board *board)
{
    boardModel = board;
    if (boardModel) {
        updateView();
        updatePegCount(boardModel->getPegCount());
        
        // Resize the widget based on board size
        QSize boardSize = calculateBoardSize();
        boardWidget->setMinimumSize(boardSize);
        setMinimumSize(boardSize.width() + 40, boardSize.height() + 150); // Extra space for controls
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
    painter.setRenderHint(QPainter::Antialiasing);

    // Get board widget geometry
    QRect boardRect = boardWidget->geometry();
    
    // Fill board background
    painter.fillRect(boardRect, boardBackgroundColor);

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
    if (!boardModel) {
        return Position(-1, -1);
    }

    QRect boardRect = boardWidget->geometry();
    
    // Calculate relative position within the board widget
    int relativeX = point.x() - boardRect.left();
    int relativeY = point.y() - boardRect.top();
    
    // Convert to board coordinates
    int col = (relativeX - BOARD_MARGIN) / CELL_SIZE;
    int row = (relativeY - BOARD_MARGIN) / CELL_SIZE;
    
    return Position(row, col);
}

QPoint BoardView::getScreenPosition(const Position &pos)
{
    if (!boardWidget) {
        return QPoint(-1, -1);
    }

    QRect boardRect = boardWidget->geometry();
    
    int x = boardRect.left() + BOARD_MARGIN + pos.col * CELL_SIZE + CELL_SIZE / 2;
    int y = boardRect.top() + BOARD_MARGIN + pos.row * CELL_SIZE + CELL_SIZE / 2;
    
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
    QRect cellRect(screenPos.x() - CELL_SIZE/2, screenPos.y() - CELL_SIZE/2, CELL_SIZE, CELL_SIZE);
    painter.drawRect(cellRect);

    if (state == PegState::Peg) {
        // Draw peg as a filled circle
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(QBrush(cellColor));
        painter.drawEllipse(screenPos, PEG_RADIUS, PEG_RADIUS);
        
        // Add some shading for 3D effect
        painter.setPen(QPen(Qt::white, 1));
        painter.drawArc(screenPos.x() - PEG_RADIUS/2, screenPos.y() - PEG_RADIUS/2, 
                       PEG_RADIUS, PEG_RADIUS, 45 * 16, 90 * 16);
    } else if (state == PegState::Empty) {
        // Draw empty hole as a circle outline
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(QBrush(cellColor));
        painter.drawEllipse(screenPos, PEG_RADIUS, PEG_RADIUS);
        
        // Draw inner shadow for hole effect
        painter.setPen(QPen(Qt::darkGray, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(screenPos, PEG_RADIUS - 2, PEG_RADIUS - 2);
    }

    // Draw highlight border if needed
    if (highlighted) {
        painter.setPen(QPen(highlightColor, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(screenPos, PEG_RADIUS + 4, PEG_RADIUS + 4);
    }
}

QSize BoardView::calculateBoardSize()
{
    if (!boardModel) {
        return QSize(400, 400);
    }

    int rows = boardModel->getRows();
    int cols = boardModel->getCols();
    
    int width = cols * CELL_SIZE + 2 * BOARD_MARGIN;
    int height = rows * CELL_SIZE + 2 * BOARD_MARGIN;
    
    return QSize(width, height);
}