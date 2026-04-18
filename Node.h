// node.h
#ifndef NODE_H
#define NODE_H

#include "ASTNode.h"
#include "qcombobox.h"
#include "qobject.h"
#include "qobjectdefs.h"
#include <QGraphicsItem>
#include <QList>
#include <QSet>
#include <QStandardItemModel>
#include <QVector>
#include "QCompleter"
#include "qwidget.h"

namespace inv
{

class IdeCompletionPopup;
class Pin;
class Connection;

class Node : public QObject, public QGraphicsItem {

  Q_OBJECT
public:
  Node(const QString &title, QGraphicsItem *parent = nullptr);
  ~Node();

  // QGraphicsItem interface
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

  // 节点操作
  Pin *addInputPin(const QString &name);
  Pin *addOutputPin(const QString &name);

  // 获取引脚（用于调试或连接）
  QList<Pin *> getInputPins() const { return m_inputPins; }
  QList<Pin *> getOutputPins() const { return m_outputPins; }

  // 连线管理
  void addConnection(Connection *conn);
  void removeConnection(Connection *conn);

signals:
  void ItemClicked(Node *aNode);

protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
  void updateLayout();
  void updateConnections();
  QAbstractItemModel *createIdeSuggestionModel(const QString &context);

  void showSecondLevelPopup();

  void onOptionSelected(const QString &text);

private slots :
    // void onCompletionSelected(const QModelIndex &index);

private:
  QString m_title;
  QRectF m_rect;
  QList<Pin *> m_inputPins;
  QList<Pin *> m_outputPins;
  QSet<Connection *> m_connections;  // 使用 QSet 便于快速增删

  IdeCompletionPopup *m_currentPopup{nullptr};
};
}

#endif // NODE_H