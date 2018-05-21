#include "tetreewidget.hpp"
#include <QAction>

TeTreeWidget::TeTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    mActionNewTable = new QAction(QIcon(":icons/newTable.png"),
                                  "New table", this);
    this->addActions({ mActionNewTable });
}
