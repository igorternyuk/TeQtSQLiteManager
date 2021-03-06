#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "settings_util.hpp"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QList>
#include <QMap>
#include <QTreeWidgetItem>


namespace Ui
{
    class MainWindow;
}

class QTextEdit;
class QSqlQueryModel;
using TreeItem = QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionNew_database_triggered();
    void on_actionOpen_database_triggered();

    void on_actionNew_SQL_script_triggered();
    void on_actionOpen_SQL_script_triggered();
    void on_actionSave_SQL_script_triggered();
    void on_actionSave_SQL_script_as_triggered();
    void on_actionSave_all_scripts_triggered();
    void on_actionClose_current_SQL_script_triggered();
    void on_actionClose_all_scripts_triggered();
    void on_actionExport_to_pdf_triggered();
    void on_actionPrint_triggered();

    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionExecute_triggered();
    void on_actionPreferences_triggered();
    void on_actionManual_triggered();
    void on_actionAbout_program_triggered();
    void on_actionFind_and_Replace_triggered();


    void markUnsavedScriptTab();
    void on_tabWidget_tabCloseRequested(int index);

    void on_treeWidget_clicked(const QModelIndex &index);

private:
    enum class Selection
    {
        NoSelection,
        Database,
        Table
    };
    enum class Settings
    {
        WindowSize,
        WindowPos,
        CurrentTabIndex,
        UntitledTabMaxIndex
    };

    enum class SettingGroup
    {
        MainWindowGeometry,
        TabWidget
    };
    Ui::MainWindow *ui;
    QSqlDatabase mDatabase;
    QSqlQueryModel *mQueryModel;
    QList<QTextEdit*> mTextEdits;
    int mUntitledTabMaxIndex = 0;
    QMap<QString,TreeItem*> mDbNameTreeItemMap;
    QMap<Settings, QString> mSettingKeys
    {
        { Settings::WindowSize, "WindowSize" },
        { Settings::WindowPos, "WindowPosition" },
        { Settings::CurrentTabIndex, "CurrentTab" },
        { Settings::UntitledTabMaxIndex, "UntitledTabIndexMax" }
    };

    QMap<SettingGroup, QString> mSettingGroups
    {
        { SettingGroup::MainWindowGeometry, "MainWindowGeometry" },
        { SettingGroup::TabWidget , "TabWidget" }
    };

    int createNewTab(const QString &title, const QString &pathToFile = QString(),
                     const QString &text = QString());
    bool readScriptsFromFiles(const QStringList &listOfFiles);
    bool readScriptFromFile(const QString &path);
    bool saveScript(int index);
    bool saveScriptAs(int index);
    bool saveTextToFile(const QString &filePath, const QString &text);
    void executeScript(const QString &sql);
    void addDatabaseToTreeWidget(const QString &dbName);
    void showMessageInListWidget(const QString &msg, bool isSuccess = true);
    Selection getCurrentSelection();
    void saveAllCurrentSessionScripts();
    void loadAllLastSessionScripts();
    void saveAllCurrentSessionDb();
    void loadAllCurrentSessionDb();
    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_HPP
