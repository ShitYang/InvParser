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
  // 缩放事件
  void wheelEvent(QWheelEvent *event) override;
  // 键盘删除事件
  void keyPressEvent(QKeyEvent *event) override;
};

}  // namespace inv

#endif  // GRAPHWIDGET_H