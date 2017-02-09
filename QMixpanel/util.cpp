#include "util.h"


QJsonObject JsonHelper::ObjectToJsonObject(const QObject *object, const QStringList &ignoredProperties) {
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

void JsonHelper::JsonObjectToObject(const QJsonObject &json, QObject *object) {
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

