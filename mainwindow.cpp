#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "settings_util.hpp"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QDebug>

#define DB_DRIVER "QSQLITE"

using TreeItem = QTreeWidgetItem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->layout()->setMargin(0);
    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tabWidget->setTabsClosable(true);
    mDatabase = QSqlDatabase::addDatabase(DB_DRIVER);

    loadAllLastSessionScripts();
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveAllCurrentSessionScripts();
    saveSettings();    
    delete ui;
}

void MainWindow::on_actionNew_database_triggered()
{

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

void MainWindow::on_actionSave_database_as_triggered()
{
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("Databases (*.db);;All files(*.*)");
    auto databaseName = QFileDialog::getSaveFileName(this, "Select destination",
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

void MainWindow::on_actionPrint_triggered()
{
    QPrintDialog dialog(this);
    if(dialog.exec() == QPrintDialog::Rejected)
        return;
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1)
        return;
    mTextEdits.at(currIndex)->print(dialog.printer());
}

void MainWindow::on_actionCopy_triggered()
{
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1) return;
    mTextEdits.at(currIndex)->copy();
}

void MainWindow::on_actionCut_triggered()
{
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1) return;
    mTextEdits.at(currIndex)->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1) return;
    mTextEdits.at(currIndex)->paste();
}


void MainWindow::on_actionUndo_triggered()
{
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1) return;
    mTextEdits.at(currIndex)->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    auto currIndex = ui->tabWidget->currentIndex();
    if(currIndex == -1) return;
    mTextEdits.at(currIndex)->redo();
}

void MainWindow::on_actionFind_and_Replace_triggered()
{

}

void MainWindow::on_actionExecute_triggered()
{

}

void MainWindow::on_actionNew_SQL_script_triggered()
{
    createNewTab(QString("Untitled%1").arg(++mUntitledTabMaxIndex), "","");
}

void MainWindow::on_actionOpen_SQL_script_triggered()
{
    auto startLocation =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("SQL scripts (*.sql);;All files (*.*)");
    auto scriptList = QFileDialog::getOpenFileNames(this, "Choose script",
                                                    startLocation, filter);
    for(const auto &path: scriptList)
    {
        if(!readScriptFromFile(path))
        {
            QMessageBox::critical(this, "Error", QString("Could not open file:")
                                  .arg(path));
        }
    }
}

