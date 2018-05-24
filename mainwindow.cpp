#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "findreplacedialog.hpp"
#include "preferencesdialog.hpp"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QFileDialog>
#include <QTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>

#define DB_DRIVER "QSQLITE"

using ListItem = QListWidgetItem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->layout()->setMargin(0);
    ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tabWidget->setTabsClosable(true);
    mDatabase = QSqlDatabase::addDatabase(DB_DRIVER);
    mQueryModel = new QSqlQueryModel(this);
    ui->tableView->setModel(mQueryModel);
    loadAllLastSessionScripts();
    loadAllCurrentSessionDb();
    loadSettings();

    connect(ui->treeWidget, &TeTreeWidget::newTable, [&]()
    {
        auto currItem = ui->treeWidget->currentItem();
        if(!currItem)
        {
            QMessageBox::warning(this, "Warning", "Database not selected");
            return;
        }
        auto parent = currItem->parent();
        if(!parent)
        {

        }
        //TODO...
    });
}

MainWindow::~MainWindow()
{
    saveSettings();
    saveAllCurrentSessionDb();
    saveAllCurrentSessionScripts();
    delete ui;
}

void MainWindow::on_actionNew_database_triggered()
{
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("Databases (*.db);;All files(*.*)");
    auto databaseName = QFileDialog::getSaveFileName(this,
                                                     "Specify database name",
                                                     initDir, filter);
    if(databaseName.isEmpty())
        return;
    QFile dbFile(databaseName);
    addDatabaseToTreeWidget(databaseName);
}

