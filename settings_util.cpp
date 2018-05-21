#include "settings_util.hpp"

void saveParameter(const QString &key, const QVariant &value,
                   const QString &group)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}

QVariant loadParameter(const QString &key, const QString &group,
                       const QVariant &defaultValue)
{
    QSettings settings;
    settings.beginGroup(group);
    QVariant value = settings.value(key, defaultValue);
    settings.endGroup();
    return value;
}
