#include "findreplacedialog.hpp"
#include "ui_findreplacedialog.h"
#include <QTextEdit>
#include <QTextDocument>
#include <QPushButton>
#include <QRegExp>
#include <QDebug>
#include "settings_util.hpp"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Find&Replace dialog");
    ui->lblSearchResults->setText("");
    connect(ui->txtFind, SIGNAL(textChanged(QString)), this,
            SLOT(textToFindChanged()));
    connect(ui->checkRegEx, &QCheckBox::clicked, [this](bool isSelected){
        this->validateRegExp(isSelected ? ui->txtFind->text() : "");
    });
    auto slot = static_cast<void(FindReplaceDialog::*)()>(&FindReplaceDialog::find);
    connect(ui->btnFind, &QPushButton::clicked, this, slot);
    connect(ui->btnFindNext, &QPushButton::clicked, this,
            &FindReplaceDialog::findNext);
    connect(ui->btnFindPrevoius, &QPushButton::clicked, this,
            &FindReplaceDialog::findPrev);
    connect(ui->btnReplace, &QPushButton::clicked, this,
            &FindReplaceDialog::replace);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this,
            &FindReplaceDialog::replaceAll);
    readSettings();
}

FindReplaceDialog::~FindReplaceDialog()
{
    //qDebug() << "Find&Replace dialog destructor was called";
    writeSettings();
    delete ui;
}

void FindReplaceDialog::setTextEdit(QTextEdit *textEdit)
{
    this->mTextEdit = textEdit;
    connect(mTextEdit, &QTextEdit::copyAvailable, ui->btnReplace,
            &QPushButton::setEnabled);
    connect(mTextEdit, &QTextEdit::copyAvailable, ui->btnReplaceAll,
            &QPushButton::setEnabled);
}

void FindReplaceDialog::find()
{
    find(ui->radioDown->isChecked());
}

void FindReplaceDialog::find(bool down)
{
    if(!mTextEdit) return;
    const QString &toSearch = ui->txtFind->text();
    bool isMatch { false };
    QTextDocument::FindFlags flags;
    if(!down)
        flags |= QTextDocument::FindBackward;
    if(ui->checkCaseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if(ui->checkWholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;

    QTextCursor cursor;
    if(ui->checkRegEx->isChecked())
    {
        QRegExp re(toSearch, ui->checkRegEx->isChecked() ?
                   Qt::CaseSensitive : Qt::CaseInsensitive);
        cursor = mTextEdit->document()->find(re, mTextCursor, flags);
    }
    else
    {
        cursor = mTextEdit->document()->find(toSearch, mTextCursor, flags);
    }

    isMatch = !cursor.isNull();

    if(isMatch)
    {
        mTextEdit->setTextCursor(cursor);
        mTextCursor = cursor;
        this->showInfo("");
    }
    else
    {
        this->showInfo("No match found", true);
    }
}

void FindReplaceDialog::findNext()
{
    this->find(true);
}

void FindReplaceDialog::findPrev()
{
    this->find(false);
}

void FindReplaceDialog::replace()
{
    if(!mTextEdit->textCursor().hasSelection())
    {
        find();
    }
    else
    {
        mTextEdit->textCursor().insertText(ui->txtReplace->text());
        find();
    }
}

void FindReplaceDialog::replaceAll()
{
    size_t counter {0};
    while(mTextEdit->textCursor().hasSelection())
    {
        mTextEdit->textCursor().insertText(ui->txtReplace->text());
        find();
        ++counter;
    }
    this->showInfo(QString("%1 occurences were replaced").arg(counter));
}

/*void FindReplaceDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}*/

void FindReplaceDialog::writeSettings()
{
    saveParameter(mKeys[SettingKey::TextToFind], ui->txtFind->text(),
            mSettingGroups[SettingGroup::Search]);
    saveParameter(mKeys[SettingKey::TextToReplace], ui->txtReplace->text(),
            mSettingGroups[SettingGroup::Search]);
    saveParameter(mKeys[SettingKey::DownRadio], ui->radioDown->isChecked(),
            mSettingGroups[SettingGroup::Search]);
    saveParameter(mKeys[SettingKey::CaseCheck],
            ui->checkCaseSensitive->isChecked(),
            mSettingGroups[SettingGroup::Search]);
    saveParameter(mKeys[SettingKey::WholeWordCheck],
            ui->checkWholeWords->isChecked(),
            mSettingGroups[SettingGroup::Search]);
    saveParameter(mKeys[SettingKey::RegExpCheck], ui->checkRegEx->isChecked(),
            mSettingGroups[SettingGroup::Search]);
}

void FindReplaceDialog::readSettings()
{
    auto textToFind = loadParameter(mKeys[SettingKey::TextToFind],
                                    mSettingGroups[SettingGroup::Search],
                                    QString()).toString();
    ui->txtFind->setText(textToFind);

    auto textToReplaceWith = loadParameter(mKeys[SettingKey::TextToReplace],
                                           mSettingGroups[SettingGroup::Search],
                                           QString()).toString();
    ui->txtReplace->setText(textToReplaceWith);

    auto isSearchDirectionDown = loadParameter(mKeys[SettingKey::DownRadio],
                                           mSettingGroups[SettingGroup::Search],
                                           false).toBool();
    ui->radioDown->setChecked(isSearchDirectionDown);

    auto isCaseSensitive = loadParameter(mKeys[SettingKey::CaseCheck],
                                         mSettingGroups[SettingGroup::Search],
                                         false).toBool();
    ui->checkCaseSensitive->setChecked(isCaseSensitive);

    auto checkWholeWorlds = loadParameter(mKeys[SettingKey::WholeWordCheck],
                                          mSettingGroups[SettingGroup::Search],
                                          false).toBool();
    ui->checkWholeWords->setChecked(checkWholeWorlds);

    auto chekcRegEx = loadParameter(mKeys[SettingKey::RegExpCheck],
                                    mSettingGroups[SettingGroup::Search],
                                    false).toBool();
    ui->checkRegEx->setChecked(chekcRegEx);
}

void FindReplaceDialog::textToFindChanged()
{
    bool enabled = ui->txtFind->text().size() > 0;
    ui->btnFind->setEnabled(enabled);
    ui->btnFindNext->setEnabled(enabled);
    ui->btnFindPrevoius->setEnabled(enabled);
}

void FindReplaceDialog::validateRegExp(const QString &regExp)
{
    if(!ui->checkRegEx->isChecked() || regExp.isEmpty())
    {
        ui->lblSearchResults->setText("");
        return;
    }
    QRegExp re(regExp, (ui->checkCaseSensitive->isChecked() ?
                        Qt::CaseSensitive : Qt::CaseInsensitive));
    if(re.isValid())
        ui->lblSearchResults->setText("");
    else
        this->showInfo(re.errorString(), true);
}

void FindReplaceDialog::showInfo(const QString &msg, bool isError)
{
    ui->lblSearchResults->setText("<span style =\" font-weight:600;" +
                                  (isError ? QString("color:red;") :
                                             QString("color:green;")) + " \">" +
                                  msg + "</span>");
}
