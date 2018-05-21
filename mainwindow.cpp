#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "settings_util.hpp"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>

#define DB_DRIVER "QSQLITE"

using TreeItem = QTreeWidgetItem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->layout()->setMargin(0);
    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    mDatabase = QSqlDatabase::addDatabase(DB_DRIVER);

    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
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
    mDatabase.setDatabaseName(databaseName);
    if(!mDatabase.open())
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not open database: %1")
                              .arg(databaseName));
    }
}

void MainWindow::on_actionSave_database_triggered()
{
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("Databases (*.db);;All files(*.*)");
    auto databaseName = QFileDialog::getSaveFileName(this, "Select destination",
                                                 initDir, filter);
    if(databaseName.isEmpty())
        return;
    /*auto databaseItem = new TreeItem(ui->treeWidget);
    databaseItem->setText(0, databaseName);
    databaseItem->setIcon(0, QIcon(":icons/database3"));
    ui->treeWidget->addTopLevelItem(databaseItem);
    mDatabase.setDatabaseName(databaseName);
    if(!mDatabase.open())
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not open database: %1")
                              .arg(databaseName));
    }*/

}

void MainWindow::on_actionOpen_SQL_script_triggered()
{
    /*auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("SQL scripts (*.sql);;All files (*.*)");
    auto filePath = QFileDialog::getOpenFileName(this, "Choose script",
                                                 initDir, filter);
    if(filePath.isEmpty())
        return;
    QFile fi(filePath);
    if(fi.open(QFile::ReadOnly))
    {
        QTextStream stream(&fi);
        //ui->tabWidget->currentIndex();
        ui->textEdit->setText(stream.readAll());
        fi.close();
    }
    else
    {
        QMessageBox::critical(this, "Error", QString("Could not open file:")
                              .arg(filePath));
        return;
    }*/
}

void MainWindow::on_actionSave_SQL_script_triggered()
{
    /*auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("SQL scripts (*.sql);;All files(*.*)");
    auto filePath = QFileDialog::getSaveFileName(this, "Choose destination path",
                                                 initDir, filter);
    if(filePath.isEmpty())
        return;
    QFile fo(filePath);
    if(fo.open(QFile::WriteOnly))
    {
        QTextStream stream(&fo);
        stream << ui->textEdit->toPlainText();
        fo.flush();
        fo.close();
    }
    else
    {
        QMessageBox::critical(this, "Error", QString("Could not save file: %1")
                              .arg(filePath));
    }*/
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

void MainWindow::on_actionNew_SQL_script_triggered()
{

}

int MainWindow::create_new_tab(const QString &title, const QString &pathToFile,
                               const QString &text)
{

}

bool MainWindow::openFile(const QString &path)
{

}

bool MainWindow::openFiles(const QStringList &listOfFiles)
{

}

bool MainWindow::saveFile(int index)
{

}

bool MainWindow::saveFileAs(int index)
{

}

bool MainWindow::saveTextToFile(const QString &filePath, const QString &text)
{

}

void MainWindow::saveAllCurrentSessionFiles()
{

}

void MainWindow::loadAllLastSessionFiles()
{

}

void MainWindow::loadSettings()
{
    auto size = loadParameter(mSettingKeys[SettingIdentifier::WindowSize],
            mSettingGroups[SettingGroupIndentifier::MainWindowGeometry],
            this->size()).value<QSize>();
    this->resize(size);

    auto pos = loadParameter(mSettingKeys[SettingIdentifier::WindowPos],
            mSettingGroups[SettingGroupIndentifier::MainWindowGeometry],
            this->pos()).value<QPoint>();
    this->move(pos);

    bool ok = false;
    auto currTabIndex = loadParameter(mSettingKeys[SettingIdentifier::CurrentTabIndex],
            mSettingGroups[SettingGroupIndentifier::TabWidget],
            0).toInt(&ok);
    if(ok)
        ui->tabWidget->setCurrentIndex(currTabIndex);
}

void MainWindow::saveSettings()
{
    saveParameter(mSettingKeys[SettingIdentifier::WindowSize],
            this->size(),
            mSettingGroups[SettingGroupIndentifier::MainWindowGeometry]);
    saveParameter(mSettingKeys[SettingIdentifier::WindowPos],
            this->pos(),
            mSettingGroups[SettingGroupIndentifier::MainWindowGeometry]);
    saveParameter(mSettingKeys[SettingIdentifier::CurrentTabIndex],
            ui->tabWidget->currentIndex(),
            mSettingGroups[SettingGroupIndentifier::TabWidget]);
}



void MainWindow::on_actionExport_to_pdf_triggered()
{
    /*
        auto index = ui->tabWidget->currentIndex();
        if(index == -1) return;
        QString startLocation = QStandardPaths::standardLocations(
                                QStandardPaths::DocumentsLocation)
                                .value(0, QDir::homePath());
        QString filter = QString::fromStdString("PDF documents (*.pdf);;"
                                                "All files (*.*)");
        QString fileName = QFileDialog::getSaveFileName(this, "Choose a file path",
                                                        startLocation,
                                                        filter);
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        mListOfTextEdits.at(index)->document()->print(&printer);
    */

}
