#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QMessageBox>

using TreeItem = QTreeWidgetItem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->layout()->setMargin(0);
    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    int reply = QMessageBox::question(this, "Confirm quit, please",
                                      "Do you really want to quit?",
                                      QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        close();
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_actionOpen_database_triggered()
{
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("Databases (*.db);;All files(*.*)");
    auto databaseName = QFileDialog::getOpenFileName(this, "Choose database",
                                                 initDir, filter);
    if(databaseName.isEmpty())
        return;
    auto databaseItem = new TreeItem(ui->treeWidget);
    databaseItem->setText(0, databaseName);
    databaseItem->setIcon(0, QIcon(":icons/database3"));
    ui->treeWidget->addTopLevelItem(databaseItem);
}

void MainWindow::on_actionSave_database_triggered()
{

}

void MainWindow::on_actionOpen_SQL_script_triggered()
{

}

void MainWindow::on_actionSave_SQL_script_triggered()
{

}

void MainWindow::on_actionPrint_triggered()
{

}

void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::on_actionExecute_triggered()
{

}

void MainWindow::on_actionPreferences_triggered()
{

}

void MainWindow::on_actionManual_triggered()
{

}

void MainWindow::on_actionAbout_program_triggered()
{
    QMessageBox::about(this, "About program",
                       "Author: igorternyuk\n"
                       "Company: IgorTernyukCorporation\n"
                       "Date:2018-05-19\n"
                       "Software:TeSQLiteMan");
}

void MainWindow::on_actionFind_and_Replace_triggered()
{

}
