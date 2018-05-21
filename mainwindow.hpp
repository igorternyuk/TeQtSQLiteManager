#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
