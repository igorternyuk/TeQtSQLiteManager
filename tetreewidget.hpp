#ifndef TETREEWIDGET_H
#define TETREEWIDGET_H

#include <QTreeWidget>
class QAction;

class TeTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TeTreeWidget(QWidget *parent = nullptr);
    void setNewTableActionEnabled(bool enabled = true);
signals:
    void newTable();
/*protected:
    void mousePressEvent(QMouseEvent *event);*/
private:
    QAction *mActionNewTable;
};

#endif // TETREEWIDGET_H