void MainWindow::on_actionOpen_database_triggered()
{
    auto initDir =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
            .value(0, QDir::homePath());
    QString filter("Databases (*.db);;All files(*.*)");
    auto databaseList = QFileDialog::getOpenFileNames(this, "Choose databases",
                                                      initDir, filter);

    foreach (auto db_name, databaseList)
    {
        if(!db_name.isEmpty())
            addDatabaseToTreeWidget(db_name);
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
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;
    FindReplaceDialog dialog;
    dialog.setTextEdit(mTextEdits.at(index));
    dialog.exec();
}

void MainWindow::on_actionExecute_triggered()
{
    auto index = ui->tabWidget->currentIndex();
    if(index == -1) return;

    qDebug() << "Current index " << index;
    auto sql = mTextEdits.at(index)->toPlainText();
    qDebug() << "SQL = " << sql;

    if(sql.startsWith("SELECT", Qt::CaseInsensitive))
    {
        mQueryModel->setQuery(sql);
    }
    else if(sql.startsWith("CREATE TABLE", Qt::CaseInsensitive))
    {
        executeScript(sql);
        auto dbName = mDatabase.databaseName();

        QRegExp re("(create table){1}([\\s|\\t|\\n]+(if){1}[\\s|\\t|\\n]+"
                   "(not){1}[\\s|\\t|\\n]+(exists){1})?[\\s|\\t|\\n]+",
                   Qt::CaseInsensitive);
        auto pos = re.indexIn(sql, 0);
        if(pos == -1) return;

        QString match = re.cap(0);
        auto posBegin = match.size();
        auto posEnd = sql.indexOf("(");
        QString tableName = sql.mid(posBegin, posEnd - posBegin).trimmed()
                .toLower();
        TreeItem* currTreeItem = mDbNameTreeItemMap[dbName];
        bool existTableWithSameName = false;
        auto childCount = currTreeItem->childCount();
        for(int i = 0; i < childCount; ++i)
        {
            if(currTreeItem->child(i)->text(0).toLower() == tableName.toLower())
            {
                existTableWithSameName = true;
                break;
            }
        }
        if(!existTableWithSameName)
        {
            auto tableItem = new TreeItem;
            tableItem->setIcon(0, QIcon(":icons/table1"));
            tableItem->setText(0, tableName);
            currTreeItem->addChild(tableItem);
        }
        else
        {
            showMessageInListWidget(QString("Table %1 already exists")
                                    .arg(tableName), false);
            /*QMessageBox::warning(this, "Warning",
                                 QString("Table %1 already exists")
                                 .arg(tableName));*/
        }
    }
    else if(sql.startsWith("DROP TABLE", Qt::CaseInsensitive))
    {
        executeScript(sql);

        auto dbName = mDatabase.databaseName();
        TreeItem* currTreeItem = mDbNameTreeItemMap[dbName];
        QRegExp re("(drop table){1}([\\s|\\t|\\n]+(if){1}[\\s|\\t|\\n]+"
                   "(exists){1})?[\\s|\\t|\\n]+",
                   Qt::CaseInsensitive);
        auto pos = re.indexIn(sql, 0);
        if(pos == -1) return;

        QString match = re.cap(0);
        auto posBegin = match.size();
        QString tableName = sql.mid(posBegin).trimmed();
        if(tableName.endsWith(";"))
            tableName.remove(tableName.size() - 1, 1);
        qDebug() << "Table to remove: " << tableName;
        tableName = tableName.trimmed();
        int childToRemoveIndex = -1;
        int childCount = currTreeItem->childCount();
        for(int i = 0; i < childCount; ++i)
            if(currTreeItem->child(i)->text(0).toLower() == tableName.toLower())
            {
                childToRemoveIndex = i;
                break;
            }
        if(childToRemoveIndex != -1)
        {
            int reply = QMessageBox::question(this, "Confirm deleting",
                                              QString("Do yo really want to delete"
                                              " table %1?").arg(tableName),
                                              QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                auto itemToRemove = currTreeItem->child(childToRemoveIndex);
                currTreeItem->removeChild(itemToRemove);
             }
        }
    }
    else
    {
        executeScript(sql);
    }
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

void MainWindow::executeScript(const QString &sql)
{
    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug() << "table name repetition";
        showMessageInListWidget(QString("%1 Error: %2").arg(sql)
                                .arg(query.lastError().text()), false);
        return;
    }
    showMessageInListWidget(QString("%1").arg(sql));
}

void MainWindow::showMessageInListWidget(const QString &msg, bool isSuccess)
{
    auto item = new ListItem(isSuccess
                             ? QIcon(":icons/success4")
                             : QIcon(":icons/fail2"),
                             msg, ui->listWidget);
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentItem(item);
    ui->listWidget->clearSelection();
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
    if(!QDir(".lastSession").exists())
    {
        if(!QDir().mkdir(".lastSession"))
        {
            QMessageBox::critical(this, "Error",
                                  "Could not create session folder");
            return;
        }
    }
    QFile scriptListFile(".lastSession/script_list.data");
    if(scriptListFile.open(QFile::WriteOnly))
    {
        QTextStream stream(&scriptListFile);
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
        scriptListFile.flush();
        scriptListFile.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not save current session: %1")
                              .arg(scriptListFile.errorString()));
    }
}

void MainWindow::loadAllLastSessionScripts()
{
    QFile scriptListFile(".lastSession/script_list.data");
    if(scriptListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&scriptListFile);
        bool isAtLeastOneFile = false;
        while(!stream.atEnd())
        {
            auto currFilePath = stream.readLine();
            if(readScriptFromFile(currFilePath))
                isAtLeastOneFile = true;
        }
        if(!isAtLeastOneFile)
            on_actionNew_SQL_script_triggered();
        scriptListFile.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              "Could not restore scripts from last session");
    }
}

void MainWindow::saveAllCurrentSessionDb()
{
    if(!QDir(".lastSession").exists())
    {
        if(!QDir().mkdir(".lastSession"))
        {
            QMessageBox::critical(this, "Error",
                                  "Could not create session folder");
            return;
        }
    }
    QFile dbListFile(".lastSession/db_list.data");
    if(dbListFile.open(QFile::WriteOnly))
    {
        QTextStream stream(&dbListFile);
        for (auto dbPath: mDbNameTreeItemMap.keys()) {
            stream << dbPath << "\n";
        }
        dbListFile.flush();
        dbListFile.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not save database list: %1")
                              .arg(dbListFile.errorString()));
    }
}

