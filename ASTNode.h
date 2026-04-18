#ifndef ASTNODE_H
#define ASTNODE_H

#include "WsfParseNode.hpp"
#include <QString>
#include <QVector>
#include <memory>
#include <string>
#include <vector>

#include "iostream"

namespace inv {

class AstNode {
public:
  AstNode(WsfParseNode *aNode) : mNode(aNode) {
    // std::cout << mNode->mType;
    if (mNode->mType)
    {
      name = mNode->mType;
    }
  };

  AstNode(const std::string &n, const std::string &t) : name(n), type(t) {}

  ~AstNode() {
    for (auto child : children) {
      delete child;
    }
  }

  std::string GetNodeString()
  {
    return WsfParseNode::GetNodeString(mNode);
  }

  std::string name {"NULL"};
  std::string type;
  std::vector<AstNode *> children;

  // 模拟获取子节点的接口
  const std::vector<AstNode *> &getChildren() const { return children; }

  WsfParseNode *mNode;
};

} // namespace inv
#endif