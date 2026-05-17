// graphwidget.h
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>

namespace inv {

class GraphWidget : public QGraphicsView {
  Q_OBJECT

 public:
  GraphWidget(QWidget *parent = nullptr);

 protected:
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private:
  bool m_isPanning = false;
  QPoint m_lastPanPoint;
};

}  // namespace inv

#endif  // GRAPHWIDGET_H