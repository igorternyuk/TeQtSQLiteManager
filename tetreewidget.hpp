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
    void removeTable();
    void removeDatabase();
    void selectFrom();
    void selectionChanged();
private:
    QAction *mActionNewTable;
    QAction *mActionRemoveTable;
    QAction *mActionRemoveDatabase;
    QAction *mActionSelectFrom;
};

#endif // TETREEWIDGET_H
