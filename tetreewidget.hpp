#ifndef TETREEWIDGET_H
#define TETREEWIDGET_H

#include <QTreeWidget>
class QAction;

class TeTreeWidget : public QTreeWidget
{
public:
    explicit TeTreeWidget(QWidget *parent = nullptr);
private:
    QAction *mActionNewTable;
};

#endif // TETREEWIDGET_H
