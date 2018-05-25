#include "addnewtabledialog.hpp"
#include "ui_addnewtabledialog.h"
#include <QAction>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QMessageBox>
#ifdef DEBUG
#include <QDebug>
#endif

AddNewTableDialog::AddNewTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewTableDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    mActionNewField = new QAction(QIcon(":icons/addField"), "New field", this);
    mActionRemoveField = new QAction(QIcon(":icons/close2"), "Remove field",
                                     this);

    auto createCombo = [&](QWidget *parent = nullptr){
        QComboBox *combo = new QComboBox(parent);
        combo->addItems({"INTEGER", "DOUBLE", "REAL",
                         "NUMERIC", "TEXT", "DATE",
                         "TIME", "BOOL", "BLOB"});
        return combo;
    };

    auto createRadioButton = [&](QWidget *parent = nullptr){
        QRadioButton *radio = new QRadioButton(parent);
        return radio;
    };

    auto createCheckBox = [&](QWidget *parent = nullptr){
        QCheckBox *checkBox = new QCheckBox(parent);
        return checkBox;
    };

    connect(mActionNewField, &QAction::triggered, [&](){
        int lastRowIndex = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(lastRowIndex);
        ui->tableWidget->setCellWidget(lastRowIndex, TYPE, createCombo());
        ui->tableWidget->setCellWidget(lastRowIndex, CHECK, createCheckBox());
        ui->tableWidget->setCellWidget(lastRowIndex, PK, createRadioButton());
        ui->tableWidget->setCellWidget(lastRowIndex, AI, createCheckBox());
        ui->tableWidget->setCellWidget(lastRowIndex, NN, createCheckBox());
        ui->tableWidget->setCellWidget(lastRowIndex, UNIQUE, createCheckBox());
    });

    connect(mActionRemoveField, &QAction::triggered, [&](){
        ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    });

    ui->tableWidget->addActions({ mActionNewField, mActionRemoveField});
}

AddNewTableDialog::~AddNewTableDialog()
{
    delete ui;
}

void AddNewTableDialog::on_btnAddField_clicked()
{
    mActionNewField->trigger();
}

void AddNewTableDialog::on_btnRemoveField_clicked()
{
    mActionRemoveField->trigger();
}

void AddNewTableDialog::on_btnAccept_clicked()
{
    auto dbName = ui->lineEditTableName->text();
    if(dbName.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Table name not specified");
        return;
    }
    mQuery = QString("CREATE TABLE IF NOT EXISTS %1 (\n").arg(dbName);
    int rowCount = ui->tableWidget->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
        auto fieldNameItem = ui->tableWidget->item(i, FIELD_NAME);
        if(!fieldNameItem)
        {
            QMessageBox::warning(this, "Warning",
                                 QString(" %1-th field name not specified")
                                 .arg(i + 1));
            return;
        }
        QString fieldName = fieldNameItem->text();
        QString type = static_cast<QComboBox*>(
                    ui->tableWidget->cellWidget(i, TYPE))->currentText();
        mQuery += QString("%1 %2 ").arg(fieldName).arg(type);
        auto defaultValItem = ui->tableWidget->item(i, DEFAULT_VAL);
        if(defaultValItem)
            mQuery += QString("DEFAULT %1 ").arg(defaultValItem->text());
        bool isChecked = static_cast<QCheckBox*>(
                    ui->tableWidget->cellWidget(i, CHECK))->isChecked();
        if(isChecked)
            mQuery += "CHECK(1) ";
        bool isPrimaryKey = static_cast<QRadioButton*>(
                    ui->tableWidget->cellWidget(i, PK))->isChecked();
        if(isPrimaryKey)
            mQuery += "PRIMARY KEY ";
        bool isAutoincrement = static_cast<QCheckBox*>(
                    ui->tableWidget->cellWidget(i, AI))->isChecked();
        if(isAutoincrement)
            mQuery += "AUTOINCREMENT ";
        bool isUnique = static_cast<QCheckBox*>(
                    ui->tableWidget->cellWidget(i, UNIQUE))->isChecked();
        if(isUnique)
            mQuery += "UNIQUE ";
        auto foreignKeyItem = ui->tableWidget->item(i, FK);
        if(foreignKeyItem)
            mQuery += QString("FOREIGN KEY(%1) REFERENCES %2")
                    .arg(fieldName).arg(foreignKeyItem->text());
        mQuery += ",";
    }

    int lastIndex = mQuery.size() - 1;
    mQuery.replace(lastIndex, 1, "\n");
    mQuery += ");";
    accept();
}

void AddNewTableDialog::on_btnCancel_clicked()
{
    reject();
}

QString AddNewTableDialog::query() const
{
    return mQuery;
}
