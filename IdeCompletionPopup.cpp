// IdeCompletionPopup.cpp
#include "IdeCompletionPopup.h"
#include "qapplication.h"
#include "QDesktopWidget"

namespace inv 
{

IdeCompletionPopup::IdeCompletionPopup(QWidget *parent)
    : QWidget(parent, Qt::Popup) { // Qt::Popup 保证点击外部自动关闭
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose); // 用完自动销毁

  // 样式：像 VS Code 或 PyCharm 的提示框
  setStyleSheet(R"(
        QWidget {
            background-color: #282c34; /* 深色背景 */
            border: 1px solid #3e4452;
            border-radius: 4px;
            color: #abb2bf;
            font-family: Consolas, 'Courier New', monospace;
            font-size: 12px;
        }
        QListWidget {
            border: none;
            outline: none;
            show-decoration-selected: 0;
        }
        QListWidget::item {
            padding: 4px 8px;
            border: none;
        }
        QListWidget::item:selected {
            background-color: #3e4452;
            color: #ffffff;
        }
    )");

  m_listWidget = new QListWidget(this);
  m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_listWidget);

  connect(m_listWidget, &QListWidget::itemClicked, this,
          &IdeCompletionPopup::onItemClicked);
}

void IdeCompletionPopup::setOptions(const QStringList &options) {
  m_listWidget->clear();
  m_listWidget->addItems(options);
  if (!options.isEmpty()) {
    m_listWidget->setCurrentRow(0); // 默认选中第一项
  }
}

void IdeCompletionPopup::showAt(const QPoint &globalPos) {
  // 自动调整大小，不超过屏幕宽度的 1/3
  int width = qMin(300, qApp->desktop()->screenGeometry(this).width() / 3);
  int height = qMin(400, m_listWidget->count() * 20 + 20);
  resize(width, height);
  move(globalPos);
  show();
  m_listWidget->setFocus();
}

void IdeCompletionPopup::onItemClicked(QListWidgetItem *item) {
  emit optionSelected(item->text());
  close();
}

// 可选：支持键盘上下键和回车
// void IdeCompletionPopup::keyPressEvent(QKeyEvent *event) {
//   switch (event->key()) {
//   case Qt::Key_Up:
//     if (m_listWidget->currentRow() > 0) {
//       m_listWidget->setCurrentRow(m_listWidget->currentRow() - 1);
//     }
//     break;
//   case Qt::Key_Down:
//     if (m_listWidget->currentRow() < m_listWidget->count() - 1) {
//       m_listWidget->setCurrentRow(m_listWidget->currentRow() + 1);
//     }
//     break;
//   case Qt::Key_Enter:
//   case Qt::Key_Return:
//     if (m_listWidget->currentItem()) {
//       emit optionSelected(m_listWidget->currentItem()->text());
//       close();
//     }
//     break;
//   case Qt::Key_Escape:
//     close();
//     break;
//   default:
//     QWidget::keyPressEvent(event);
//   }
// }

}