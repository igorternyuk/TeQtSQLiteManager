#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QSqlDatabase>
#include <QList>
#include <QMap>

namespace Ui
{
    class MainWindow;
}

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionOpen_database_triggered();
    void on_actionSave_database_triggered();

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
    void on_actionNew_database_triggered();
    void on_actionSave_database_as_triggered();

    void markUnsavedScriptTab();
    void on_tabWidget_tabCloseRequested(int index);

private:
    enum class SettingIdentifier
    {
        WindowSize,
        WindowPos,
        CurrentTabIndex,
        UntitledTabMaxIndex
    };

    QMap<SettingIdentifier, QString> mSettingKeys
    {
        { SettingIdentifier::WindowSize, "WindowSize" },
        { SettingIdentifier::WindowPos, "WindowPosition" },
        { SettingIdentifier::CurrentTabIndex, "CurrentTab" },
        { SettingIdentifier::UntitledTabMaxIndex, "UntitledTabIndexMax" }
    };

    const QString mSettingGroup { "Main window geometry"};

    enum class SettingGroupIndentifier
    {
        MainWindowGeometry,
        TabWidget
    };

    QMap<SettingGroupIndentifier, QString> mSettingGroups
    {
        { SettingGroupIndentifier::MainWindowGeometry, "MainWindowGeometry" },
        { SettingGroupIndentifier::TabWidget , "TabWidget" }
    };

    Ui::MainWindow *ui;
    QSqlDatabase mDatabase;
    QList<QTextEdit*> mTextEdits;
    int mUntitledTabMaxIndex = 0;

    int createNewTab(const QString &title, const QString &pathToFile = QString(),
                     const QString &text = QString());
    bool readScriptsFromFiles(const QStringList &listOfFiles);
    bool readScriptFromFile(const QString &path);
    bool saveScript(int index);
    bool saveScriptAs(int index);
    bool saveTextToFile(const QString &filePath, const QString &text);
    void saveAllCurrentSessionScripts();
    void loadAllLastSessionScripts();
    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_HPP
