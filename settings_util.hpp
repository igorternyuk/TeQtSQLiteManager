#ifndef SETTINGSUITL_H
#define SETTINGSUITL_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QMap>

#define SGROUP_SCRIPT_EDITOR "ScriptEditor"
#define SKEY_SCRIPT_EDITOR_FONT "ScriptEditorFont"

void saveParameter(const QString &key, const QVariant &value,
                   const QString &group);
QVariant loadParameter(const QString &key, const QString &group,
                       const QVariant &defaultValue);

#endif // SETTINGSUITL_H
