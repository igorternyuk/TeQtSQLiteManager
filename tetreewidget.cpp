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

    mActionRemoveTable = new QAction(QIcon(":icons/removeTable.png"),
                                  "Remove table", this);

    mActionRemoveDatabase = new QAction(QIcon(":icons/database_remove.png"),
                                  "Remove database", this);

    mActionSelectFrom = new QAction(QIcon(":icons/showTable.png"),
                                  "Show table", this);
    connect(this, &TeTreeWidget::itemClicked, [&](QTreeWidgetItem *, int)
    {
        emit selectionChanged();
    });

    connect(mActionNewTable, &QAction::triggered, [&]() {
        emit newTable();
    });
    connect(mActionRemoveTable, &QAction::triggered, [&]() {
        emit removeTable();
    });
    connect(mActionRemoveDatabase, &QAction::triggered, [&]() {
        emit removeDatabase();
    });
    connect(mActionSelectFrom, &QAction::triggered, [&]() {
        emit selectFrom();
    });

    this->addActions({ mActionNewTable, mActionRemoveTable,
                       mActionRemoveDatabase, mActionSelectFrom });
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
