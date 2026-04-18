// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <string>
#include "ASTNode.h"
#include "memory"
#include "QTextBrowser" 
#include "QDockWidget"
#include "qtextbrowser.h"

class WsfParseNode;

namespace inv 
{

class GraphWidget;

class AstConverter;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void SetASTTree(WsfParseNode *aTree);
  AstNode *DFSTraverseAST(WsfParseNode *aCurrent, WsfParseNode *aParent);

private slots:
  void ReceiveMessage(const std::string &aMsg);

private:
  void setupUI();

  GraphWidget *m_graphView;
  QGraphicsScene *m_scene;
  AstConverter *m_converter;
  QTextBrowser *mTextBrowser;

};

}

#endif // MAINWINDOW_H