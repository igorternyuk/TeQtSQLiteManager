#include "tetreewidget.hpp"
#include <QAction>
#include <QMouseEvent>
#include <QDebug>

TeTreeWidget::TeTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    mActionNewTable = new QAction(QIcon(":icons/newTable.png"),
                                  "New table", this);
    mActionNewTable->setDisabled(true);
    this->addActions({ mActionNewTable });

    connect(this, &TeTreeWidget::itemClicked, [&](QTreeWidgetItem *item, int)
    {
        if(!item) return;
        if(!item->parent())
            mActionNewTable->setEnabled(true);
        else
            mActionNewTable->setEnabled(false);
    });

    connect(mActionNewTable, &QAction::triggered, [&]() { emit newTable(); });
}

void TeTreeWidget::setNewTableActionEnabled(bool enabled)
{
    mActionNewTable->setEnabled(enabled);
}

/*void TeTreeWidget::mousePressEvent(QMouseEvent *event)
{
    auto currItem = currentItem();
    if(!currItem)
    {
        mActionNewTable->setEnabled(false);
    }
    else
    {
        if(currItem->parent())
            mActionNewTable->setEnabled(false);
        else
            mActionNewTable->setEnabled(true);
    }
    QTreeWidget::mousePressEvent(event);
}*/
