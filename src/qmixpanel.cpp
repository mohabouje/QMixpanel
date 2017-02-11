#include "qmixpanel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMetaObject>
#include <QMetaProperty>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

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

QMixpanel::QMixpanel(QObject* object) :
    QObject(object),
    _timer(new QTimer(object))
{
    _timer->setInterval(30000);
    _timer->setSingleShot(false);
    connect(_timer, &QTimer::timeout, this, &QMixpanel::flushEvents);
    connect(_timer, &QTimer::timeout, this, &QMixpanel::flushProfiles);
}

void QMixpanel::setDistinctId(const QString &distinctId)
{
    _distinctId = distinctId;
}

void QMixpanel::setToken(const QString &token)
{
    _token = token;
}

bool QMixpanel::insertProfile(QMixpanelProfile* profile, bool instantSync, bool validate) {
    if (validate) {
        profile->setDistinctId(distinctId());
        profile->setToken(token());
    }

    const bool success = instantSync ? postProfileHelper(profile) : (_profileSet.insert(profile) != _profileSet.end());
    flushProfiles();
    return success;
}

bool QMixpanel::insertEvent(QMixpanelEvent* event, bool instantSync, bool validate) {
    if (validate) {
        event->setDistinctId(distinctId());
        event->setToken(token());
    }

    const bool success = instantSync ? postEventHelper(event) : (_eventSet.insert(event) != _eventSet.end());
    flushEvents();
    return success;
}

void QMixpanel::flushProfiles() {
    if (_profileSet.size() < MaxEventCount || !_isSyncingProfiles) {
        return;
    }

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
    if (_eventSet.size() < MaxEventCount || !_isSyncingEvents) {
        return;
    }

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

bool QMixpanel::postProfileHelper(const QMixpanelProfile* profile) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(profile);
    QNetworkReply* reply = networkReplyHelper(EngineApi, obj);

    connect(reply, &QNetworkReply::finished, [&, profile]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
        emit successSendingProfile(*profile);
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, profile](const QNetworkReply::NetworkError error){
        reply->deleteLater();
        emit errorSendingProfile(*profile, error);
    });
    return reply->isRunning() || reply->isFinished();
}

bool QMixpanel::postEventHelper(const QMixpanelEvent* event) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(event);
    QNetworkReply* reply = networkReplyHelper(TrackApi, obj);

    connect(reply, &QNetworkReply::finished, [&, event]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        Q_UNUSED(response);
        reply->deleteLater();
        emit successSendingEvent(*event);
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [&, event](QNetworkReply::NetworkError error){
        reply->deleteLater();
        emit errorSendingEvent(*event, error);
    });
    return reply->isRunning() || reply->isFinished();
}


QNetworkReply* QMixpanel::networkReplyHelper(const QString &API, const QJsonObject &obj) {
    const QJsonDocument doc = QJsonDocument(obj);
    const QByteArray base64 = "?data=" + doc.toJson().toBase64();
    const QNetworkRequest request(API + base64);
    QNetworkAccessManager* netAccessManager = new QNetworkAccessManager(this);
    return netAccessManager->get(request);
}




