#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace JsonHelper {
    static QJsonObject ObjectToJsonObject(const QObject* object, const QStringList &ignoredProperties = QStringList()) {
        QJsonObject json;
        const QMetaObject *metaobject = object->metaObject();
        const int count = metaobject->propertyCount();
        for (int i=0; i<count; i++) {
            const QMetaProperty metaproperty = metaobject->property(i);
            const char *name = metaproperty.name();
            const QLatin1String field = QLatin1String(name);
            if (ignoredProperties.contains(field) || (!metaproperty.isReadable()))
                continue;
            json.insert(field, QJsonValue::fromVariant(object->property(name)));
        }
        return json;
    }
    static QJsonObject JsonArrayToJsonObject(const QJsonArray& array) {
        const QJsonDocument doc(array);
        return doc.object();
    }
    static void JsonObjectToObject(const QJsonObject &json, QObject *object) {
        const QMetaObject *metaobject = object->metaObject();
        const int count = metaobject->propertyCount();
        for (QJsonObject::const_iterator iter = json.begin(); iter != json.end(); ++iter) {
            const QByteArray field = iter.key().toLatin1();
            const int index =  metaobject->indexOfProperty(field);
            if (index < 0 || index >= count)
                continue;
            const QMetaProperty metaproperty = metaobject->property( index );
            const QVariant::Type type = metaproperty.type();
            QVariant v = iter.value().toVariant();
            if (v.canConvert(type)) {
                v.convert(type);
                metaproperty.write(object, v);
            } else if (QString(QLatin1String("QVariant")).compare(QLatin1String(metaproperty.typeName())) == 0) {
                metaproperty.write(object, v);
            }
        }
    }
    template<class Object>
    static QJsonArray ObjectSetToJsonArray(const QSet<const Object*>& set) {
        QJsonArray array;
        foreach (const Object* t, set) {
            const QJsonObject obj = JsonHelper::ObjectToJsonObject(t);
            array.append(obj);
        }
        return array;
    }
}
#endif // UTIL_H
