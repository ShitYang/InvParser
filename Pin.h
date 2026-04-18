// pin.h
#ifndef PIN_H
#define PIN_H

#include <QGraphicsItem>
#include <QString>

namespace inv
{

class Node; // 前向声明

class Pin : public QGraphicsItem {
public:
  enum PinType { Input, Output };

  Pin(Node *parent, const QString &name, PinType type);

  // QGraphicsItem interface
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

  // 获取引脚中心的世界坐标
  QPointF getCenterPos() const;

//   PinType type() const { return m_type; }
  Node *getNode() const { return m_parentNode; }
  bool isConnected() const { return m_isConnected; }
  void setConnected(bool connected) { m_isConnected = connected; }
  QString getName() const { return m_name; }

private:
  QString m_name;
  PinType m_type;
  Node *m_parentNode;
  bool m_isConnected;
};

}

#endif // PIN_H