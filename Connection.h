// connection.h
#ifndef CONNECTION_H
#define CONNECTION_H

#include "pin.h"
#include <QGraphicsPathItem>

namespace inv
{

class Connection : public QGraphicsPathItem {
public:
  Connection(Pin *startPin, Pin *endPin, QGraphicsItem *parent = nullptr);

  void updatePath();

  Pin *getStartPin() const { return m_startPin; }
  Pin *getEndPin() const { return m_endPin; }

private:
  Pin *m_startPin;
  Pin *m_endPin;
};

}

#endif // CONNECTION_H