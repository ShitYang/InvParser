// pin.cpp
#include "Pin.h"
#include "Node.h"
#include <QPainter>

namespace inv 
{

static constexpr int PIN_RADIUS = 6;
static constexpr int TEXT_OFFSET = 15;

Pin::Pin(Node *parent, const QString &name, PinType type)
    : m_name(name), m_type(type), m_parentNode(parent), m_isConnected(false) {
  setParentItem(parent);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QRectF Pin::boundingRect() const {
  return QRectF(-PIN_RADIUS, -PIN_RADIUS, PIN_RADIUS * 2, PIN_RADIUS * 2);
}

void Pin::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

  painter->setRenderHint(QPainter::Antialiasing);
  QColor color;
  if (m_type == Input) {
    color = QColor(0, 160, 230); // 蓝色
  } else {
    color = QColor(0, 200, 100); // 绿色
  }
  if (m_isConnected) {
    color = color.lighter(120);
  }

  painter->setBrush(color);
  painter->setPen(Qt::white);
  painter->drawEllipse(boundingRect());

  // 绘制文字
  painter->setPen(Qt::white);
  QFont font = painter->font();
  font.setPointSize(8);
  painter->setFont(font);

  int textX =
      (m_type == Input) ? TEXT_OFFSET : -TEXT_OFFSET - m_name.size() * 6;
  painter->drawText(QPointF(textX, 2), m_name);
}

QVariant Pin::itemChange(GraphicsItemChange change, const QVariant &value) {
  // 当引脚位置改变时，通知场景更新
  if (change == ItemScenePositionHasChanged) {
    // 这里可以触发连线更新逻辑
  }
  return QGraphicsItem::itemChange(change, value);
}

QPointF Pin::getCenterPos() const { return scenePos(); }

}