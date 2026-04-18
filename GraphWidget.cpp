// graphwidget.cpp
#include "GraphWidget.h"
#include "QOpenGLWidget"
#include <QKeyEvent>
#include <QWheelEvent>

namespace inv {

GraphWidget::GraphWidget(QWidget *parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // --- 关键代码：设置拖拽模式 ---
    setDragMode(QGraphicsView::ScrollHandDrag);

    // 可选：设置光标样式，提升用户体验
    setCursor(Qt::OpenHandCursor); // 默认显示张开的手

    // --- 性能优化开关 ---
    // 1. 关闭视口更新时的抗锯齿调整（提升平移/缩放速度）
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);

    // 2. 不保存 Painter 状态（提升绘制速度）
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    // 3. 使用间接绘制（配合缓存更高效）
    setOptimizationFlag(QGraphicsView::IndirectPainting, true);

    // 4. 设置视口更新模式为“智能”或“最小”
    // 避免拖拽时重绘整个窗口
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // QOpenGLWidget *glWidget = new QOpenGLWidget;
    // setViewport(glWidget);
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    // 实现缩放
    double scaleFactor = 1.15;
    if (event->delta() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void GraphWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        // 删除选中的节点或连线
        QList<QGraphicsItem *> items = scene()->selectedItems();
        for (QGraphicsItem *item : items) {
            // 这里需要做类型判断并清理
            scene()->removeItem(item);
            delete item;
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

} // namespace inv