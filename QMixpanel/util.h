#ifndef UTIL_H
#define UTIL_H
#include <QJsonObject>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>

namespace JsonHelper {
    static QJsonObject ObjectToJsonObject(const QObject *object, const QStringList &ignoredProperties = QStringList());
    static void JsonObjectToObject(const QJsonObject &json, QObject *object);
}
#endif // UTIL_H
