#include "preferencesdialog.hpp"
#include "ui_preferencesdialog.h"
#include "settings_util.hpp"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    QFont restoredFont = loadParameter(SKEY_SCRIPT_EDITOR_FONT,
                                      SGROUP_SCRIPT_EDITOR,
                                      ui->comboFont->currentFont())
                                      .value<QFont>();
    ui->comboFont->setCurrentFont(restoredFont);
    ui->spinnerFontSize->setValue(restoredFont.pointSizeF());
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_btnAccept_clicked()
{
    on_btnApply_clicked();
    accept();
}

void PreferencesDialog::on_btnApply_clicked()
{
    auto currFont = ui->comboFont->currentFont();
    currFont.setPointSizeF(ui->spinnerFontSize->value());
    emit fontChanged(currFont);
}