void MainWindow::loadAllCurrentSessionDb()
{
    QFile dbListFile(".lastSession/db_list.data");
    if(dbListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&dbListFile);
        while(!stream.atEnd())
        {
            auto currFilePath = stream.readLine();
            addDatabaseToTreeWidget(currFilePath);
        }
        dbListFile.close();
    }
    else
    {
        QMessageBox::critical(this, "Error",
                              "Could not restore scripts from last session");
    }
}

void MainWindow::addDatabaseToTreeWidget(const QString &dbName)
{
    auto databaseItem = new TreeItem(ui->treeWidget);
    databaseItem->setText(0, dbName);
    databaseItem->setIcon(0, QIcon(":icons/database3"));
    mDatabase.setDatabaseName(dbName);
    if(!mDatabase.open())
    {
        QMessageBox::critical(this, "Error",
                              QString("Could not open database: %1")
                              .arg(dbName));
        return;
    }
    QSqlQuery query;
    query.exec("SELECT tbl_name FROM sqlite_master WHERE type LIKE '%table%'");
    while(query.next())
    {
        auto tableName = query.value(0).toString();
        auto databaseTableItem = new TreeItem(databaseItem);
        databaseTableItem->setText(0, tableName);
        databaseTableItem->setIcon(0, QIcon(":icons/table1"));
        databaseItem->addChild(databaseTableItem);
    }
    ui->treeWidget->addTopLevelItem(databaseItem);
    mDbNameTreeItemMap.insert(dbName, databaseItem);
    ui->treeWidget->setNewTableActionEnabled(true);
}

void MainWindow::loadSettings()
{
    auto size = loadParameter(mSettingKeys[Settings::WindowSize],
            mSettingGroups[SettingGroup::MainWindowGeometry],
            this->size()).value<QSize>();
    this->resize(size);

    auto pos = loadParameter(mSettingKeys[Settings::WindowPos],
            mSettingGroups[SettingGroup::MainWindowGeometry],
            this->pos()).value<QPoint>();
    this->move(pos);

    bool ok = false;
    auto currTabIndex = loadParameter(
            mSettingKeys[Settings::CurrentTabIndex],
            mSettingGroups[SettingGroup::TabWidget],
            0).toInt(&ok);
    if(ok)
        ui->tabWidget->setCurrentIndex(currTabIndex);
    ok = false;
    auto untitledTabIndexMax = loadParameter(
                mSettingKeys[Settings::UntitledTabMaxIndex],
                mSettingGroups[SettingGroup::TabWidget],
                0).toInt(&ok);
    if(ok)
        mUntitledTabMaxIndex = untitledTabIndexMax;

    QFont restoredFont = loadParameter(SKEY_SCRIPT_EDITOR_FONT,
                                      SGROUP_SCRIPT_EDITOR,
                                      QFont("Times", 14))
                                      .value<QFont>();
    for(auto textEdit: mTextEdits)
        textEdit->setFont(restoredFont);
}

void MainWindow::saveSettings()
{
    saveParameter(mSettingKeys[Settings::WindowSize],
            this->size(),
            mSettingGroups[SettingGroup::MainWindowGeometry]);
    saveParameter(mSettingKeys[Settings::WindowPos],
            this->pos(),
            mSettingGroups[SettingGroup::MainWindowGeometry]);
    saveParameter(mSettingKeys[Settings::CurrentTabIndex],
            ui->tabWidget->currentIndex(),
            mSettingGroups[SettingGroup::TabWidget]);
    saveParameter(mSettingKeys[Settings::UntitledTabMaxIndex],
            mUntitledTabMaxIndex,
            mSettingGroups[SettingGroup::TabWidget]);
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
    PreferencesDialog dialog;
    connect(&dialog, &PreferencesDialog::fontChanged, [&](const QFont &font)
    {
        for(auto textEdit: mTextEdits)
            textEdit->setFont(font);
        saveParameter(SKEY_SCRIPT_EDITOR_FONT, font, SGROUP_SCRIPT_EDITOR);
    });
    if(dialog.exec() == PreferencesDialog::Rejected)
        return;
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

void MainWindow::on_treeWidget_clicked(const QModelIndex &index)
{
    if(index.parent().isValid())
        return;
    mDatabase.setDatabaseName(index.data().toString());
    if(!mDatabase.open())
    {
        //TODO...
    }
}
