// connection.cpp
#include "connection.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "qdebug.h"

namespace inv 
{

Connection::Connection(Pin *startPin, Pin *endPin, QGraphicsItem *parent)
    : m_startPin(startPin), m_endPin(endPin) {
    setParentItem(parent);
    setFlag(QGraphicsItem::ItemIsSelectable);

    QPen pen;
    pen.setColor(Qt::black);    // 先用亮黄色测试，确保能看见
    pen.setWidth(3);             // 设置宽度为3，更明显
    pen.setStyle(Qt::SolidLine); // 确保是实线
    setPen(pen);

    setZValue(-1);

    updatePath();
}

void Connection::updatePath() {
    QPainterPath path(m_startPin->getCenterPos());
    QPointF p1 = m_startPin->getCenterPos();
    QPointF p2 = m_endPin->getCenterPos();

    // 贝塞尔曲线控制点
    qreal dx = qAbs(p2.x() - p1.x()) * 0.5;
    QPointF c1(p1.x() + dx, p1.y());
    QPointF c2(p2.x() - dx, p2.y());

    path.cubicTo(c1, c2, p2);
    setPath(path);
}

}