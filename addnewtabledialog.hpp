#ifndef ADDNEWTABLEDIALOG_HPP
#define ADDNEWTABLEDIALOG_HPP

#include <QDialog>

namespace Ui
{
    class AddNewTableDialog;
}

class AddNewTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewTableDialog(QWidget *parent = nullptr);
    ~AddNewTableDialog();

private slots:
    void on_btnAddField_clicked();
    void on_btnRemoveField_clicked();
    void on_btnAccept_clicked();
    void on_btnCancel_clicked();

private:
    enum Columns
    {
        FIELD_NAME,
        TYPE,
        DEFAULT,
        CHECK,
        PK,
        AI,
        NN,
        UNIQUE,
        FK
    };
    Ui::AddNewTableDialog *ui;
};

#endif // ADDNEWTABLEDIALOG_HPP
