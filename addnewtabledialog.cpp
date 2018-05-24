#include "addnewtabledialog.hpp"
#include "ui_addnewtabledialog.h"
#include <QAction>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>

AddNewTableDialog::AddNewTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewTableDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actionNewField = new QAction(this);
    QAction *actionRemoveField = new QAction(this);

    auto createCombo = [&](QWidget *parent = nullptr){
        QComboBox *combo = new QComboBox(parent);
        combo->addItems({"INTEGER", "DOUBLE", "REAL", "NUMERIC", "TEXT", "DATE",
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

    connect(actionNewField, &QAction::triggered, [&](){

    });

    connect(actionRemoveField, &QAction::triggered, [&](){

    });
}

AddNewTableDialog::~AddNewTableDialog()
{
    delete ui;
}

void AddNewTableDialog::on_btnAddField_clicked()
{

}

void AddNewTableDialog::on_btnRemoveField_clicked()
{

}

void AddNewTableDialog::on_btnAccept_clicked()
{

}

void AddNewTableDialog::on_btnCancel_clicked()
{

}
