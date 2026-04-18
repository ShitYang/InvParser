// AstConverter.cpp
#include "AstConverter.h"

#include <QRandomGenerator>
#include <cstddef>
#include <memory>
#include <string>

#include "ASTConverter.h"
#include "ASTNode.h"
#include "WsfParseAlternate.hpp"
#include "WsfParseBasicRules.hpp"
#include "WsfParseRecurrence.hpp"
#include "WsfParseSequence.hpp"
#include "connection.h"
#include "node.h"
#include "wsf_parser/source/WsfParseAlternate.hpp"
#include "wsf_parser/source/WsfParseRule.hpp"

namespace inv {

AstConverter::AstConverter(QGraphicsScene *scene, QObject *parent)
    : QObject(parent), m_scene(scene) {}

void AstConverter::convert(AstNode *root) {
    mRoot = root;

    m_astToNodeMap.clear();
    m_scene->clear();

    if (!root)
        return;

    // 第一步：创建所有节点
    createNodeRecursive(root);

    LayoutNode(root, {0.0, 0.0});

    // 第二步：连接所有节点
    connectNodeRecursive(root);
}

void AstConverter::createNodeRecursive(AstNode *astNode) {
    if (!astNode)
        return;

    // --- 创建图形节点 ---
    Node *graphicsNode =
        new Node(QString::fromLocal8Bit(astNode->name.c_str()));
    m_scene->addItem(graphicsNode);

    connect(graphicsNode, &Node::ItemClicked, this, &AstConverter::NodeClicked);

    // 随机分配一点位置，避免重叠
    graphicsNode->setPos(QRandomGenerator::global()->bounded(200),
                         QRandomGenerator::global()->bounded(200));

    // --- 保存映射关系 ---
    m_astToNodeMap[astNode] = graphicsNode;
    m_nodeToAstMap[graphicsNode] = astNode;

    // --- 根据 AST 节点类型创建引脚 ---
    // 这里只是一个示例逻辑
    //   if (astNode->type == "Function") {
    //     graphicsNode->addOutputPin("Return");
    //   } else {
    graphicsNode->addInputPin("In");
    graphicsNode->addOutputPin("Out");
    //   }

    // 递归处理子节点
    for (auto &child : astNode->getChildren()) {
        createNodeRecursive(child);
    }
}

void AstConverter::connectNodeRecursive(AstNode *astNode) {
    if (!astNode)
        return;

    Node *parentNode = m_astToNodeMap[astNode];

    // 简单逻辑：将父节点的输出连接到子节点的输入
    for (auto &childAst : astNode->getChildren()) {
        Node *childNode = m_astToNodeMap[childAst];

        if (parentNode && childNode) {
            // 获取父节点的第一个输出引脚
            Pin *outPin = parentNode->getOutputPins()[0];
            // 获取子节点的第一个输入引脚
            Pin *inPin = childNode->getInputPins()[0];

            if (outPin && inPin) {
                Connection *conn = new Connection(outPin, inPin);
                m_scene->addItem(conn);

                // 可选：注册连线到节点（如果 Node 类有管理连线的逻辑）
                parentNode->addConnection(conn);
                childNode->addConnection(conn);
            }
        }
    }

    // 递归连接子树
    for (auto &child : astNode->getChildren()) {
        connectNodeRecursive(child);
    }
}

void AstConverter::LayoutNode(AstNode *root, QPointF aPos) {
    if (root && m_astToNodeMap.find(root) != m_astToNodeMap.end()) {
        m_astToNodeMap.find(root).value()->setPos(aPos.rx(), aPos.ry());
    }
    // 计算子节点的位置
    size_t childCount = root->getChildren().size();
    double startY = aPos.y() - (childCount * 100.0) / 2; // 居中

    for (int i = 0; i < childCount; ++i) {
        auto childAst = root->getChildren()[i];
        auto childNode = m_astToNodeMap.find(childAst); // 获取对应的图形节点

        if (childNode != m_astToNodeMap.end()) {
            // 水平排列子节点
            QPointF childPos(aPos.x() + 250, startY + i * 100);
            // 递归布局孙子节点
            LayoutNode(childAst, childPos);
        }
    }
}

void AstConverter::NodeClicked(Node *aNode) {
    auto node = m_nodeToAstMap[aNode]->mNode;
    // emit SendMessage(m_nodeToAstMap[aNode]->GetNodeString());

    auto rule = m_nodeToAstMap[aNode]->mNode->mRulePtr;
    std::string res = GetRuleString(rule);
    emit SendMessage(res);
    // if (rule && rule->Type() == WsfParseRule::cALTERNATE) {
    //     WsfParseAlternate *altenate = static_cast<WsfParseAlternate *>(rule);
    //     short index = 1;
    //     for (auto rule : altenate->Alternates()) {
    //         emit SendMessage(std::to_string(index) + " : " +
    //                          rule->GetRuleDescription());
    //         index++;
    //     }
    // }
}

std::string AstConverter::GetRuleString(WsfParseRule *aRule) {

    if (!aRule) {
        return "";
    }

    std::string res = "";
    switch (aRule->Type()) {
    case WsfParseRule::cLITERAL: {
        res += static_cast<WsfParseLiteral *>((aRule))->mText;
        break;
    }
    case WsfParseRule::cINT: {
        res += "[INT]";
        break;
    }
    case WsfParseRule::cREAL: {
        res += "[REAL]";
        break;
    }
    case WsfParseRule::cSTRING: {
        res += "[String]";
        break;
    }
    case WsfParseRule::cSEQUENCE: {
        auto seq = aRule->GetSequence();
        for (const auto &rule : seq) {
            res = res + " " + GetRuleString(rule);
        }
        break;
    }
    case WsfParseRule::cVALUE: {
        WsfParseValue *value = static_cast<WsfParseValue *>(aRule);
        if (value->GetSubordinateRule()) {
            res += GetRuleString(value->GetSubordinateRule());
        }
        break;
    }
    case WsfParseRule::cSTRUCT: {
        WsfParseStruct *stru = static_cast<WsfParseStruct *>(aRule);
        if (stru->GetSubordinateRule()) {
            res += GetRuleString(stru->GetSubordinateRule());
        }
        break;
    }
    case WsfParseRule::cNAMED_RULE: {
        WsfParseNamedRule *named = static_cast<WsfParseNamedRule *>(aRule);
        if (named->GetSubordinateRule()) {
            res += GetRuleString(named->GetSubordinateRule());
        }
        break;
    }
    case WsfParseRule::cRULE_REFERENCE: {
        WsfParseRuleReference *wrr =
            static_cast<WsfParseRuleReference *>(aRule);
        if (wrr->GetRulePtr()) {
            res += GetRuleString(wrr->GetRulePtr());
        }
        break;
    }
    case WsfParseRule::cRECURRENCE: {
        WsfParseRecurrence *rec = static_cast<WsfParseRecurrence *>(aRule);
        if (rec->GetSubordinateRule()) {
            res +=
                "[ " + GetRuleString(rec->GetSubordinateRule()) + " " + "[*] ]";
        }
        break;
    }
    case WsfParseRule::cALTERNATE: {
        WsfParseAlternate *alternate = static_cast<WsfParseAlternate *>(aRule);
        if (alternate) {
            res += " [ ";
            for (const auto &rule : alternate->Alternates()) {
                res += GetRuleString(rule);
                if (rule != alternate->Alternates().back()) {
                    res += " || ";
                }
            }
            res += " ] ";
        }
        break;
    }
    case WsfParseRule::cTYPE_COMMAND: {
        res += "[ TypeCommand ]";
        break;
    }
        // break;
    }

    return res;
}

} // namespace inv