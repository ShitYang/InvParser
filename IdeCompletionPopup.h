// IdeCompletionPopup.h
#include <QKeyEvent>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace inv
{

class IdeCompletionPopup : public QWidget {
  Q_OBJECT
public:
  explicit IdeCompletionPopup(QWidget *parent = nullptr);

  void setOptions(const QStringList &options);
  void showAt(const QPoint &globalPos);

signals:
  void optionSelected(const QString &text);

private slots:
  void onItemClicked(QListWidgetItem *item);

private:
  QListWidget *m_listWidget;
};

}