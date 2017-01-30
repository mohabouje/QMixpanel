#ifndef UTIL_H
#define UTIL_H

#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>


namespace JsonHelper {
    static void QObjectToQJsonObject(QJsonObject& jsonObject,  const QObject* object) {
        const QMetaObject *metaobject = object->metaObject();
        const int count = metaobject->propertyCount();
        for (int i=0; i<count; i++) {
            const QMetaProperty metaproperty = metaobject->property(i);
            const char *name = metaproperty.name();
            const QLatin1String field = QLatin1String(name);
            if (ignoredProperties.contains(field) || (!metaproperty.isReadable()))
                continue;
            jsonObject.insert(field, QJsonValue::fromVariant(object->property(name)));
        }
        return jsonObject;
    }

    static void QJsonObjectToQObject(QObject* object, const QJsonObject& jsonObject) {
        const QMetaObject *metaobject = object->metaObject();
        const int count = metaobject->propertyCount();
        for (QJsonObject::const_iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
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
}


#endif // UTIL_H