void MainWindow::on_actionSave_SQL_script_triggered()
{
    saveScript(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionSave_SQL_script_as_triggered()
{
    saveScriptAs(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionSave_all_scripts_triggered()
{
    int tabCount = ui->tabWidget->count();
    for(int index{0}; index < tabCount; ++index)
        saveScript(index);
}

void MainWindow::on_actionClose_current_SQL_script_triggered()
{
    on_tabWidget_tabCloseRequested(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionClose_all_scripts_triggered()
{
    while(ui->tabWidget->count())
        on_tabWidget_tabCloseRequested(ui->tabWidget->count() - 1);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(index == -1) return;
    auto currColor = ui->tabWidget->tabBar()->tabTextColor(index);
    if(currColor == Qt::red)
    {
        auto reply = QMessageBox::question(this, "Save file?",
                         "Would you like to save script before closing?",
                         QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            saveScript(index);
        }
    }

    QWidget *widget = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete widget;
    mTextEdits.removeAt(index);
}

bool MainWindow::readScriptFromFile(const QString &path)
{
    if(path.isEmpty())
        return false;
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QFileInfo fileInfo(file);
        createNewTab(fileInfo.fileName(), path, stream.readAll());
        file.close();
        return true;
    }
    return false;
}

bool MainWindow::saveScript(int index)
{
    if(index == -1) return false;
    auto currPath = ui->tabWidget->tabBar()->tabToolTip(index);
    bool ok = false;
    if(currPath.isEmpty())
    {
        ok = saveScriptAs(index);
    }
    else
    {
        auto currScript = mTextEdits.at(index)->toPlainText();
        if(saveTextToFile(currPath, currScript))
        {
            ui->tabWidget->tabBar()->setTabTextColor(index, Qt::black);
            ok = true;
        }
        else
        {
            ok = false;
        }
    }
    return ok;
}

bool MainWindow::readScriptsFromFiles(const QStringList &listOfFiles)
{
    bool isAtLeastOneFile = false;
    for(const auto &path: listOfFiles)
        if(readScriptFromFile(path))
            isAtLeastOneFile = true;
    return isAtLeastOneFile;
}

bool MainWindow::saveScriptAs(int index)
{
    if(index == -1) return false;
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("SQL scripts (*.sql);;All files(*.*)");
    auto filePath = QFileDialog::getSaveFileName(this, "Choose destination path",
                                                 initDir, filter);
    if(filePath.isEmpty())
        return false;
    QFile fo(filePath);
    if(fo.open(QFile::WriteOnly))
    {
        QTextStream stream(&fo);
        stream << mTextEdits.at(index)->toPlainText();
        QFileInfo fileInfo(fo);
        ui->tabWidget->setTabText(index, fileInfo.fileName());
        ui->tabWidget->setTabToolTip(index, filePath);
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::black);
        fo.flush();
        fo.close();
        return true;
    }
    return false;
}

bool MainWindow::saveTextToFile(const QString &filePath, const QString &text)
{
    if(filePath.isEmpty())
        return false;
    QFile file(filePath);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream << text;
        file.flush();
        file.close();
        return true;
    }
    //TODO add error string
    return false;
}

int MainWindow::createNewTab(const QString &title, const QString &pathToFile,
                             const QString &text)
{
    QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
    textEdit->document()->setDefaultFont(QFont("Consolas", 14));
    textEdit->setText(text);
    textEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
    textEdit->addAction(ui->actionCopy);
    textEdit->addAction(ui->actionCut);
    textEdit->addAction(ui->actionPaste);
    textEdit->addAction(ui->actionUndo);
    textEdit->addAction(ui->actionRedo);
    textEdit->addAction(ui->actionClose_current_SQL_script);
    connect(textEdit, &QTextEdit::textChanged, this,
            &MainWindow::markUnsavedScriptTab);
    mTextEdits.append(textEdit);
    ui->tabWidget->addTab(textEdit, title);
    auto index = ui->tabWidget->indexOf(textEdit);
    if(!pathToFile.contains(QRegExp("(lastSession)")))
    {
        ui->tabWidget->setTabToolTip(index, pathToFile);
    }
    else
    {
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
    }
    if((text.isEmpty() && pathToFile.isEmpty()))
        ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
    ui->tabWidget->setCurrentIndex(index);
    return index;
}

void MainWindow::saveAllCurrentSessionScripts()
{
    QFile session("session.data");
    if(!QDir(".lastSession").exists())
    {
        if(!QDir().mkdir(".lastSession"))
        {
            QMessageBox::critical(this, "Error",
                                  "Could not create session folder");
            return;
        }
    }

    if(session.open(QFile::WriteOnly))
    {
        QTextStream stream(&session);
        for(int i = 0; i < ui->tabWidget->count(); ++i)
        {
            auto currPath = ui->tabWidget->tabBar()->tabToolTip(i);
            auto currText = mTextEdits.at(i)->toPlainText();
            if(currPath.isEmpty())
            {
                currPath = QString(".lastSession/%1")
                        .arg(ui->tabWidget->tabText(i));
            }
            saveTextToFile(currPath, currText);
            stream << currPath << "\n";
        }
        session.flush();
        session.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not save current session: %1")
                              .arg(session.errorString()));
    }
}

void MainWindow::loadAllLastSessionScripts()
{
    QFile session("session.data");
    session.open(QIODevice::ReadOnly | QIODevice::Text);
    if(session.isOpen())
    {
        QTextStream stream(&session);
        bool isAtLeastOneFile = false;
        while(!stream.atEnd())
        {
            auto currFilePath = stream.readLine();
            if(readScriptFromFile(currFilePath))
                isAtLeastOneFile = true;
        }
        if(!isAtLeastOneFile)
            on_actionNew_SQL_script_triggered();
        session.flush();
        session.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              "Could not restore scripts from last session");
    }
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
    auto currTabIndex = loadParameter(
            mSettingKeys[SettingIdentifier::CurrentTabIndex],
            mSettingGroups[SettingGroupIndentifier::TabWidget],
            0).toInt(&ok);
    if(ok)
        ui->tabWidget->setCurrentIndex(currTabIndex);
    ok = false;
    auto untitledTabIndexMax = loadParameter(
                mSettingKeys[SettingIdentifier::UntitledTabMaxIndex],
                mSettingGroups[SettingGroupIndentifier::TabWidget],
                0).toInt(&ok);
    if(ok)
        mUntitledTabMaxIndex = untitledTabIndexMax;
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
    saveParameter(mSettingKeys[SettingIdentifier::UntitledTabMaxIndex],
            mUntitledTabMaxIndex,
            mSettingGroups[SettingGroupIndentifier::TabWidget]);
}

void MainWindow::on_actionExport_to_pdf_triggered()
{
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
    if(fileName.isEmpty())
        return;
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    mTextEdits.at(index)->document()->print(&printer);}

void MainWindow::markUnsavedScriptTab()
{
    QTextEdit* sender = static_cast<QTextEdit*>(QObject::sender());
    auto index = ui->tabWidget->indexOf(sender);
    if(index == -1) return;
    ui->tabWidget->tabBar()->setTabTextColor(index, Qt::red);
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

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
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
