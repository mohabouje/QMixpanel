#include "qmixpanel.h"

#include "util.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>


#define API_ENTRY_ENGINE "http://api.mixpanel.com/engine/?data="
#define API_ENTRY_TRACK "http://api.mixpanel.com/track/?data="

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

QString QMixpanel::distinctId() const
{
    return _distinctId;
}

void QMixpanel::setDistinctId(const QString &distinctId)
{
    _distinctId = distinctId;
}

QString QMixpanel::token() const
{
    return _token;
}

void QMixpanel::setToken(const QString &token)
{
    _token = token;
}

void QMixpanel::insertProfile(const QMixpanelProfile &profile, bool instantSync) {
    if (instantSync) {

    } else if (!_profileSet.contains(profile)) {
        _profileSet.insert(profile);
    }
}

void QMixpanel::insertEvent(const QMixpanelEvent &event, bool instantSync) {
    if (instantSync) {

    } else if (!_eventSet.contains(event)) {
        _eventSet.insert(profile);
    }
}


template<class T>
static QJsonArray ObjectSetToJsonArray(const QSet<T>& set) {
    QJsonArray array;
    foreach (const T& t, set) {
        const QJsonObject obj = JsonHelper::ObjectToJsonObject(t);
        array.append(obj);
    }
    return array;
}


void QMixpanel::flushProfiles() {
    _isSyncingProfiles = true;
    const ProfileSet tmp = _profileSet;
    const QJsonArray array = ObjectSetToJsonArray<QMixpanelProfile>(tmp);
    QNetworkReply reply = networkReplyHelper(API_ENTRY_ENGINE, array);
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _profileSet = _profileSet.subtract(tmp);
        }
        _isSyncingProfiles = false;
    });

    connect(reply, &QNetworkReply::error,[&](const QNetworkReply::NetworkError& error){
        Q_UNUSED(error);
        _isSyncingProfiles = false;
    });
}


void QMixpanel::flushEvents() {
    _isSyncingEvents = true;
    const EventSet tmp = _eventSet;
    const QJsonArray array = ObjectSetToJsonArray<QMixpanelEvent>(tmp);
    QNetworkReply reply = networkReplyHelper(API_ENTRY_TRACK, array);
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _eventSet = _eventSet.subtract(tmp);
        }
        _isSyncingEvents = false;
    });

    connect(reply, &QNetworkReply::error,[&](const QNetworkReply::NetworkError& error){
        Q_UNUSED(error);
        _isSyncingEvents = false;
    });
}

void QMixpanel::postProfileHelper(const QMixpanelProfile& profile) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(profile);
    QNetworkReply reply = networkReplyHelper(API_ENTRY_ENGINE, obj);
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
        }
    });
}

void QMixpanel::postEventHelper(const QMixpanelEvent& event) {
    const QJsonObject obj = JsonHelper::ObjectToJsonObject(event);
    QNetworkReply reply = networkReplyHelper(API_ENTRY_TRACK, obj);
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        const QByteArray data = reply->readAll();
        const int response = data.toInt();
        if (response) {
            _eventSet = _eventSet.subtract(tmp);
        }
    });

    connect(reply, &QNetworkReply::error,[&](const QNetworkReply::NetworkError& error){
        Q_UNUSED(error);
    });
}


void QMixpanel::networkReplyHelper(const QString &API, const QJsonObject &obj) {
    const QJsonDocument doc = QJsonDocument(obj);
    const QByteArray base64 = doc.toJson().toBase64();
    const QString apiString = QString(API).append(base64);

    QNetworkAccessManager* netAccessManager = new QNetworkAccessManager(this);
    return netAccessManager->get(apiString);
}




