// node.cpp
#include "node.h"
#include "IdeCompletionPopup.h"
#include "QApplication"
#include "QGraphicsProxyWidget"
#include "QGraphicsScene"
#include "QGraphicsSceneMouseEvent"
#include "QGraphicsView"
#include "WsfParseRule.hpp"
#include "connection.h"
#include "iostream"
#include "pin.h"
#include "qobjectdefs.h"
#include "wsf_parser/source/WsfParseRule.hpp"
#include <QCompleter>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

static constexpr int CORNER_RADIUS = 8;
static constexpr int PIN_SPACING = 25;
static constexpr int HEADER_HEIGHT = 30;
static constexpr int MIN_WIDTH = 120;
static constexpr int MIN_HEIGHT = 60;

namespace inv {

Node::Node(const QString &title, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_title(title) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setZValue(1);

    // 初始化矩形大小
    m_rect = QRectF(0, 0, MIN_WIDTH, MIN_HEIGHT);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

Node::~Node() {
    // 清理连线
    qDeleteAll(m_connections);
}

QRectF Node::boundingRect() const { return m_rect.adjusted(-10, -10, 10, 10); }

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    // --- 1. 设置背景色 (增加透明度) ---
    QColor bgColor;
    if (option->state & QStyle::State_Selected) {
        bgColor = QColor(70, 70, 100, 180); // 选中背景：稍微不透明一点
    } else {
        bgColor = QColor(50, 50, 50, 150); // 默认背景：更透明一点
    }
    painter->setBrush(bgColor);

    // --- 2. 设置边框 (紫色 + 透明度) ---
    QPen borderPen;
    if (option->state & QStyle::State_Selected) {
        // 紫色边框，带一点透明度 (最后的 200)
        borderPen.setColor(
            QColor(147, 112, 219, 225)); // 使用"中紫色" (MediumPurple)
        borderPen.setWidth(2);
    } else {
        borderPen.setColor(QColor(100, 100, 100, 150)); // 灰色边框，半透明
        borderPen.setWidth(1);
    }
    painter->setPen(borderPen);

    // --- 3. 绘制主矩形 ---
    painter->drawRoundedRect(m_rect, CORNER_RADIUS, CORNER_RADIUS);

    // --- 4. 绘制标题栏 ---
    // 标题栏颜色也可以稍微透明，或者保持稍亮
    QColor headerColor(40, 120, 200, 200);
    painter->setBrush(headerColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(m_rect.x(), m_rect.y(), m_rect.width(),
                             HEADER_HEIGHT, CORNER_RADIUS, CORNER_RADIUS);

    // --- 5. 绘制标题文字 ---
    painter->setPen(Qt::white);
    painter->drawText(m_rect.adjusted(10, 5, -10, HEADER_HEIGHT - 5),
                      Qt::AlignLeft, m_title);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange || change == ItemScenePositionHasChanged) {
        updateConnections();
    }
    return QGraphicsItem::itemChange(change, value);
}

Pin *Node::addInputPin(const QString &name) {
    Pin *pin = new Pin(this, name, Pin::Input);
    m_inputPins.append(pin);
    updateLayout();
    return pin;
}

Pin *Node::addOutputPin(const QString &name) {
    Pin *pin = new Pin(this, name, Pin::Output);
    m_outputPins.append(pin);
    updateLayout();
    return pin;
}

void Node::updateLayout() {
    qreal y = HEADER_HEIGHT + 15;
    qreal xInput = 10;
    qreal xOutput = m_rect.width() - 10;

    // 修复 1: 使用标准字体度量，而不是未声明的 'font'
    // 声明字体度量对象
    QFontMetrics fm(QApplication::font());

    // 计算最大宽度
    qreal maxWidth = MIN_WIDTH;

    // 布局输入引脚
    for (Pin *pin : qAsConst(m_inputPins)) {
        pin->setPos(xInput, y);
        // 修复 2: 使用 getName() 获取名字宽度
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(pin->getName()) + 30.0);
        y += PIN_SPACING;
    }

    // 布局输出引脚
    y = HEADER_HEIGHT + 15;
    for (Pin *pin : qAsConst(m_outputPins)) {
        pin->setPos(xOutput, y);
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(pin->getName()) + 30.0);
        y += PIN_SPACING;
    }

    // 修复 3: 强制转换 MIN_HEIGHT 为 qreal，或者直接使用浮点数
    // 解决 'qMax' 类型冲突 (int vs qreal)
    qreal newHeight = qMax(static_cast<qreal>(MIN_HEIGHT), y);

    m_rect.setSize(QSizeF(maxWidth, newHeight));
}

void Node::addConnection(Connection *conn) {
    if (conn) {
        m_connections.insert(conn);
    }
}

void Node::removeConnection(Connection *conn) { m_connections.remove(conn); }

void Node::updateConnections() {
    for (Connection *conn : qAsConst(m_connections)) {
        conn->updatePath();
    }
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    emit ItemClicked(this);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // // 1. 清理旧的弹窗
    // if (m_currentPopup) {
    //   m_currentPopup->deleteLater();
    //   m_currentPopup = nullptr;
    // }

    // // 2. 创建第一级弹窗
    // m_currentPopup = new IdeCompletionPopup();
    // m_currentPopup->setOptions({"变量", "函数", "类", "导入"});

    // // 3. 连接选择信号
    // connect(m_currentPopup, &IdeCompletionPopup::optionSelected, this,
    //         &Node::onOptionSelected);

    // // 4. 计算弹出位置（相对于场景）
    // QPointF scenePos = mapToScene(event->pos());
    // auto view = scene()->views().first();
    // QPoint globalPos = view->mapToGlobal(view->mapFromScene(scenePos));

    // // 5. 显示弹窗
    // m_currentPopup->showAt(globalPos);
}

void Node::onOptionSelected(const QString &text) {
    if (text == "函数") {
        showSecondLevelPopup();
    } else if (text == "变量") {
        // ...
    }
}

void Node::showSecondLevelPopup() {
    // 创建第二级弹窗
    auto *popup = new IdeCompletionPopup();
    popup->setOptions({"funcA", "funcB", "funcC"});

    // connect(popup, &IdeCompletionPopup::optionSelected, this,
    //         [this](const QString &text) {
    //           // 处理最终选择
    //           qDebug() << "最终选择：" << text;
    //           // 可以在这里更新节点文本
    //           setLabel(text);
    //         });

    // 获取当前弹窗位置，显示在它下方
    QPoint pos = m_currentPopup->pos();
    pos.setY(pos.y() + m_currentPopup->height());
    popup->showAt(pos);
}
} // namespace inv