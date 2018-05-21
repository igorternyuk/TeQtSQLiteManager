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
    void on_actionOpen_SQL_script_triggered();
    void on_actionSave_SQL_script_triggered();
    void on_actionPrint_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionExecute_triggered();
    void on_actionPreferences_triggered();
    void on_actionManual_triggered();
    void on_actionAbout_program_triggered();
    void on_actionFind_and_Replace_triggered();
    void on_actionNew_SQL_script_triggered();

    void on_actionExport_to_pdf_triggered();

private:
    int create_new_tab(const QString &title, const QString &pathToFile = QString(),
                        const QString &text = QString());
    bool openFiles(const QStringList &listOfFiles);
    bool openFile(const QString &path);
    bool saveFile(int index);
    bool saveFileAs(int index);
    bool saveTextToFile(const QString &filePath, const QString &text);
    void saveAllCurrentSessionFiles();
    void loadAllLastSessionFiles();
    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;
    QSqlDatabase mDatabase;

    enum class SettingIdentifier
    {
        WindowSize,
        WindowPos,
        CurrentTabIndex
    };

    QMap<SettingIdentifier, QString> mSettingKeys
    {
        { SettingIdentifier::WindowSize, "WindowSize" },
        { SettingIdentifier::WindowPos, "WindowPosition" },
        { SettingIdentifier::CurrentTabIndex, "CurrentTab" }
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
};

#endif // MAINWINDOW_HPP
