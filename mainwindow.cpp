// MainWindow.cpp
#include "MainWindow.h"
#include "ASTConverter.h"
#include "ASTNode.h"
#include "GraphWidget.h"
#include "qchar.h"
#include "wsf_parser/source/WsfParseNode.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <string>

namespace inv {

static const std::string TWO_BREAK_LINE = "<br></br> <br></br>";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_graphView(new GraphWidget(this)),
      m_scene(new QGraphicsScene(this)),
      m_converter(new AstConverter(m_scene, this)) {
    setupUI();

    // 设置视图为场景
    m_graphView->setScene(m_scene);

    // 自动适应大小
    m_graphView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);

    // 1. 创建一个 QTextBrowser 作为内容部件
    mTextBrowser = new QTextBrowser(this);
    mTextBrowser->setOpenExternalLinks(true); // 可选：允许打开外部链接
    mTextBrowser->setReadOnly(true);          // 设置为只读

    // 2. 创建 QDockWidget
    QDockWidget *dock = new QDockWidget(tr("节点属性"), this);
    // dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(mTextBrowser); // 将文本浏览器设置为 Dock 的中心部件

    this->addDockWidget(Qt::RightDockWidgetArea, dock);

    connect(m_converter, &AstConverter::SendMessage, this,
            &MainWindow::ReceiveMessage);
}

void MainWindow::SetASTTree(WsfParseNode *aTree) {
    AstNode *aAst = DFSTraverseAST(aTree, nullptr);
    m_converter->convert(aAst);
}

AstNode *MainWindow::DFSTraverseAST(WsfParseNode *aCurrent,
                                    WsfParseNode *aParent) {

    if (!aCurrent) {
        return nullptr;
    }

    AstNode *aRet = new AstNode(aCurrent);
    auto firstChild = aCurrent->Down();
    if (firstChild) {
        for (; firstChild->Left(); firstChild = firstChild->Left())
            ;
        for (; firstChild; firstChild = firstChild->Right()) {
            auto res = DFSTraverseAST(firstChild, aCurrent);
            if (res) {
                aRet->children.push_back(std::move(res));
            }
        }
    }

    return aRet;
}

void MainWindow::setupUI() {

    resize(1200, 800);
    // 设置中央部件和布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建一个按钮用于刷新（可选）
    layout->addWidget(m_graphView);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::ReceiveMessage(const std::string &aMsg) {
    std::string content = "<b>" + aMsg + "</b>" + TWO_BREAK_LINE;
    mTextBrowser->insertHtml(QString::fromStdString(content));
}

} // namespace inv