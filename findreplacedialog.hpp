#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QMap>
#include <QTextCursor>

namespace Ui
{
    class FindReplaceDialog;
}

class QTextEdit;
class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog();
    void setTextEdit(QTextEdit *textEdit);

public slots:
    void find();
    void find(bool down);
    void findNext();
    void findPrev();
    void replace();
    void replaceAll();

/*protected:
    void closeEvent(QCloseEvent *event);*/

private:
    enum class SettingKey
    {
        TextToFind,
        TextToReplace,
        DownRadio,
        CaseCheck,
        WholeWordCheck,
        RegExpCheck
    };

    QMap<SettingKey, QString> mKeys
    {
        { SettingKey::TextToFind, "textToFind" },
        { SettingKey::TextToReplace, "textToReplace" },
        { SettingKey::DownRadio, "downRadio " },
        { SettingKey::CaseCheck, "caseCheck" },
        { SettingKey::WholeWordCheck, "wholeWordCheck" },
        { SettingKey::RegExpCheck, "regexCheck " }
    };

    enum class SettingGroup
    {
        Search
    };

    QMap<SettingGroup, QString> mSettingGroups
    {
        { SettingGroup::Search, "SearchPreferences"}
    };

    Ui::FindReplaceDialog *ui;
    QTextEdit *mTextEdit;
    QTextCursor mTextCursor;

    void writeSettings();
    void readSettings();
private slots:
    void textToFindChanged();
    void validateRegExp(const QString &regExp);
    void showInfo(const QString &msg, bool isError = false);
};

#endif // FINDREPLACEDIALOG_H
