// graphwidget.cpp
#include "GraphWidget.h"
#include "QOpenGLWidget"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>

namespace inv {

GraphWidget::GraphWidget(QWidget *parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setCursor(Qt::OpenHandCursor);

    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setOptimizationFlag(QGraphicsView::IndirectPainting, true);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // QOpenGLWidget *glWidget = new QOpenGLWidget;
    // setViewport(glWidget);
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    double scaleFactor = 1.15;
    if (event->delta() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void GraphWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        QList<QGraphicsItem *> items = scene()->selectedItems();
        for (QGraphicsItem *item : items) {
            scene()->removeItem(item);
            delete item;
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::mousePressEvent(QMouseEvent *event) {
    QGraphicsItem *item = itemAt(event->pos());
    if (item) {
        // Clicked on a node or other item — let Qt handle item move
        QGraphicsView::mousePressEvent(event);
    } else {
        // Clicked on empty space — start manual panning
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::OpenHandCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

} // namespace inv