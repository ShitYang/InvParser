// AstConverter.h
#ifndef ASTCONVERTER_H
#define ASTCONVERTER_H

#include "WsfParser.hpp"
#include "memory"
#include "qobjectdefs.h"
#include "wsf_parser/source/WsfParseRule.hpp"
#include <QGraphicsScene>
#include <QMap>
#include <QObject>
#include <string>

namespace inv {

class Node;
class Connection;
class AstNode; // 假设这是你的 AST 节点基类

class AstConverter : public QObject {
    Q_OBJECT

  public:
    explicit AstConverter(QGraphicsScene *scene, QObject *parent = nullptr);

    // 核心函数：将根节点转换为图形
    void convert(AstNode *root);
    ~AstConverter() { delete mRoot; }

  public slots:
    void NodeClicked(Node *aNode);

  signals:
    void SendMessage(const std::string &aMsg);

  private:
    void createNodeRecursive(AstNode *astNode);
    void connectNodeRecursive(AstNode *astNode);
    void LayoutNode(AstNode *root, QPointF aPos);

    std::string GetRuleString(WsfParseRule *aRule);

    QGraphicsScene *m_scene;

    // 用于记录 AST 节点和图形节点的映射关系
    QMap<AstNode *, Node *> m_astToNodeMap;
    QMap<Node *, AstNode *> m_nodeToAstMap;

    AstNode *mRoot{nullptr};
};

} // namespace inv

#endif // ASTCONVERTER_H