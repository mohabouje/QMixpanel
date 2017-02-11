#include "qmixpanel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMetaObject>
#include <QMetaProperty>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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


const char *QMixpanel::EngineApi = "http://api.mixpanel.com/engine/";
const char *QMixpanel::TrackApi = "http://api.mixpanel.com/track/";
QMixpanel *QMixpanel::_instance = Q_NULLPTR;
QMixpanel *QMixpanel::instance(QObject *object) {
    if (!_instance) {
        _instance = new QMixpanel(object);
        Q_ASSERT(_instance);
    }
    return _instance;
}

QMixpanel::QMixpanel(QObject* object) : QObject(object) {

}

void QMixpanel::insertProfile(const QMixpanelProfile &profile, bool instantSync) {
    if (instantSync) {
        postProfileHelper(profile);
    } else {
        _profileSet.insert(&profile);
    }
}

void QMixpanel::insertEvent(const QMixpanelEvent &event, bool instantSync) {
    if (instantSync) {
        postEventHelper(event);
    } else {
        _eventSet.insert(&event);
    }
}



void QMixpanel::flushProfiles() {
    _isSyncingProfiles = true;
    const ProfilesContainer tmp = _profileSet;
    const QJsonArray array = JsonHelper::ObjectSetToJsonArray<QMixpanelProfile>(tmp);
    QNetworkReply* reply = networkReplyHelper(EngineApi, JsonHelper::JsonArrayToJsonObject(array));
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _profileSet = _profileSet.subtract(tmp);
        }
        _isSyncingProfiles = false;
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, reply](const QNetworkReply::NetworkError& error){
        Q_UNUSED(error);
        _isSyncingProfiles = false;
        reply->deleteLater();
    });
}


void QMixpanel::flushEvents() {
    _isSyncingEvents = true;
    const EventsContainer tmp = _eventSet;
    const QJsonArray array = JsonHelper::ObjectSetToJsonArray<QMixpanelEvent>(tmp);
    QNetworkReply* reply = networkReplyHelper(TrackApi, JsonHelper::JsonArrayToJsonObject(array));
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _eventSet = _eventSet.subtract(tmp);
        }
        _isSyncingEvents = false;
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, reply](QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        _isSyncingEvents = false;
        reply->deleteLater();
    });
}

void QMixpanel::postProfileHelper(const QMixpanelProfile& profile) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(&profile);
    QNetworkReply* reply = networkReplyHelper(EngineApi, obj);

    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [reply](const QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        reply->deleteLater();
    });
}

void QMixpanel::postEventHelper(const QMixpanelEvent& event) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(&event);
    QNetworkReply* reply = networkReplyHelper(TrackApi, obj);

    connect(reply, &QNetworkReply::finished, [reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [reply](QNetworkReply::NetworkError error){
        Q_UNUSED(error);
        reply->deleteLater();
    });
}


QNetworkReply* QMixpanel::networkReplyHelper(const QString &API, const QJsonObject &obj) {
    const QJsonDocument doc = QJsonDocument(obj);
    const QByteArray base64 = "?data=" + doc.toJson().toBase64();
    const QNetworkRequest request(API + base64);
    QNetworkAccessManager* netAccessManager = new QNetworkAccessManager(this);
    return netAccessManager->get(request);
}




